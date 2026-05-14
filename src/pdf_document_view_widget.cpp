/**
 * @file pdf_document_view_widget.cpp
 * @brief `PdfDocumentViewWidget` implementation: layout, paint, navigation, find, host highlights, tiles.
 *
 * @details All entry points that touch document state, scroll position, or `QImage` caches are
 * intended for the **GUI thread**. Optional **async** tile path marshals completed tiles back
 * to the GUI thread via queued connections when the backend supports isolated per-job raster. `revealPageRect` / `navigateToHostSearchHit` /
 * `openDocumentAndRevealSearchHit` map **page-space** `QRectF` through the same layout as find hits
 * (`ensurePageRectVisibleInLayout`). `findText` scrolls to the first match; `openDocumentAndRevealSearchHit`
 * runs `refreshFindMatches` without auto-scrolling so host-provided geometry wins unless a rect match
 * selects an index. Errors from `setDocumentPath` / `openDocumentBuffer` surface via `documentError` and
 * the model’s `lastError()`.
 */

#include "pdf_document_view/pdf_document_view_widget.hpp"

#include "pdf_document_view/pdf_document_model.hpp"
#include "create_default_pdf_render_backend.hpp"
#include "pdf_render_backend.hpp"
#include "pdf_tile_cache.hpp"

#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
#include "pdf_tile_render_worker.hpp"
#endif

#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
#include "pdfium_backend.hpp"
#include "pdfium_document_model.hpp"
#elif defined(PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER) && defined(PDFDOCUMENTVIEW_POPPLER_REAL) \
    && PDFDOCUMENTVIEW_POPPLER_REAL
#include "poppler_qt_backend.hpp"
#include "poppler_qt_document_model.hpp"
#endif

#include <QEvent>
#include <QFileInfo>
#include <QFrame>
#include <QImage>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPalette>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSet>
#include <QSizePolicy>
#include <QTimer>
#include <QWheelEvent>

#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>
#endif

#include <vector>

#include <cmath>

namespace pdf_document_view {

struct PdfDocumentViewTileRuntime {
    TileImageCache cache{64};
    std::vector<TileKey> pending;
    QSet<TileKey> pendingKeys;
    bool drainScheduled = false;
};

namespace {

constexpr int kTileDevicePixels = 512;
constexpr QColor kTilePlaceholderColor(0xe8, 0xe8, 0xe8);

} // namespace

PdfDocumentViewWidget::PdfDocumentViewWidget(QWidget* parent)
    : QAbstractScrollArea(parent)
    , m_documentModel(createDefaultPdfDocumentModel())
    , m_backend(createDefaultPdfRenderBackend())
    , m_findHighlightFillColor(255, 255, 0, 110)
    , m_findActiveMatchOutlineColor(255, 140, 0)
    , m_findActiveMatchOutlineWidth(2.0)
    , m_tileRuntime(std::make_unique<PdfDocumentViewTileRuntime>()) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
    viewport()->setMouseTracking(true);
    viewport()->setFocusPolicy(Qt::StrongFocus);
    viewport()->installEventFilter(this);
    setFocusProxy(viewport());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    if (m_backend) {
        setToolTip(QStringLiteral("%1 — %2").arg(m_backend->displayName(), m_backend->id()));
    }

#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
    if (m_backend && m_documentModel) {
        if (auto* backend = dynamic_cast<PdfiumBackend*>(m_backend.get())) {
            if (auto* model = dynamic_cast<PdfiumDocumentModel*>(m_documentModel.get())) {
                backend->attachDocumentModel(model);
            }
        }
    }
#elif defined(PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER) && defined(PDFDOCUMENTVIEW_POPPLER_REAL) \
    && PDFDOCUMENTVIEW_POPPLER_REAL
    if (m_backend && m_documentModel) {
        if (auto* backend = dynamic_cast<PopplerQtBackend*>(m_backend.get())) {
            if (auto* model = dynamic_cast<PopplerQtDocumentModel*>(m_documentModel.get())) {
                backend->attachDocumentModel(model);
            }
        }
    }
#endif

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, viewport(), [this] { viewport()->update(); });
    connect(verticalScrollBar(), &QScrollBar::valueChanged, viewport(), [this] { viewport()->update(); });

#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    setupPdfTileRenderThread();
#endif
}

#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER

PdfDocumentViewWidget::~PdfDocumentViewWidget() {
    shutdownPdfTileRenderThread();
}

void PdfDocumentViewWidget::setupPdfTileRenderThread() {
    m_tileRenderThread = new QThread(this);
    m_tileRenderThread->setObjectName(QStringLiteral("PdfTileRenderThread"));
    m_tileRenderWorker = new PdfTileRenderWorker();
    m_tileRenderWorker->moveToThread(m_tileRenderThread);
    connect(m_tileRenderWorker,
        &PdfTileRenderWorker::tileReady,
        this,
        [this](quint64 generation,
            QString docIdentity,
            int pageIndex,
            int zoomMilli,
            int dprMilli,
            int col,
            int row,
            QImage image) {
            onAsyncTileReady(generation,
                docIdentity,
                pageIndex,
                zoomMilli,
                dprMilli,
                col,
                row,
                std::move(image));
        },
        Qt::QueuedConnection);
    m_tileRenderThread->start();
}

void PdfDocumentViewWidget::shutdownPdfTileRenderThread() {
    if (!m_tileRenderThread) {
        return;
    }
    if (m_tileRenderWorker) {
        QObject::disconnect(m_tileRenderWorker, nullptr, this, nullptr);
        QMetaObject::invokeMethod(m_tileRenderWorker, "clearPendingJobs", Qt::BlockingQueuedConnection);
    }
    m_tileRenderThread->quit();
    m_tileRenderThread->wait(10000);
    if (m_tileRenderWorker) {
        m_tileRenderWorker->moveToThread(QCoreApplication::instance()->thread());
        delete m_tileRenderWorker;
        m_tileRenderWorker = nullptr;
    }
}

void PdfDocumentViewWidget::purgeAsyncTileWorkerBlocking() {
    if (!m_tileRenderWorker) {
        return;
    }
    ++m_renderGeneration;
    QMetaObject::invokeMethod(m_tileRenderWorker,
        "purgeQueue",
        Qt::BlockingQueuedConnection,
        Q_ARG(quint64, m_renderGeneration));
}

bool PdfDocumentViewWidget::useAsyncWorkerTilePath() const {
    return m_asyncTileRenderingEnabled && m_tileRenderWorker && useTilePaintPath() && m_backend
        && m_backend->capabilities().asyncTileRendering;
}

void PdfDocumentViewWidget::onAsyncTileReady(quint64 generation,
    const QString& docIdentity,
    int pageIndex,
    int zoomMilli,
    int dprMilli,
    int col,
    int row,
    QImage image) {
    if (!m_tileRuntime || !useTilePaintPath()) {
        return;
    }
    if (generation != m_renderGeneration) {
        return;
    }
    const TileKey key{docIdentity, pageIndex, zoomMilli, dprMilli, col, row};
    if (key.docIdentity != m_documentIdentity || key.pageIndex != m_currentPageIndex) {
        return;
    }
    const qreal dpr = qMax(static_cast<qreal>(1.0), viewport()->devicePixelRatioF());
    if (key.zoomMilli != qRound(m_zoom * 1000.0) || key.dprMilli != qRound(dpr * 1000.0)) {
        return;
    }
    if (image.isNull()) {
        return;
    }
    m_tileRuntime->cache.insert(key, std::move(image));

    const QSize docPx = documentPixelSize();
    QSize devPx;
    devicePageBitmapSize(&devPx);
    const int iw = devPx.width();
    const int ih = devPx.height();
    const QRect tileDev(col * kTileDevicePixels,
        row * kTileDevicePixels,
        qMin(kTileDevicePixels, iw - col * kTileDevicePixels),
        qMin(kTileDevicePixels, ih - row * kTileDevicePixels));
    const QRect dirty = viewportRectForTileDeviceRect(docPx, dpr, tileDev);
    if (!dirty.isEmpty()) {
        viewport()->update(dirty);
    }
}

#endif // PDFDOCUMENTVIEW_ASYNC_TILE_WORKER

QString PdfDocumentViewWidget::backendId() const {
    return m_backend ? m_backend->id() : QString();
}

PdfWidgetCapabilities PdfDocumentViewWidget::capabilities() const {
    if (!m_backend) {
        return {};
    }
    PdfWidgetCapabilities caps = m_backend->capabilities();
    if (!m_documentModel || !m_documentModel->supportsOpenBuffer()) {
        caps.openFromBuffer = false;
    }
    if (!m_tileRenderingEnabled) {
        caps.tileRendering = false;
    }
    if (!m_asyncTileRenderingEnabled || !m_tileRenderingEnabled) {
        caps.asyncTileRendering = false;
    }
    return caps;
}

PdfDocumentModel* PdfDocumentViewWidget::documentModel() const {
    return m_documentModel.get();
}

bool PdfDocumentViewWidget::tileRenderingEnabled() const {
    return m_tileRenderingEnabled;
}

void PdfDocumentViewWidget::setTileRenderingEnabled(bool enabled) {
    if (m_tileRenderingEnabled == enabled) {
        return;
    }
    m_tileRenderingEnabled = enabled;
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    if (!enabled && m_asyncTileRenderingEnabled) {
        m_asyncTileRenderingEnabled = false;
        purgeAsyncTileWorkerBlocking();
    }
#endif
    clearAllRasterAndTileState();
    viewport()->update();
    emit viewStateChanged();
}

bool PdfDocumentViewWidget::asyncTileRenderingEnabled() const {
    return m_asyncTileRenderingEnabled;
}

void PdfDocumentViewWidget::setAsyncTileRenderingEnabled(bool enabled) {
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    if (m_asyncTileRenderingEnabled == enabled) {
        return;
    }
    if (enabled && (!m_backend || !m_backend->capabilities().asyncTileRendering)) {
        return;
    }
    m_asyncTileRenderingEnabled = enabled;
    if (!enabled) {
        purgeAsyncTileWorkerBlocking();
    }
    clearAllRasterAndTileState();
    viewport()->update();
    emit viewStateChanged();
#else
    (void)enabled;
#endif
}

bool PdfDocumentViewWidget::useTilePaintPath() const {
    return m_tileRenderingEnabled && m_backend && m_backend->capabilities().tileRendering;
}

bool PdfDocumentViewWidget::asyncTileRenderingSupported() const {
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    return m_tileRenderWorker && m_backend && m_backend->capabilities().asyncTileRendering
        && useTilePaintPath();
#else
    return false;
#endif
}

void PdfDocumentViewWidget::clearAllRasterAndTileState() {
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    if (m_asyncTileRenderingEnabled && m_tileRenderWorker) {
        purgeAsyncTileWorkerBlocking();
    }
#endif
    m_rasterPage = -1;
    m_pageRaster = QImage();
    if (m_tileRuntime) {
        m_tileRuntime->cache.clear();
        m_tileRuntime->pending.clear();
        m_tileRuntime->pendingKeys.clear();
        m_tileRuntime->drainScheduled = false;
    }
}

void PdfDocumentViewWidget::devicePageBitmapSize(QSize* outDevicePx) const {
    const QSize docPx = documentPixelSize();
    const qreal dpr = qMax(static_cast<qreal>(1.0), viewport()->devicePixelRatioF());
    const int iw = qMax(1, static_cast<int>(std::ceil(static_cast<double>(docPx.width()) * dpr)));
    const int ih = qMax(1, static_cast<int>(std::ceil(static_cast<double>(docPx.height()) * dpr)));
    *outDevicePx = QSize(iw, ih);
}

QRect PdfDocumentViewWidget::viewportRectForTileDeviceRect(const QSize& docPx,
    qreal dpr,
    const QRect& tileDev) const {
    if (docPx.isEmpty() || tileDev.isEmpty()) {
        return {};
    }
    const QRectF destF(static_cast<qreal>(tileDev.left()) / dpr,
        static_cast<qreal>(tileDev.top()) / dpr,
        static_cast<qreal>(tileDev.width()) / dpr,
        static_cast<qreal>(tileDev.height()) / dpr);
    const QRect docTile = destF.toAlignedRect();
    const int hx = horizontalScrollBar()->value();
    const int hy = verticalScrollBar()->value();
    return docTile.translated(-hx, -hy).intersected(viewport()->rect());
}

void PdfDocumentViewWidget::enqueueMissingVisibleTiles(const QSize& docPx,
    const QSize& devPx,
    qreal dpr,
    const QRect& visDevTiles) {
    if (!m_tileRuntime || !useTilePaintPath() || docPx.isEmpty()) {
        return;
    }
    const int iw = devPx.width();
    const int ih = devPx.height();
    const int cols = (iw + kTileDevicePixels - 1) / kTileDevicePixels;
    const int rows = (ih + kTileDevicePixels - 1) / kTileDevicePixels;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const QRect tileDev(col * kTileDevicePixels,
                row * kTileDevicePixels,
                qMin(kTileDevicePixels, iw - col * kTileDevicePixels),
                qMin(kTileDevicePixels, ih - row * kTileDevicePixels));
            if (!tileDev.intersects(visDevTiles)) {
                continue;
            }
            TileKey key{m_documentIdentity,
                m_currentPageIndex,
                qRound(m_zoom * 1000.0),
                qRound(dpr * 1000.0),
                col,
                row};
            if (m_tileRuntime->cache.lookup(key)) {
                continue;
            }
            if (m_tileRuntime->pendingKeys.contains(key)) {
                continue;
            }
            m_tileRuntime->pending.push_back(key);
            m_tileRuntime->pendingKeys.insert(key);
        }
    }
    scheduleTileDrain();
}

void PdfDocumentViewWidget::scheduleTileDrain() {
    if (!m_tileRuntime || !useTilePaintPath()) {
        return;
    }
    if (m_tileRuntime->drainScheduled) {
        return;
    }
    if (m_tileRuntime->pending.empty()) {
        return;
    }
    m_tileRuntime->drainScheduled = true;
    QTimer::singleShot(0, this, [this] {
        if (!m_tileRuntime) {
            return;
        }
        m_tileRuntime->drainScheduled = false;
        processOnePendingTile();
    });
}

void PdfDocumentViewWidget::processOnePendingTile() {
    if (!m_tileRuntime || !useTilePaintPath() || !m_backend) {
        return;
    }
    if (m_tileRuntime->pending.empty()) {
        return;
    }

    const TileKey key = m_tileRuntime->pending.front();
    m_tileRuntime->pending.erase(m_tileRuntime->pending.begin());
    m_tileRuntime->pendingKeys.remove(key);

    if (key.docIdentity != m_documentIdentity || key.pageIndex != m_currentPageIndex) {
        scheduleTileDrain();
        return;
    }

    const qreal dpr = qMax(static_cast<qreal>(1.0), viewport()->devicePixelRatioF());
    if (key.zoomMilli != qRound(m_zoom * 1000.0) || key.dprMilli != qRound(dpr * 1000.0)) {
        scheduleTileDrain();
        return;
    }

    if (m_tileRuntime->cache.lookup(key)) {
        scheduleTileDrain();
        return;
    }

#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    if (useAsyncWorkerTilePath()) {
        QSize devPx;
        devicePageBitmapSize(&devPx);
        const int iw = devPx.width();
        const int ih = devPx.height();
        const QRect tileDev(key.col * kTileDevicePixels,
            key.row * kTileDevicePixels,
            qMin(kTileDevicePixels, iw - key.col * kTileDevicePixels),
            qMin(kTileDevicePixels, ih - key.row * kTileDevicePixels));
        if (!tileDev.isValid() || tileDev.isEmpty()) {
            scheduleTileDrain();
            return;
        }

#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
        auto* model = dynamic_cast<PdfiumDocumentModel*>(m_documentModel.get());
#elif defined(PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER) && PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER
        auto* model = dynamic_cast<PopplerQtDocumentModel*>(m_documentModel.get());
#else
        PdfDocumentModel* model = nullptr;
#endif
        if (!model) {
            scheduleTileDrain();
            return;
        }

        TileJob job;
        job.generation = m_renderGeneration;
        job.key = key;
        job.pdfBytes = model->savedCopy();
        if (job.pdfBytes.isEmpty()) {
            job.filePath = model->loadedFilePath();
        }
        if (job.pdfBytes.isEmpty() && job.filePath.isEmpty()) {
            scheduleTileDrain();
            return;
        }
        job.devicePixelSize = devPx;
        job.tileRect = tileDev;
        job.paperColor = m_paperColor;
        job.renderBackend = m_backend.get();

        QMetaObject::invokeMethod(m_tileRenderWorker,
            "enqueue",
            Qt::QueuedConnection,
            Q_ARG(pdf_document_view::TileJob, job));

        scheduleTileDrain();
        return;
    }
#endif

    const QSize docPx = documentPixelSize();
    QSize devPx;
    devicePageBitmapSize(&devPx);
    const int iw = devPx.width();
    const int ih = devPx.height();
    const QRect tileDev(key.col * kTileDevicePixels,
        key.row * kTileDevicePixels,
        qMin(kTileDevicePixels, iw - key.col * kTileDevicePixels),
        qMin(kTileDevicePixels, ih - key.row * kTileDevicePixels));
    if (!tileDev.isValid() || tileDev.isEmpty()) {
        scheduleTileDrain();
        return;
    }

    PageRenderRequest req;
    req.pageIndex = m_currentPageIndex;
    req.devicePixelRatio = dpr;
    req.devicePixelSize = devPx;
    req.paperColor = m_paperColor;

    QImage img;
    if (!m_backend->renderPageTile(req, tileDev, &img) || img.isNull()) {
        scheduleTileDrain();
        return;
    }

    m_tileRuntime->cache.insert(key, std::move(img));

    const QRect dirty = viewportRectForTileDeviceRect(docPx, dpr, tileDev);
    if (!dirty.isEmpty()) {
        viewport()->update(dirty);
    }

    scheduleTileDrain();
}

void PdfDocumentViewWidget::paintViewportTiled(QPainter& painter) {
    const qreal dpr = qMax(static_cast<qreal>(1.0), viewport()->devicePixelRatioF());
    const QSize docPx = documentPixelSize();
    QSize devPx;
    devicePageBitmapSize(&devPx);
    const int iw = devPx.width();
    const int ih = devPx.height();

    const int hx = horizontalScrollBar()->value();
    const int hy = verticalScrollBar()->value();
    const int vw = viewport()->width();
    const int vh = viewport()->height();

    const int dlx = static_cast<int>(std::floor(static_cast<qreal>(hx) * dpr));
    const int dty = static_cast<int>(std::floor(static_cast<qreal>(hy) * dpr));
    const int drx = static_cast<int>(std::ceil(static_cast<qreal>(hx + vw) * dpr));
    const int dby = static_cast<int>(std::ceil(static_cast<qreal>(hy + vh) * dpr));
    const QRect visDevTiles = QRect(dlx, dty, qMax(0, drx - dlx), qMax(0, dby - dty)).intersected(QRect(0, 0, iw, ih));

    painter.save();
    painter.translate(-hx, -hy);
    painter.fillRect(QRectF(0, 0, static_cast<qreal>(docPx.width()), static_cast<qreal>(docPx.height())), m_paperColor);

    const int cols = (iw + kTileDevicePixels - 1) / kTileDevicePixels;
    const int rows = (ih + kTileDevicePixels - 1) / kTileDevicePixels;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const QRect tileDev(col * kTileDevicePixels,
                row * kTileDevicePixels,
                qMin(kTileDevicePixels, iw - col * kTileDevicePixels),
                qMin(kTileDevicePixels, ih - row * kTileDevicePixels));
            if (!tileDev.intersects(visDevTiles)) {
                continue;
            }
            TileKey key{m_documentIdentity,
                m_currentPageIndex,
                qRound(m_zoom * 1000.0),
                qRound(dpr * 1000.0),
                col,
                row};
            const QRectF destF(static_cast<qreal>(tileDev.left()) / dpr,
                static_cast<qreal>(tileDev.top()) / dpr,
                static_cast<qreal>(tileDev.width()) / dpr,
                static_cast<qreal>(tileDev.height()) / dpr);
            if (const QImage* cached = m_tileRuntime->cache.lookup(key)) {
                painter.drawImage(destF, *cached);
            } else {
                painter.fillRect(destF, kTilePlaceholderColor);
            }
        }
    }

    const QSizeF pagePts = m_backend->pageSize(m_currentPageIndex);
    // Z-order in document space (bottom → top): (1) PDF raster, (2) host `PageHighlight` overlays,
    // (3) find highlights so inactive/active find ink sits above host decorations.
    paintPageHostHighlights(painter, pagePts, docPx);
    paintFindHighlights(painter, pagePts, docPx);
    painter.restore();

    enqueueMissingVisibleTiles(docPx, devPx, dpr, visDevTiles);
}

void PdfDocumentViewWidget::paintViewportFullRaster(QPainter& painter) {
    ensurePageRaster();

    const int hx = horizontalScrollBar()->value();
    const int hy = verticalScrollBar()->value();
    const QSize docPx = documentPixelSize();

    painter.save();
    painter.translate(-hx, -hy);

    const QRectF pageRect(0, 0, docPx.width(), docPx.height());
    painter.fillRect(pageRect, m_paperColor);

    if (!m_pageRaster.isNull()) {
        painter.drawImage(pageRect, m_pageRaster);
    }

    const QSizeF pagePts = m_backend->pageSize(m_currentPageIndex);
    // Z-order: see `paintViewportTiled` (raster → host highlights → find).
    paintPageHostHighlights(painter, pagePts, docPx);
    paintFindHighlights(painter, pagePts, docPx);

    painter.restore();
}

void PdfDocumentViewWidget::setDocumentPath(const QString& path) {
    clearFind();
    m_userAdjustedZoom = false;
    m_pendingAutoFitWidthOnOpen = false;
    clearAllRasterAndTileState();

    if (!m_documentModel) {
        m_documentPath.clear();
        m_documentIdentity.clear();
        m_lastDocumentError = QStringLiteral("Document model is not available.");
        emit documentError(m_lastDocumentError);
        emit documentOpened(false);
        clampCurrentPage();
        updateScrollBars();
        viewport()->update();
        emit viewStateChanged();
        return;
    }

    QString err;
    bool ok = true;

    if (path.trimmed().isEmpty()) {
        m_documentModel->close();
        m_documentPath.clear();
        m_documentIdentity.clear();
        m_lastDocumentError.clear();
    } else {
        ok = m_documentModel->openFile(path, &err);
        if (ok) {
            const QFileInfo fi(path);
            m_documentPath = fi.absoluteFilePath();
            m_documentIdentity = m_documentModel->identityKey();
            m_lastDocumentError.clear();
        } else {
            m_documentPath.clear();
            m_documentIdentity.clear();
            m_lastDocumentError = err.isEmpty() ? m_documentModel->lastError() : err;
            emit documentError(m_lastDocumentError);
            emit documentOpened(false);
        }
    }

    clampCurrentPage();

    if (!path.trimmed().isEmpty() && !ok) {
        m_pendingAutoFitWidthOnOpen = false;
        if (m_backend) {
            setToolTip(QStringLiteral("%1 — %2").arg(m_backend->displayName(), m_backend->id()));
        }
        updateScrollBars();
        viewport()->update();
        emit viewStateChanged();
        return;
    }

    if (isDocumentOpen() && pageCount() > 0) {
        m_pendingAutoFitWidthOnOpen = true;
    }

    if (m_backend) {
        if (!isDocumentOpen()) {
            setToolTip(QStringLiteral("%1 — %2").arg(m_backend->displayName(), m_backend->id()));
        } else {
            const QString src = documentSourceLabel();
            setToolTip(QStringLiteral("%1 — %2\n%3")
                           .arg(m_backend->displayName(), m_backend->id(), src));
        }
    } else {
        setToolTip(documentSourceLabel());
    }

    updateScrollBars();
    viewport()->update();
    tryApplyAutoFitWidthOnOpen(true);
    if (path.trimmed().isEmpty()) {
        emit documentOpened(false);
    } else if (ok) {
        emit documentOpened(true);
    }
    emit viewStateChanged();
}

bool PdfDocumentViewWidget::openDocumentBuffer(const QByteArray& data, QString* errorMessage) {
    clearFind();
    m_userAdjustedZoom = false;
    m_pendingAutoFitWidthOnOpen = false;
    clearAllRasterAndTileState();
    m_documentPath.clear();

    if (!m_documentModel) {
        m_documentIdentity.clear();
        m_lastDocumentError = QStringLiteral("Document model is not available.");
        emit documentError(m_lastDocumentError);
        emit documentOpened(false);
        clampCurrentPage();
        updateScrollBars();
        viewport()->update();
        emit viewStateChanged();
        return false;
    }

    QString errLocal;
    QString* errOut = errorMessage ? errorMessage : &errLocal;
    const bool ok = m_documentModel->openBuffer(data, errOut);
    if (!ok) {
        m_documentIdentity.clear();
        m_lastDocumentError = errOut->isEmpty() ? m_documentModel->lastError() : *errOut;
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = m_lastDocumentError;
        }
        emit documentError(m_lastDocumentError);
        emit documentOpened(false);
        clampCurrentPage();
        if (m_backend) {
            setToolTip(QStringLiteral("%1 — %2").arg(m_backend->displayName(), m_backend->id()));
        }
        updateScrollBars();
        viewport()->update();
        emit viewStateChanged();
        return false;
    }

    m_documentIdentity = m_documentModel->identityKey();
    m_lastDocumentError.clear();
    clampCurrentPage();

    if (pageCount() > 0) {
        m_pendingAutoFitWidthOnOpen = true;
    }

    if (m_backend) {
        const QString src = documentSourceLabel();
        setToolTip(QStringLiteral("%1 — %2\n%3").arg(m_backend->displayName(), m_backend->id(), src));
    }

    updateScrollBars();
    viewport()->update();
    tryApplyAutoFitWidthOnOpen(true);
    emit documentOpened(true);
    emit viewStateChanged();
    return true;
}

QString PdfDocumentViewWidget::documentPath() const {
    return m_documentPath;
}

QString PdfDocumentViewWidget::documentSourceLabel() const {
    return m_documentModel ? m_documentModel->sourceLabel() : QString();
}

bool PdfDocumentViewWidget::isDocumentOpen() const {
    return m_documentModel && m_documentModel->isOpen();
}

QString PdfDocumentViewWidget::documentError() const {
    return m_lastDocumentError;
}

int PdfDocumentViewWidget::pageCount() const {
    return m_documentModel ? m_documentModel->pageCount() : 0;
}

int PdfDocumentViewWidget::currentPageIndex() const {
    return m_currentPageIndex;
}

void PdfDocumentViewWidget::clampCurrentPage() {
    const int n = pageCount();
    if (n <= 0) {
        m_currentPageIndex = 0;
        return;
    }
    if (m_currentPageIndex < 0) {
        m_currentPageIndex = 0;
    }
    if (m_currentPageIndex >= n) {
        m_currentPageIndex = n - 1;
    }
}

void PdfDocumentViewWidget::setCurrentPageIndex(int index) {
    m_currentPageIndex = index;
    clampCurrentPage();
    clearAllRasterAndTileState();
    updateScrollBars();
    viewport()->update();
    emit viewStateChanged();
}

void PdfDocumentViewWidget::nextPage() {
    if (pageCount() <= 0) {
        return;
    }
    setCurrentPageIndex(m_currentPageIndex + 1);
}

void PdfDocumentViewWidget::previousPage() {
    if (pageCount() <= 0) {
        return;
    }
    setCurrentPageIndex(m_currentPageIndex - 1);
}

double PdfDocumentViewWidget::zoom() const {
    return m_zoom;
}

void PdfDocumentViewWidget::applyZoomChange(double zoomValue, bool suppressViewStateChanged) {
    const double z = qBound(0.1, zoomValue, 8.0);
    if (qFuzzyCompare(z, m_zoom)) {
        return;
    }
    m_zoom = z;
    clearAllRasterAndTileState();
    updateScrollBars();
    viewport()->update();
    if (!suppressViewStateChanged) {
        emit viewStateChanged();
    }
}

void PdfDocumentViewWidget::setZoom(double zoomValue) {
    m_userAdjustedZoom = true;
    m_pendingAutoFitWidthOnOpen = false;
    applyZoomChange(zoomValue, false);
}

void PdfDocumentViewWidget::resetZoom() {
    m_userAdjustedZoom = true;
    m_pendingAutoFitWidthOnOpen = false;
    applyZoomChange(1.0, false);
}

std::optional<double> PdfDocumentViewWidget::computeFitWidthZoom() const {
    if (!m_backend || pageCount() <= 0) {
        return std::nullopt;
    }
    const QSizeF pts = m_backend->pageSize(m_currentPageIndex);
    if (pts.width() <= 0.0) {
        return std::nullopt;
    }
    const qreal dpiX = viewport()->logicalDpiX();
    const qreal targetW = static_cast<qreal>(viewport()->width());
    if (targetW <= 0.0) {
        return std::nullopt;
    }
    return (static_cast<double>(targetW) * 72.0) / (static_cast<double>(pts.width()) * static_cast<double>(dpiX));
}

void PdfDocumentViewWidget::tryApplyAutoFitWidthOnOpen(bool suppressViewStateChanged) {
    if (!m_pendingAutoFitWidthOnOpen || m_userAdjustedZoom) {
        return;
    }
    // Avoid locking zoom to a tiny pre-show viewport; wait until the top-level window is visible.
    QWidget* top = window();
    if (!top || !top->isVisible()) {
        return;
    }
    const std::optional<double> z = computeFitWidthZoom();
    if (!z.has_value()) {
        return;
    }
    applyZoomChange(*z, suppressViewStateChanged);
    m_pendingAutoFitWidthOnOpen = false;
}

void PdfDocumentViewWidget::fitWidth() {
    m_userAdjustedZoom = true;
    m_pendingAutoFitWidthOnOpen = false;
    const std::optional<double> z = computeFitWidthZoom();
    if (z.has_value()) {
        applyZoomChange(*z, false);
    }
}

QColor PdfDocumentViewWidget::paperColor() const {
    return m_paperColor;
}

void PdfDocumentViewWidget::setPaperColor(const QColor& color) {
    if (color == m_paperColor) {
        return;
    }
    m_paperColor = color;
    clearAllRasterAndTileState();
    viewport()->update();
    emit viewStateChanged();
}

void PdfDocumentViewWidget::refreshFindMatches(const QString& trimmedNeedle) {
    m_findMatches.clear();
    m_activeFindIndex = -1;
    if (!m_backend || !isDocumentOpen() || trimmedNeedle.isEmpty()) {
        return;
    }
    (void)m_backend->findTextMatches(trimmedNeedle, &m_findMatches);
}

void PdfDocumentViewWidget::findText(const QString& needle) {
    const QString trimmed = needle.trimmed();
    refreshFindMatches(trimmed);
    if (!m_backend || !isDocumentOpen() || trimmed.isEmpty()) {
        emit findResultsChanged(0, -1);
        viewport()->update();
        return;
    }

    if (!m_findMatches.isEmpty()) {
        scrollToFindMatch(0);
        return;
    }

    emit findResultsChanged(0, -1);
    viewport()->update();
    emit viewStateChanged();
}

void PdfDocumentViewWidget::findNext() {
    if (m_findMatches.isEmpty()) {
        return;
    }
    const int n = m_findMatches.size();
    scrollToFindMatch((m_activeFindIndex + 1 + n) % n);
}

void PdfDocumentViewWidget::findPrevious() {
    if (m_findMatches.isEmpty()) {
        return;
    }
    const int n = m_findMatches.size();
    scrollToFindMatch((m_activeFindIndex - 1 + n) % n);
}

void PdfDocumentViewWidget::clearFind() {
    const bool had = !m_findMatches.isEmpty() || m_activeFindIndex >= 0;
    m_findMatches.clear();
    m_activeFindIndex = -1;
    if (had) {
        emit findResultsChanged(0, -1);
    }
    viewport()->update();
}

int PdfDocumentViewWidget::findMatchCount() const {
    return m_findMatches.size();
}

int PdfDocumentViewWidget::findCurrentIndex() const {
    return m_activeFindIndex;
}

QVector<TextMatch> PdfDocumentViewWidget::findMatches() const {
    return m_findMatches;
}

TextMatch PdfDocumentViewWidget::currentFindMatch() const {
    if (m_activeFindIndex < 0 || m_activeFindIndex >= m_findMatches.size()) {
        return TextMatch{};
    }
    return m_findMatches.at(m_activeFindIndex);
}

void PdfDocumentViewWidget::scrollToFindMatch(int index) {
    if (index < 0 || index >= m_findMatches.size() || !m_backend) {
        return;
    }
    m_activeFindIndex = index;
    const TextMatch& m = m_findMatches.at(m_activeFindIndex);
    if (m.pageIndex != m_currentPageIndex) {
        m_currentPageIndex = m.pageIndex;
        clampCurrentPage();
        clearAllRasterAndTileState();
    }
    updateScrollBars();
    ensureFindMatchVisible(m);
    emit findResultsChanged(m_findMatches.size(), m_activeFindIndex);
    viewport()->update();
    emit viewStateChanged();
}

void PdfDocumentViewWidget::scrollToFindMatch(const TextMatch& match) {
    for (int i = 0; i < m_findMatches.size(); ++i) {
        const TextMatch& e = m_findMatches.at(i);
        if (e.pageIndex == match.pageIndex && e.pageRect == match.pageRect) {
            scrollToFindMatch(i);
            return;
        }
    }
}

void PdfDocumentViewWidget::revealPageRect(int pageIndex, const QRectF& pageRect) {
    if (!m_backend || !isDocumentOpen() || pageCount() <= 0) {
        return;
    }
    if (pageIndex < 0 || pageIndex >= pageCount() || !pageRect.isValid()) {
        return;
    }
    if (pageIndex != m_currentPageIndex) {
        m_currentPageIndex = pageIndex;
        clampCurrentPage();
        clearAllRasterAndTileState();
    }
    updateScrollBars();
    ensurePageRectVisibleInLayout(pageRect);
    viewport()->update();
    emit viewStateChanged();
}

void PdfDocumentViewWidget::navigateToHostSearchHit(int pageIndex, const QRectF& pageRect, bool showHighlight) {
    revealPageRect(pageIndex, pageRect);
    if (!showHighlight) {
        return;
    }
    PageHighlight h;
    h.pageIndex = pageIndex;
    h.pageRect = pageRect;
    h.fill = m_findHighlightFillColor;
    h.border = m_findActiveMatchOutlineColor;
    setPageHighlights({h});
}

void PdfDocumentViewWidget::openDocumentAndRevealSearchHit(const QString& filePath,
    const QString& searchTerm,
    int pageIndex,
    const QRectF& pageRect) {
    setDocumentPath(filePath);
    if (!isDocumentOpen() || pageCount() <= 0) {
        return;
    }
    if (pageIndex < 0 || pageIndex >= pageCount() || !pageRect.isValid()) {
        return;
    }

    setCurrentPageIndex(pageIndex);

    PageHighlight h;
    h.pageIndex = pageIndex;
    h.pageRect = pageRect;
    h.fill = m_findHighlightFillColor;
    h.border = m_findActiveMatchOutlineColor;
    setPageHighlights({h});

    revealPageRect(pageIndex, pageRect);

    const QString term = searchTerm.trimmed();
    if (term.isEmpty()) {
        return;
    }

    refreshFindMatches(term);
    for (int i = 0; i < m_findMatches.size(); ++i) {
        const TextMatch& m = m_findMatches.at(i);
        if (m.pageIndex == pageIndex && m.pageRect == pageRect) {
            m_activeFindIndex = i;
            emit findResultsChanged(m_findMatches.size(), m_activeFindIndex);
            viewport()->update();
            emit viewStateChanged();
            return;
        }
    }
    m_activeFindIndex = -1;
    emit findResultsChanged(m_findMatches.size(), -1);
    viewport()->update();
    emit viewStateChanged();
}

QColor PdfDocumentViewWidget::findHighlightFillColor() const {
    return m_findHighlightFillColor;
}

void PdfDocumentViewWidget::setFindHighlightFillColor(const QColor& color) {
    if (color == m_findHighlightFillColor) {
        return;
    }
    m_findHighlightFillColor = color;
    if (!m_findMatches.isEmpty()) {
        viewport()->update();
    }
}

QColor PdfDocumentViewWidget::findActiveMatchOutlineColor() const {
    return m_findActiveMatchOutlineColor;
}

void PdfDocumentViewWidget::setFindActiveMatchOutlineColor(const QColor& color) {
    if (color == m_findActiveMatchOutlineColor) {
        return;
    }
    m_findActiveMatchOutlineColor = color;
    if (!m_findMatches.isEmpty()) {
        viewport()->update();
    }
}

qreal PdfDocumentViewWidget::findActiveMatchOutlineWidth() const {
    return m_findActiveMatchOutlineWidth;
}

void PdfDocumentViewWidget::setFindActiveMatchOutlineWidth(qreal width) {
    if (qFuzzyCompare(width, m_findActiveMatchOutlineWidth)) {
        return;
    }
    m_findActiveMatchOutlineWidth = width;
    if (!m_findMatches.isEmpty()) {
        viewport()->update();
    }
}

void PdfDocumentViewWidget::setPageHighlights(const QVector<PageHighlight>& highlights) {
    m_pageHighlights = highlights;
    viewport()->update();
}

void PdfDocumentViewWidget::clearPageHighlights() {
    if (m_pageHighlights.isEmpty()) {
        return;
    }
    m_pageHighlights.clear();
    viewport()->update();
}

QVector<PageHighlight> PdfDocumentViewWidget::pageHighlights() const {
    return m_pageHighlights;
}

QSize PdfDocumentViewWidget::documentPixelSize() const {
    if (!m_backend || pageCount() <= 0) {
        return {};
    }
    const QSizeF pts = m_backend->pageSize(m_currentPageIndex);
    if (pts.isEmpty()) {
        return {};
    }
    const qreal lx = viewport()->logicalDpiX();
    const qreal ly = viewport()->logicalDpiY();
    const int w = qMax(1, static_cast<int>(std::ceil(static_cast<double>(pts.width()) * m_zoom * static_cast<double>(lx) / 72.0)));
    const int h = qMax(1, static_cast<int>(std::ceil(static_cast<double>(pts.height()) * m_zoom * static_cast<double>(ly) / 72.0)));
    return QSize(w, h);
}

void PdfDocumentViewWidget::ensurePageRaster() {
    if (!m_backend || pageCount() <= 0) {
        m_pageRaster = QImage();
        return;
    }

    const QSize docPx = documentPixelSize();
    const qreal dpr = qMax(static_cast<qreal>(1.0), viewport()->devicePixelRatioF());

    if (!m_pageRaster.isNull() && m_rasterIdentity == m_documentIdentity && m_rasterPage == m_currentPageIndex
        && qFuzzyCompare(m_rasterZoom, m_zoom) && m_rasterDocPx == docPx && qFuzzyCompare(m_rasterDpr, dpr)) {
        return;
    }

    const int iw = qMax(1, static_cast<int>(std::ceil(static_cast<double>(docPx.width()) * dpr)));
    const int ih = qMax(1, static_cast<int>(std::ceil(static_cast<double>(docPx.height()) * dpr)));

    m_pageRaster = QImage(iw, ih, QImage::Format_ARGB32_Premultiplied);
    m_pageRaster.fill(m_paperColor);

    QPainter ip(&m_pageRaster);
    ip.setRenderHint(QPainter::Antialiasing, true);
    ip.setRenderHint(QPainter::TextAntialiasing, true);
    const QRectF target(0, 0, static_cast<qreal>(iw), static_cast<qreal>(ih));
    PageRenderRequest req;
    req.pageIndex = m_currentPageIndex;
    req.devicePixelRatio = dpr;
    req.devicePixelSize = QSize(iw, ih);
    req.paperColor = m_paperColor;
    m_backend->renderPage(req, ip, target);

    m_rasterIdentity = m_documentIdentity;
    m_rasterPage = m_currentPageIndex;
    m_rasterZoom = m_zoom;
    m_rasterDocPx = docPx;
    m_rasterDpr = dpr;
}

void PdfDocumentViewWidget::updateScrollBars() {
    const QSize doc = documentPixelSize();
    const int vw = viewport()->width();
    const int vh = viewport()->height();

    horizontalScrollBar()->setPageStep(qMax(1, vw));
    verticalScrollBar()->setPageStep(qMax(1, vh));

    const int maxW = qMax(0, doc.width() - vw);
    const int maxH = qMax(0, doc.height() - vh);

    horizontalScrollBar()->setRange(0, maxW);
    verticalScrollBar()->setRange(0, maxH);

    horizontalScrollBar()->setSingleStep(qMax(1, vw / 12));
    verticalScrollBar()->setSingleStep(qMax(1, vh / 12));

    horizontalScrollBar()->setValue(qBound(horizontalScrollBar()->minimum(), horizontalScrollBar()->value(), horizontalScrollBar()->maximum()));
    verticalScrollBar()->setValue(qBound(verticalScrollBar()->minimum(), verticalScrollBar()->value(), verticalScrollBar()->maximum()));
}

void PdfDocumentViewWidget::paintPageHostHighlights(QPainter& painter, const QSizeF& pagePts, const QSize& docPx) const {
    if (pagePts.width() <= 0.0 || pagePts.height() <= 0.0 || docPx.isEmpty()) {
        return;
    }

    const double sx = static_cast<double>(docPx.width()) / pagePts.width();
    const double sy = static_cast<double>(docPx.height()) / pagePts.height();

    for (const PageHighlight& h : m_pageHighlights) {
        if (h.pageIndex != m_currentPageIndex || !h.pageRect.isValid()) {
            continue;
        }
        const QRectF dr(h.pageRect.x() * sx,
            h.pageRect.y() * sy,
            h.pageRect.width() * sx,
            h.pageRect.height() * sy);

        if (h.fill.isValid() && h.fill.alpha() != 0) {
            painter.fillRect(dr, h.fill);
        }
        if (h.border.isValid() && h.border.alpha() != 0) {
            painter.setPen(QPen(h.border, 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(dr);
        }
    }
}

void PdfDocumentViewWidget::paintFindHighlights(QPainter& painter, const QSizeF& pagePts, const QSize& docPx) const {
    if (pagePts.width() <= 0.0 || pagePts.height() <= 0.0 || docPx.isEmpty()) {
        return;
    }

    const double sx = static_cast<double>(docPx.width()) / pagePts.width();
    const double sy = static_cast<double>(docPx.height()) / pagePts.height();

    for (int i = 0; i < m_findMatches.size(); ++i) {
        const TextMatch& m = m_findMatches.at(i);
        if (m.pageIndex != m_currentPageIndex) {
            continue;
        }
        const QRectF hr(m.pageRect.x() * sx,
            m.pageRect.y() * sy,
            m.pageRect.width() * sx,
            m.pageRect.height() * sy);

        painter.fillRect(hr, m_findHighlightFillColor);
        if (i == m_activeFindIndex) {
            painter.setPen(QPen(m_findActiveMatchOutlineColor, m_findActiveMatchOutlineWidth));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(hr.adjusted(-1, -1, 1, 1));
        }
    }
}

void PdfDocumentViewWidget::paintViewport(QPainter& painter, const QRect& /*clip*/) {
    painter.fillRect(viewport()->rect(), kMarginColor);

    if (!m_backend || pageCount() <= 0) {
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawText(viewport()->rect(), Qt::AlignCenter, QStringLiteral("No document"));
        return;
    }

    if (useTilePaintPath()) {
        paintViewportTiled(painter);
    } else {
        paintViewportFullRaster(painter);
    }
}

void PdfDocumentViewWidget::scrollByPixels(int dx, int dy) {
    if (dx != 0) {
        QScrollBar* h = horizontalScrollBar();
        h->setValue(qBound(h->minimum(), h->value() + dx, h->maximum()));
    }
    if (dy != 0) {
        QScrollBar* v = verticalScrollBar();
        v->setValue(qBound(v->minimum(), v->value() + dy, v->maximum()));
    }
}

void PdfDocumentViewWidget::applyZoomAround(const QPointF& viewportPos, double newZoom) {
    m_userAdjustedZoom = true;
    m_pendingAutoFitWidthOnOpen = false;

    const QSize docBefore = documentPixelSize();
    const int hx = horizontalScrollBar()->value();
    const int hy = verticalScrollBar()->value();

    const double docX = static_cast<double>(hx) + viewportPos.x();
    const double docY = static_cast<double>(hy) + viewportPos.y();

    const double oldZoom = m_zoom;
    m_zoom = qBound(0.1, newZoom, 8.0);
    if (qFuzzyCompare(m_zoom, oldZoom)) {
        return;
    }

    clearAllRasterAndTileState();

    updateScrollBars();

    const QSize docAfter = documentPixelSize();
    if (docBefore.isEmpty() || docAfter.isEmpty()) {
        viewport()->update();
        emit viewStateChanged();
        return;
    }

    const double relX = docX / static_cast<double>(qMax(1, docBefore.width()));
    const double relY = docY / static_cast<double>(qMax(1, docBefore.height()));

    const int newHx = static_cast<int>(std::lround(relX * static_cast<double>(docAfter.width()) - static_cast<double>(viewportPos.x())));
    const int newHy = static_cast<int>(std::lround(relY * static_cast<double>(docAfter.height()) - static_cast<double>(viewportPos.y())));

    horizontalScrollBar()->setValue(qBound(horizontalScrollBar()->minimum(), newHx, horizontalScrollBar()->maximum()));
    verticalScrollBar()->setValue(qBound(verticalScrollBar()->minimum(), newHy, verticalScrollBar()->maximum()));

    viewport()->update();
    emit viewStateChanged();
}

void PdfDocumentViewWidget::scrollCurrentMatchVisible() {
    if (m_activeFindIndex < 0 || m_activeFindIndex >= m_findMatches.size()) {
        return;
    }
    ensureFindMatchVisible(m_findMatches.at(m_activeFindIndex));
}

void PdfDocumentViewWidget::ensurePageRectVisibleInLayout(const QRectF& pageRectInPageSpace) {
    if (!m_backend || pageCount() <= 0) {
        return;
    }

    const QSize docPx = documentPixelSize();
    const QSizeF pts = m_backend->pageSize(m_currentPageIndex);
    if (pts.width() <= 0.0 || pts.height() <= 0.0 || docPx.isEmpty()) {
        return;
    }

    const double sxx = static_cast<double>(docPx.width()) / pts.width();
    const double syy = static_cast<double>(docPx.height()) / pts.height();
    QRectF hr(pageRectInPageSpace.x() * sxx,
        pageRectInPageSpace.y() * syy,
        pageRectInPageSpace.width() * sxx,
        pageRectInPageSpace.height() * syy);
    hr = hr.adjusted(-8, -8, 8, 8);

    const int vw = viewport()->width();
    const int vh = viewport()->height();
    const int hx = horizontalScrollBar()->value();
    const int hy = verticalScrollBar()->value();
    QRect vis(hx, hy, vw, vh);

    if (vis.contains(hr.toRect())) {
        return;
    }

    const int cx = static_cast<int>(std::lround(hr.center().x() - static_cast<double>(vw) / 2.0));
    const int cy = static_cast<int>(std::lround(hr.center().y() - static_cast<double>(vh) / 2.0));

    horizontalScrollBar()->setValue(qBound(horizontalScrollBar()->minimum(), cx, horizontalScrollBar()->maximum()));
    verticalScrollBar()->setValue(qBound(verticalScrollBar()->minimum(), cy, verticalScrollBar()->maximum()));
}

void PdfDocumentViewWidget::ensureFindMatchVisible(const TextMatch& m) {
    if (!m_backend || m.pageIndex != m_currentPageIndex) {
        return;
    }
    ensurePageRectVisibleInLayout(m.pageRect);
}

bool PdfDocumentViewWidget::viewportEvent(QEvent* event) {
    if (event->type() == QEvent::Paint) {
        auto* pe = static_cast<QPaintEvent*>(event);
        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        paintViewport(painter, pe->rect());
        return true;
    }
    return QAbstractScrollArea::viewportEvent(event);
}

void PdfDocumentViewWidget::resizeEvent(QResizeEvent* event) {
    QAbstractScrollArea::resizeEvent(event);
    tryApplyAutoFitWidthOnOpen(false);
    updateScrollBars();
    viewport()->update();
    (void)event;
}

void PdfDocumentViewWidget::keyPressEvent(QKeyEvent* event) {
    const int n = pageCount();
    QScrollBar* v = verticalScrollBar();
    QScrollBar* h = horizontalScrollBar();

    if (n > 0) {
        if (event->key() == Qt::Key_Down) {
            if (v->maximum() > 0 && v->value() < v->maximum()) {
                v->setValue(qMin(v->maximum(), v->value() + v->singleStep()));
                event->accept();
                return;
            }
            if (m_currentPageIndex < n - 1) {
                setCurrentPageIndex(m_currentPageIndex + 1);
                v->setValue(0);
                event->accept();
                return;
            }
        }
        if (event->key() == Qt::Key_Up) {
            if (v->maximum() > 0 && v->value() > v->minimum()) {
                v->setValue(qMax(v->minimum(), v->value() - v->singleStep()));
                event->accept();
                return;
            }
            if (m_currentPageIndex > 0) {
                setCurrentPageIndex(m_currentPageIndex - 1);
                v->setValue(v->maximum());
                event->accept();
                return;
            }
        }
        if (event->key() == Qt::Key_PageDown) {
            if (v->maximum() > 0 && v->value() < v->maximum()) {
                v->setValue(qMin(v->maximum(), v->value() + v->pageStep()));
                event->accept();
                return;
            }
            if (m_currentPageIndex < n - 1) {
                setCurrentPageIndex(m_currentPageIndex + 1);
                v->setValue(0);
                event->accept();
                return;
            }
        }
        if (event->key() == Qt::Key_PageUp) {
            if (v->maximum() > 0 && v->value() > v->minimum()) {
                v->setValue(qMax(v->minimum(), v->value() - v->pageStep()));
                event->accept();
                return;
            }
            if (m_currentPageIndex > 0) {
                setCurrentPageIndex(m_currentPageIndex - 1);
                v->setValue(v->maximum());
                event->accept();
                return;
            }
        }
        if (event->key() == Qt::Key_Right) {
            if (h->maximum() > 0 && h->value() < h->maximum()) {
                h->setValue(qMin(h->maximum(), h->value() + h->singleStep()));
                event->accept();
                return;
            }
        }
        if (event->key() == Qt::Key_Left) {
            if (h->maximum() > 0 && h->value() > h->minimum()) {
                h->setValue(qMax(h->minimum(), h->value() - h->singleStep()));
                event->accept();
                return;
            }
        }
        if (event->key() == Qt::Key_Home) {
            if (v->maximum() > 0 || h->maximum() > 0) {
                h->setValue(h->minimum());
                v->setValue(v->minimum());
                event->accept();
                return;
            }
            setCurrentPageIndex(0);
            h->setValue(h->minimum());
            v->setValue(v->minimum());
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_End) {
            if (v->maximum() > 0 || h->maximum() > 0) {
                h->setValue(h->maximum());
                v->setValue(v->maximum());
                event->accept();
                return;
            }
            setCurrentPageIndex(n - 1);
            h->setValue(h->maximum());
            v->setValue(v->maximum());
            event->accept();
            return;
        }
    }

    QAbstractScrollArea::keyPressEvent(event);
}

bool PdfDocumentViewWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == viewport() && event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event);
        keyPressEvent(ke);
        if (ke->isAccepted()) {
            return true;
        }
    }
    if (watched == viewport() && event->type() == QEvent::Wheel) {
        auto* we = static_cast<QWheelEvent*>(event);
        if (we->modifiers() & Qt::ControlModifier) {
            const QPointF pos = we->position();
            const qreal steps = static_cast<qreal>(we->angleDelta().y()) / 120.0;
            const double factor = std::pow(1.1, static_cast<double>(steps));
            applyZoomAround(pos, m_zoom * factor);
        } else {
            const int stepX = horizontalScrollBar()->singleStep();
            const int stepY = verticalScrollBar()->singleStep();
            if (we->modifiers() & Qt::ShiftModifier) {
                const int dx = we->angleDelta().y() != 0 ? (we->angleDelta().y() / 120) * stepX : 0;
                scrollByPixels(-dx, 0);
            } else {
                const int dx = we->angleDelta().x() != 0 ? (we->angleDelta().x() / 120) * stepX : 0;
                const int dy = we->angleDelta().y() != 0 ? (we->angleDelta().y() / 120) * stepY : 0;
                scrollByPixels(-dx, -dy);
            }
        }
        return true;
    }
    return QAbstractScrollArea::eventFilter(watched, event);
}

} // namespace pdf_document_view

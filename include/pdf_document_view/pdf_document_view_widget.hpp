/**
 * @file pdf_document_view_widget.hpp
 * @brief Public embeddable PDF page widget (`PdfDocumentViewWidget`).
 *
 * @details Threading: call all public APIs from the **GUI thread** unless otherwise noted.
 * Coordinates for `TextMatch`, `PageHighlight`, `revealPageRect`, and related APIs are **page
 * space** (top-down PDF points); see `docs/widget-coordinate-contract.md`. Find vs host
 * highlights: built-in find uses `findText` / `findMatches` / `scrollToFindMatch`; host overlays use
 * `setPageHighlights` or `navigateToHostSearchHit`. `openDocumentAndRevealSearchHit` composes file
 * open, optional find alignment, scroll, and one host-style highlight. Public API freeze: ADR-0004.
 */

#pragma once

#include <pdf_document_view/page_highlight.hpp>
#include <pdf_document_view/pdf_capabilities.hpp>
#include <pdf_document_view/text_match.hpp>

#include <QAbstractScrollArea>
#include <QByteArray>
#include <QColor>
#include <QtGlobal>

#if defined(_WIN32)
#  if defined(PDFDOCUMENTVIEW_LIBRARY)
#    define PDFDOCUMENTVIEW_EXPORT Q_DECL_EXPORT
#  else
#    define PDFDOCUMENTVIEW_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define PDFDOCUMENTVIEW_EXPORT
#endif

#include <QEvent>
#include <QKeyEvent>
#include <QRect>
#include <QResizeEvent>
#include <QString>
#include <QVector>

#include <memory>
#include <optional>

namespace pdf_document_view {

class PdfDocumentModel;
class PdfRenderBackend;
struct PdfDocumentViewTileRuntime;
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
class PdfTileRenderWorker;
#endif

/**
 * @brief Embeddable scrollable PDF page view.
 *
 * @details Default construction wires the document model and render backend for the active CMake
 * backend (PDFium or Poppler). **Not** a full application viewer: hosts own menus, dialogs, and
 * persistence (`AGENTS.md`). Behavior for rendering threads and tiles is described in
 * `docs/rendering-threading.md`.
 */
class PDFDOCUMENTVIEW_EXPORT PdfDocumentViewWidget final : public QAbstractScrollArea {
    Q_OBJECT

public:
    /** @brief Constructs an empty widget with default model, backend, and styling. */
    explicit PdfDocumentViewWidget(QWidget* parent = nullptr);
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    ~PdfDocumentViewWidget() override;
#else
    ~PdfDocumentViewWidget() override = default;
#endif

    PdfDocumentViewWidget(const PdfDocumentViewWidget&) = delete;
    PdfDocumentViewWidget& operator=(const PdfDocumentViewWidget&) = delete;
    PdfDocumentViewWidget(PdfDocumentViewWidget&&) = delete;
    PdfDocumentViewWidget& operator=(PdfDocumentViewWidget&&) = delete;

    /** @brief Active render backend identifier string (diagnostics / tooltips). */
    [[nodiscard]] QString backendId() const;

    /**
     * @brief Feature flags for the active backend.
     * @return Capability struct with backend flags merged per `pdf_capabilities.hpp`.
     * @details `openFromBuffer` is intersected with `PdfDocumentModel::supportsOpenBuffer()`.
     */
    [[nodiscard]] PdfWidgetCapabilities capabilities() const;

    /**
     * @brief Non-owning pointer to the widget-owned document model.
     * @return Model pointer; never null for a constructed widget.
     * @details Same backend family as `createDefaultPdfDocumentModel()` for this build.
     */
    [[nodiscard]] PdfDocumentModel* documentModel() const;

    /**
     * @brief Open a document from an existing file path.
     * @param path Local filesystem path to a PDF file.
     * @details Delegates to the model; emits `documentOpened` / `documentError` on result.
     */
    void setDocumentPath(const QString& path);

    /**
     * @brief Open a PDF from memory.
     * @param data Raw PDF bytes.
     * @param errorMessage Optional out-parameter for failure text (same as `documentError()` when set).
     * @return False when unsupported or invalid; see `documentError()` and optional `errorMessage`.
     */
    [[nodiscard]] bool openDocumentBuffer(const QByteArray& data, QString* errorMessage = nullptr);

    /** @brief Last path passed to `setDocumentPath` (may be empty). */
    [[nodiscard]] QString documentPath() const;

    /** @brief Human-readable document source from the model (`sourceLabel()`). */
    [[nodiscard]] QString documentSourceLabel() const;

    /** @brief True when the model reports an open document. */
    [[nodiscard]] bool isDocumentOpen() const;

    /** @brief Last document open error text from the model (empty when none). */
    [[nodiscard]] QString documentError() const;

    /** @brief Page count from the model (zero when closed). */
    [[nodiscard]] int pageCount() const;

    /** @brief Current zero-based page index. */
    [[nodiscard]] int currentPageIndex() const;

    /** @brief Set current page; clamped to valid range when a document is open. */
    void setCurrentPageIndex(int index);

    /** @brief Advance to the next page if possible. */
    void nextPage();

    /** @brief Go to the previous page if possible. */
    void previousPage();

    /** @brief Logical zoom factor (1.0 = 100%). */
    [[nodiscard]] double zoom() const;

    /** @brief Set zoom and refresh layout / raster caches. */
    void setZoom(double zoom);

    /** @brief Restore default zoom (1.0). */
    void resetZoom();

    /** @brief Zoom so the page width fits the viewport (marks user-adjusted zoom). */
    void fitWidth();

    /** @brief Background color drawn behind PDF page content (“paper”). */
    [[nodiscard]] QColor paperColor() const;

    /** @brief Set paper color; invalidates cached rasters/tiles. */
    void setPaperColor(const QColor& color);

    /**
     * @brief Run a full-document search for @p needle (backend-capped; does not log hit text).
     * @details On success with matches, scrolls to the first match. Clears selection when empty.
     */
    void findText(const QString& needle);

    /** @brief Move active find selection to the next match (wraps). */
    void findNext();

    /** @brief Move active find selection to the previous match (wraps). */
    void findPrevious();

    /** @brief Clear find matches and active index; repaint. */
    void clearFind();

    /** @brief Number of matches from the last `findText` / refresh pass. */
    [[nodiscard]] int findMatchCount() const;

    /**
     * @brief Zero-based index into `findMatches()`, or -1 when none active.
     */
    [[nodiscard]] int findCurrentIndex() const;

    /**
     * @brief Copy of the current find match list.
     * @details Indices align with `findCurrentIndex()` and `findResultsChanged(..., currentIndex)`.
     */
    [[nodiscard]] QVector<TextMatch> findMatches() const;

    /**
     * @brief The active find hit, or a default `TextMatch` (`pageIndex == -1`) when none.
     */
    [[nodiscard]] TextMatch currentFindMatch() const;

    /**
     * @brief Select match @p index, switch page if needed, scroll hit into view.
     */
    void scrollToFindMatch(int index);

    /**
     * @brief Same as `scrollToFindMatch(int)` when @p match equals a list entry (same page + rect).
     * @details Otherwise no-op (pass only hits from `findMatches()` / `currentFindMatch()`).
     */
    void scrollToFindMatch(const TextMatch& match);

    /**
     * @brief Scroll @p pageRect (page space) into view; switch to @p pageIndex if needed.
     * @details Does not modify find state. **GUI thread only.**
     */
    void revealPageRect(int pageIndex, const QRectF& pageRect);

    /**
     * @brief Scroll to a host search rectangle; optionally replace highlights with one find-styled box.
     * @param pageIndex Zero-based page containing @p pageRect.
     * @param pageRect Region in page space (same basis as `TextMatch::pageRect`).
     * @param showHighlight When true, replaces `pageHighlights` with one rectangle using find colors.
     */
    void navigateToHostSearchHit(int pageIndex, const QRectF& pageRect, bool showHighlight = true);

    /**
     * @brief Open a file and reveal an external search hit in one composition.
     * @param filePath Passed to `setDocumentPath` (same open semantics and signals).
     * @param searchTerm Best-effort: after scroll, runs the same match pass as `findText` **without**
     *        auto-scrolling to the first hit. If a hit matches `(pageIndex, pageRect)` exactly,
     *        `findCurrentIndex` aligns; else `findCurrentIndex` stays -1. Whitespace-only skips find.
     * @param pageIndex Target page (validated after open).
     * @param pageRect Region in **top-down page space** (same as `TextMatch::pageRect`).
     * @details On open failure, behavior reduces to `setDocumentPath` side effects only.
     */
    void openDocumentAndRevealSearchHit(const QString& filePath, const QString& searchTerm, int pageIndex, const QRectF& pageRect);

    /** @brief Fill color for non-active find hits. */
    [[nodiscard]] QColor findHighlightFillColor() const;
    void setFindHighlightFillColor(const QColor& color);

    /** @brief Outline color for the active find hit. */
    [[nodiscard]] QColor findActiveMatchOutlineColor() const;
    void setFindActiveMatchOutlineColor(const QColor& color);

    /** @brief Outline width (device-independent stroke) for the active find hit. */
    [[nodiscard]] qreal findActiveMatchOutlineWidth() const;
    void setFindActiveMatchOutlineWidth(qreal width);

    /**
     * @brief Replace host-owned highlights (page coordinates; independent of find).
     * @details GUI thread only; widget stores a **copy** and paints in `paintEvent`.
     */
    void setPageHighlights(const QVector<PageHighlight>& highlights);

    /** @brief Remove all host highlights. */
    void clearPageHighlights();

    /** @brief Copy of current host highlights. */
    [[nodiscard]] QVector<PageHighlight> pageHighlights() const;

    /**
     * @brief When true, paint path prefers device-space tiles (PDFium-capable builds).
     */
    [[nodiscard]] bool tileRenderingEnabled() const;
    void setTileRenderingEnabled(bool enabled);

    /**
     * @brief When true, tile rasterization may run on a worker thread (opt-in; default off).
     * @details Supported when the backend advertises `capabilities().asyncTileRendering` (PDFium
     *          and linked Poppler-Qt6); still intersected with `tileRenderingEnabled()`.
     */
    [[nodiscard]] bool asyncTileRenderingEnabled() const;
    void setAsyncTileRenderingEnabled(bool enabled);

    /**
     * @brief True when the backend can run async tiles and the tile paint path is active.
     * @details Unlike merged `capabilities().asyncTileRendering`, this does not require
     *          `asyncTileRenderingEnabled()` to already be true — use for gating UI that turns
     *          the opt-in on.
     */
    [[nodiscard]] bool asyncTileRenderingSupported() const;

signals:
    /** @brief Emitted after zoom, page, document, find, or highlight state affects the view. */
    void viewStateChanged();

    /**
     * @brief Document open finished (`ok` true on success).
     * @param ok False when the model reported open failure (see `documentError()`).
     */
    void documentOpened(bool ok);

    /** @brief Human-readable error from the document model or widget open path. */
    void documentError(const QString& message);

    /**
     * @brief Find match list or selection changed.
     * @param matchCount Size of `findMatches()` after the update.
     * @param currentIndex `findCurrentIndex()`; -1 when no active selection.
     */
    void findResultsChanged(int matchCount, int currentIndex);

protected:
    bool viewportEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void clampCurrentPage();
    void tryApplyAutoFitWidthOnOpen(bool suppressViewStateChanged);
    [[nodiscard]] std::optional<double> computeFitWidthZoom() const;
    void applyZoomChange(double zoomValue, bool suppressViewStateChanged = false);
    void updateScrollBars();
    QSize documentPixelSize() const;
    void devicePageBitmapSize(QSize* outDevicePx) const;
    [[nodiscard]] bool useTilePaintPath() const;
    void clearAllRasterAndTileState();
    void ensurePageRaster();
    void paintViewport(QPainter& painter, const QRect& clip);
    void paintViewportTiled(QPainter& painter);
    void paintViewportFullRaster(QPainter& painter);
    void enqueueMissingVisibleTiles(const QSize& docPx, const QSize& devPx, qreal dpr, const QRect& visDevTiles);
    void scheduleTileDrain();
    void processOnePendingTile();
#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    void setupPdfTileRenderThread();
    void shutdownPdfTileRenderThread();
    void purgeAsyncTileWorkerBlocking();
    [[nodiscard]] bool useAsyncWorkerTilePath() const;
    void onAsyncTileReady(quint64 generation,
        const QString& docIdentity,
        int pageIndex,
        int zoomMilli,
        int dprMilli,
        int col,
        int row,
        QImage image);
#endif
    [[nodiscard]] QRect viewportRectForTileDeviceRect(const QSize& docPx, qreal dpr, const QRect& tileDev) const;
    void scrollByPixels(int dx, int dy);
    void applyZoomAround(const QPointF& viewportPos, double newZoom);
    void scrollCurrentMatchVisible();
    void ensurePageRectVisibleInLayout(const QRectF& pageRectInPageSpace);
    void ensureFindMatchVisible(const TextMatch& match);
    /// Clears find state, then repopulates `m_findMatches` from @p trimmedNeedle when backend + document are available (same as
    /// `findText`'s collection step). Does **not** scroll or select an active index.
    void refreshFindMatches(const QString& trimmedNeedle);
    void paintPageHostHighlights(QPainter& painter, const QSizeF& pagePts, const QSize& docPx) const;
    void paintFindHighlights(QPainter& painter, const QSizeF& pagePts, const QSize& docPx) const;

    std::unique_ptr<PdfDocumentModel> m_documentModel;
    std::unique_ptr<PdfRenderBackend> m_backend;
    QString m_documentPath;
    QString m_documentIdentity;
    QString m_lastDocumentError;
    int m_currentPageIndex = 0;

    double m_zoom = 1.0;
    /// After a successful document open, fit width once the viewport has non-zero width (handles pre-show geometry).
    bool m_pendingAutoFitWidthOnOpen = false;
    /// User changed zoom (wheel, `setZoom`, `resetZoom`, or explicit `fitWidth`); suppress repeating auto-fit for that document.
    bool m_userAdjustedZoom = false;
    QColor m_paperColor = QColor(255, 255, 255);
    static constexpr QColor kMarginColor = QColor(0xd8, 0xd8, 0xd8);

    QVector<TextMatch> m_findMatches;
    int m_activeFindIndex = -1;
    QColor m_findHighlightFillColor;
    QColor m_findActiveMatchOutlineColor;
    qreal m_findActiveMatchOutlineWidth;

    QVector<PageHighlight> m_pageHighlights;

    QImage m_pageRaster;
    QString m_rasterIdentity;
    int m_rasterPage = -1;
    double m_rasterZoom = 0.0;
    QSize m_rasterDocPx;
    qreal m_rasterDpr = 0.0;

    bool m_tileRenderingEnabled = true;
    bool m_asyncTileRenderingEnabled = false;
    quint64 m_renderGeneration = 1;
    std::unique_ptr<PdfDocumentViewTileRuntime> m_tileRuntime;

#if defined(PDFDOCUMENTVIEW_ASYNC_TILE_WORKER) && PDFDOCUMENTVIEW_ASYNC_TILE_WORKER
    QThread* m_tileRenderThread = nullptr;
    PdfTileRenderWorker* m_tileRenderWorker = nullptr;
#endif
};

} // namespace pdf_document_view

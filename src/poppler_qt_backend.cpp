/**
 * @file poppler_qt_backend.cpp
 * @brief Poppler-Qt6 full-page raster backend when `PDFDOCUMENTVIEW_POPPLER_REAL` is on.
 */

#include "poppler_qt_backend.hpp"

#include "poppler_qt_document_model.hpp"

#include <poppler/qt6/poppler-document.h>
#include <poppler/qt6/poppler-page.h>

#include <memory>

#include <pdf_document_view/pdf_capabilities.hpp>

#include <QColor>
#include <QImage>
#include <QMutexLocker>
#include <QPainter>

namespace pdf_document_view {
namespace {

constexpr int kMaxFindMatches = 2000;

/// Poppler `Page::search` returns PDF user-space rects (points, Y-up). Convert to the same
/// top-down page space as `PdfDocumentModel::pageSize()` / PDFium find (`TextMatch::pageRect`).
[[nodiscard]] QRectF popplerUserRectToPageTopDown(double left,
    double top,
    double right,
    double bottom,
    double pageHeightPts) {
    const double qtLeft = left;
    const double qtRight = right;
    const double qtTop = pageHeightPts - top;
    const double qtBottom = pageHeightPts - bottom;
    return QRectF(qtLeft, qtTop, qtRight - qtLeft, qtBottom - qtTop).normalized();
}

/// Renders @p tileRectInFullPageDeviceSpace from an already-open `Poppler::Document` (any thread when
/// @p doc is not shared with concurrent users — async jobs load a private document per job).
[[nodiscard]] bool popplerRenderTileFromOpenDocument(Poppler::Document* doc,
    const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out) {
    if (!out || !doc || doc->isLocked()) {
        return false;
    }
    *out = QImage();

    const int pageIndex = request.pageIndex;
    if (pageIndex < 0 || pageIndex >= doc->numPages()) {
        return false;
    }

    std::unique_ptr<Poppler::Page> page(doc->page(pageIndex));
    if (!page) {
        return false;
    }

    const QRectF pr = page->pageRectF(Poppler::Page::CropBox);
    const double pw = qMax(1.0e-6, static_cast<double>(pr.width()));
    const double ph = qMax(1.0e-6, static_cast<double>(pr.height()));

    const int bw = qMax(1, request.devicePixelSize.width());
    const int bh = qMax(1, request.devicePixelSize.height());

    const QRect tr = tileRectInFullPageDeviceSpace.intersected(QRect(0, 0, bw, bh));
    if (!tr.isValid() || tr.isEmpty()) {
        return false;
    }

    const double xDpi = 72.0 * static_cast<double>(bw) / pw;
    const double yDpi = 72.0 * static_cast<double>(bh) / ph;

    QImage img = page->renderToImage(xDpi,
        yDpi,
        tr.x(),
        tr.y(),
        tr.width(),
        tr.height(),
        Poppler::Page::Rotate0);
    if (img.isNull()) {
        return false;
    }

    if (img.format() != QImage::Format_ARGB32_Premultiplied && img.format() != QImage::Format_RGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    *out = std::move(img);
    return true;
}

} // namespace

PopplerQtBackend::PopplerQtBackend() = default;

PopplerQtBackend::~PopplerQtBackend() {
    attachDocumentModel(nullptr);
}

void PopplerQtBackend::attachDocumentModel(PopplerQtDocumentModel* model) {
    m_model = model;
}

QString PopplerQtBackend::id() const {
    return QStringLiteral("poppler-qt6");
}

QString PopplerQtBackend::displayName() const {
    return QStringLiteral("Poppler (Qt6)");
}

PdfWidgetCapabilities PopplerQtBackend::capabilities() const {
    return PdfWidgetCapabilities{.textSearch = true,
        .renderPages = true,
        .openFromBuffer = m_model ? m_model->supportsOpenBuffer() : false,
        .tileRendering = true,
        .asyncTileRendering = true};
}

void PopplerQtBackend::setDocumentPath(const QString& path) {
    (void)path;
}

int PopplerQtBackend::pageCount() const {
    return m_model ? m_model->pageCount() : 0;
}

QSizeF PopplerQtBackend::pageSize(int pageIndex) const {
    return m_model ? m_model->pageSize(pageIndex) : QSizeF{};
}

void PopplerQtBackend::renderPage(const PageRenderRequest& request, QPainter& painter, const QRectF& targetRect) {
    (void)request.devicePixelRatio;

    if (!m_model) {
        painter.fillRect(targetRect, QColor(235, 235, 235));
        painter.setPen(QColor(90, 90, 90));
        painter.drawText(targetRect, Qt::AlignCenter, QStringLiteral("No document"));
        return;
    }

    QMutexLocker locker(&m_model->m_mutex);

    auto* doc = static_cast<Poppler::Document*>(m_model->m_document);
    if (!doc) {
        painter.fillRect(targetRect, QColor(235, 235, 235));
        painter.setPen(QColor(90, 90, 90));
        painter.drawText(targetRect, Qt::AlignCenter, QStringLiteral("No document"));
        return;
    }

    const int pageIndex = request.pageIndex;
    if (pageIndex < 0 || pageIndex >= doc->numPages()) {
        painter.fillRect(targetRect, QColor(235, 235, 235));
        return;
    }

    std::unique_ptr<Poppler::Page> page(doc->page(pageIndex));
    if (!page) {
        painter.fillRect(targetRect, QColor(255, 220, 220));
        painter.setPen(Qt::darkRed);
        painter.drawText(targetRect, Qt::AlignCenter, QStringLiteral("Page load failed"));
        return;
    }

    const QRectF pr = page->pageRectF(Poppler::Page::CropBox);
    const double pw = qMax(1.0e-6, static_cast<double>(pr.width()));
    const double ph = qMax(1.0e-6, static_cast<double>(pr.height()));

    const int bw = qMax(1, request.devicePixelSize.width());
    const int bh = qMax(1, request.devicePixelSize.height());

    const double xDpi = 72.0 * static_cast<double>(bw) / pw;
    const double yDpi = 72.0 * static_cast<double>(bh) / ph;

    QImage img = page->renderToImage(xDpi, yDpi, -1, -1, -1, -1, Poppler::Page::Rotate0);
    if (img.isNull()) {
        painter.fillRect(targetRect, QColor(255, 200, 200));
        return;
    }

    if (img.format() != QImage::Format_ARGB32_Premultiplied && img.format() != QImage::Format_RGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    painter.save();
    painter.drawImage(targetRect, img);
    painter.restore();
}

bool PopplerQtBackend::renderPageTile(const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out) {
    if (!out) {
        return false;
    }
    *out = QImage();

    if (!m_model) {
        return false;
    }

    QMutexLocker locker(&m_model->m_mutex);

    auto* doc = static_cast<Poppler::Document*>(m_model->m_document);
    if (!doc) {
        return false;
    }

    return popplerRenderTileFromOpenDocument(doc, request, tileRectInFullPageDeviceSpace, out);
}

bool PopplerQtBackend::renderPageTile(const QByteArray& detachedPdfBytes,
    const QString& detachedAbsolutePathIfBytesEmpty,
    const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out) {
    if (!out) {
        return false;
    }
    *out = QImage();

    std::unique_ptr<Poppler::Document> doc;
    if (!detachedPdfBytes.isEmpty()) {
        doc = Poppler::Document::loadFromData(detachedPdfBytes);
    } else if (!detachedAbsolutePathIfBytesEmpty.isEmpty()) {
        doc = Poppler::Document::load(detachedAbsolutePathIfBytesEmpty);
    }
    if (!doc || doc->isLocked()) {
        return false;
    }

    return popplerRenderTileFromOpenDocument(doc.get(), request, tileRectInFullPageDeviceSpace, out);
}

int PopplerQtBackend::findTextMatches(const QString& needle, QVector<TextMatch>* out) const {
    const QString q = needle.trimmed();
    if (q.isEmpty()) {
        if (out) {
            out->clear();
        }
        return 0;
    }

    if (!m_model) {
        if (out) {
            out->clear();
        }
        return 0;
    }

    QMutexLocker locker(&m_model->m_mutex);

    auto* doc = static_cast<Poppler::Document*>(m_model->m_document);
    if (!doc) {
        if (out) {
            out->clear();
        }
        return 0;
    }

    const int n = doc->numPages();
    int total = 0;

    if (out) {
        out->clear();
    }

    for (int pi = 0; pi < n && total < kMaxFindMatches; ++pi) {
        const double pageH = (pi < m_model->m_pageSizes.size()) ? m_model->m_pageSizes.at(pi).height() : 792.0;

        std::unique_ptr<Poppler::Page> page(doc->page(pi));
        if (!page) {
            continue;
        }

        const QList<QRectF> hits = page->search(q, Poppler::Page::NoSearchFlags, Poppler::Page::Rotate0);
        for (const QRectF& r : hits) {
            if (total >= kMaxFindMatches) {
                break;
            }
            const double pdfLeft = qMin(r.left(), r.right());
            const double pdfRight = qMax(r.left(), r.right());
            const double pdfYMin = qMin(r.top(), r.bottom());
            const double pdfYMax = qMax(r.top(), r.bottom());
            ++total;
            if (out) {
                out->push_back(TextMatch{pi,
                    popplerUserRectToPageTopDown(pdfLeft, pdfYMax, pdfRight, pdfYMin, pageH)});
            }
        }
    }

    return total;
}

} // namespace pdf_document_view

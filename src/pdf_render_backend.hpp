#pragma once

#include <pdf_document_view/page_render_request.hpp>
#include <pdf_document_view/pdf_capabilities.hpp>
#include <pdf_document_view/text_match.hpp>

#include <QByteArray>
#include <QRect>
#include <QRectF>
#include <QSizeF>
#include <QString>
#include <QVector>

class QImage;
class QPainter;

namespace pdf_document_view {

/// Narrow render surface for document pages (stub / engine implementations live in `src/`).
class PdfRenderBackend {
public:
    virtual ~PdfRenderBackend() = default;

    [[nodiscard]] virtual QString id() const = 0;
    [[nodiscard]] virtual QString displayName() const = 0;
    [[nodiscard]] virtual PdfWidgetCapabilities capabilities() const = 0;
    [[nodiscard]] virtual int pageCount() const = 0;
    [[nodiscard]] virtual QSizeF pageSize(int pageIndex) const = 0;
    virtual void renderPage(const PageRenderRequest& request, QPainter& painter, const QRectF& targetRect) = 0;

    /// Renders a sub-rectangle of the page at the same scale as @p request.devicePixelSize (full-page device bitmap).
    /// @p tileRectInFullPageDeviceSpace uses top-left origin in that bitmap's device pixels.
    /// Default: unsupported (returns false); the widget may fall back to full-page raster.
    [[nodiscard]] virtual bool renderPageTile(const PageRenderRequest& request,
        const QRect& tileRectInFullPageDeviceSpace,
        QImage* out) {
        (void)request;
        (void)tileRectInFullPageDeviceSpace;
        (void)out;
        return false;
    }

    /// Same mapping and device semantics as `renderPageTile(request, …)`, but rasterizes from a
    /// detached PDF copy (`pdfBytes` or `absolutePathIfBytesEmpty`) for off-GUI-thread use.
    /// Default: unsupported. PDFium implements; GUI-thread tiles use the live-model overload above.
    [[nodiscard]] virtual bool renderPageTile(const QByteArray& detachedPdfBytes,
        const QString& detachedAbsolutePathIfBytesEmpty,
        const PageRenderRequest& request,
        const QRect& tileRectInFullPageDeviceSpace,
        QImage* out) {
        (void)detachedPdfBytes;
        (void)detachedAbsolutePathIfBytesEmpty;
        (void)request;
        (void)tileRectInFullPageDeviceSpace;
        (void)out;
        return false;
    }

    /// Opens or clears the backing document when the path changes (engine-specific).
    virtual void setDocumentPath(const QString& path) { (void)path; }

    /// Collects text search hits (implementation-defined limits may cap count). Returns match count written.
    /// When @p out is null, returns total matches without storing (same cap). Default: no search support.
    [[nodiscard]] virtual int findTextMatches(const QString& needle, QVector<TextMatch>* out) const {
        if (out) {
            out->clear();
        }
        (void)needle;
        return 0;
    }
};

} // namespace pdf_document_view

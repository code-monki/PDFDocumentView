#pragma once

#include <pdf_document_view/page_render_request.hpp>

#include <QByteArray>
#include <QImage>
#include <QRect>
#include <QString>

namespace pdf_document_view {

/// Renders one device-space tile using a **fresh** `FPDF_DOCUMENT` from bytes or file path (no shared model handle).
[[nodiscard]] bool pdfiumRenderPageTileIsolated(const QByteArray& pdfBytes,
    const QString& absolutePathIfBytesEmpty,
    const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out);

} // namespace pdf_document_view

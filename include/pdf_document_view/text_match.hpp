/**
 * @file text_match.hpp
 * @brief One text-search hit with geometry in page space.
 *
 * @details Coordinates match `PageHighlight::pageRect`, `PdfDocumentModel::pageSize()`, and
 * `docs/widget-coordinate-contract.md` (top-down PDF user units).
 */

#pragma once

#include <QRectF>

namespace pdf_document_view {

/**
 * @brief One text-search hit in **page space**.
 *
 * @details Origin top-left, Y downward, **1/72 in (PDF user units)** after conversion from
 * engine-native conventions (e.g. PDFium Y-up) to the widget’s public contract.
 */
struct TextMatch {
    /** @brief Zero-based page index, or -1 when invalid / sentinel. */
    int pageIndex = -1;
    /** @brief Axis-aligned hit rectangle in page space (see file brief). */
    QRectF pageRect;
};

} // namespace pdf_document_view

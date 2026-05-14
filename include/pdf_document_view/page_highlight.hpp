/**
 * @file page_highlight.hpp
 * @brief Host-provided highlight rectangle and colors in page space.
 *
 * @details Independent of built-in find highlights; the widget stores a copy and paints on the
 * GUI thread. See `docs/widget-coordinate-contract.md`.
 */

#pragma once

#include <QColor>
#include <QRectF>

namespace pdf_document_view {

/**
 * @brief Host-drawn highlight in **page space** (same basis as `TextMatch::pageRect`).
 *
 * @details Origin top-left, Y downward, 1/72 in aligned with `PdfDocumentModel::pageSize()` /
 * render backends.
 */
struct PageHighlight {
    /** @brief Zero-based page index, or -1 if unused. */
    int pageIndex = -1;
    /** @brief Region to fill and outline in page space. */
    QRectF pageRect;
    /** @brief Fill color (including alpha) for the interior. */
    QColor fill;
    /** @brief Color used to stroke the highlight outline. */
    QColor border;
};

} // namespace pdf_document_view

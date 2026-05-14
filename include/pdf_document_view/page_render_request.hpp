/**
 * @file page_render_request.hpp
 * @brief Parameters for a full-page rasterization request (device pixels + paper color).
 *
 * @details Used by internal render paths; public struct for backend / contract alignment.
 * Additive fields follow MINOR semver per ADR-0004. See `docs/widget-coordinate-contract.md`.
 */

#pragma once

#include <QColor>
#include <QSize>
#include <QtGlobal>

namespace pdf_document_view {

/**
 * @brief Device-pixel render parameters for one full-page rasterization.
 */
struct PageRenderRequest {
    /** @brief Zero-based page index. */
    int pageIndex = 0;
    /** @brief Device pixel ratio (Qt screen DPR) for the target surface. */
    qreal devicePixelRatio = 1.0;
    /** @brief Target bitmap dimensions in device pixels (full page at current zoom / DPR). */
    QSize devicePixelSize;
    /** @brief Background behind transparent PDF regions (full-page and tile paths). */
    QColor paperColor = QColor(255, 255, 255);
};

} // namespace pdf_document_view

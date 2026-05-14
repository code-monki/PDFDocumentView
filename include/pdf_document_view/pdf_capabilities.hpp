/**
 * @file pdf_capabilities.hpp
 * @brief Feature flags for the active render backend and widget stack.
 *
 * @details Values reflect compile-time backend and runtime policy (e.g. buffer open intersected
 * with `PdfDocumentModel::supportsOpenBuffer()`). New fields may appear in MINOR releases with
 * safe defaults; see ADR-0004.
 */

#pragma once

namespace pdf_document_view {

/**
 * @brief Capability bits merged by `PdfDocumentViewWidget::capabilities()`.
 *
 * @details Frozen additive rules per `docs/adrs/0004-public-api-surface-freeze.md`.
 */
struct PdfWidgetCapabilities {
    /** @brief Backend can run find / text match collection used by the widget. */
    bool textSearch = false;
    /** @brief Backend can rasterize pages for display. */
    bool renderPages = false;
    /** @brief Engine can open from memory; widget also requires `PdfDocumentModel::supportsOpenBuffer()`. */
    bool openFromBuffer = false;
    /**
     * @brief Device-space tile cache paint path is available (PDFium; Poppler-Qt6 when REAL).
     */
    bool tileRendering = false;
    /**
     * @brief Backend can rasterize tiles from a detached PDF copy on a worker thread (see
     *        `PdfRenderBackend::renderPageTile(QByteArray, QString, …)`).
     * @details Intersected in `PdfDocumentViewWidget::capabilities()` with `tileRenderingEnabled`
     *          and `asyncTileRenderingEnabled`. Stub Poppler: false.
     */
    bool asyncTileRendering = false;
};

} // namespace pdf_document_view

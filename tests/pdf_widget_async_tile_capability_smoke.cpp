/**
 * @file pdf_widget_async_tile_capability_smoke.cpp
 * @brief Verifies tile + async capability wiring for backends that ship the tile worker (PDFium, Poppler-Qt6).
 */

#include <pdf_document_view/pdf_document_view_widget.hpp>

#include <QApplication>
#include <QDebug>

#include <cstdlib>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    pdf_document_view::PdfDocumentViewWidget w;
    w.setTileRenderingEnabled(true);
    if (!w.capabilities().tileRendering) {
        // Poppler stub and other minimal backends: nothing to assert.
        return EXIT_SUCCESS;
    }

    w.setAsyncTileRenderingEnabled(true);
    if (!w.asyncTileRenderingEnabled()) {
        qWarning("pdf_widget_async_tile_capability_smoke: setAsyncTileRenderingEnabled did not enable");
        return 1;
    }
    if (!w.capabilities().asyncTileRendering) {
        qWarning("pdf_widget_async_tile_capability_smoke: capabilities().asyncTileRendering false after enable");
        return 2;
    }

    return EXIT_SUCCESS;
}

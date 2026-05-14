/**
 * @file create_default_pdf_render_backend.cpp
 * @brief Selects PDFium, Poppler Qt, or Poppler stub render backend at compile time.
 */

#include "create_default_pdf_render_backend.hpp"

#include "pdf_render_backend.hpp"

#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
#include "pdfium_backend.hpp"
#elif defined(PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER) && PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER
#if defined(PDFDOCUMENTVIEW_POPPLER_REAL) && PDFDOCUMENTVIEW_POPPLER_REAL
#include "poppler_qt_backend.hpp"
#else
#include "poppler_stub_backend.hpp"
#endif
#else
#error "PDFDocumentView: define PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM=1 or PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER=1"
#endif

namespace pdf_document_view {

std::unique_ptr<PdfRenderBackend> createDefaultPdfRenderBackend() {
#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
    return std::make_unique<PdfiumBackend>();
#elif defined(PDFDOCUMENTVIEW_POPPLER_REAL) && PDFDOCUMENTVIEW_POPPLER_REAL
    return std::make_unique<PopplerQtBackend>();
#else
    return std::make_unique<PopplerStubBackend>();
#endif
}

} // namespace pdf_document_view

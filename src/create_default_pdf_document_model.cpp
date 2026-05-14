/**
 * @file create_default_pdf_document_model.cpp
 * @brief Selects `PdfiumDocumentModel`, `PopplerQtDocumentModel`, or stub at compile time.
 */

#include "pdf_document_view/pdf_document_model.hpp"

#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
#include "pdfium_document_model.hpp"
#elif defined(PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER) && PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER
#if defined(PDFDOCUMENTVIEW_POPPLER_REAL) && PDFDOCUMENTVIEW_POPPLER_REAL
#include "poppler_qt_document_model.hpp"
#else
#include "stub_pdf_document_model.hpp"
#endif
#else
#error "PDFDocumentView: define PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM=1 or PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER=1"
#endif

namespace pdf_document_view {

std::unique_ptr<PdfDocumentModel> createDefaultPdfDocumentModel() {
#if defined(PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM) && PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM
    return std::make_unique<PdfiumDocumentModel>();
#elif defined(PDFDOCUMENTVIEW_POPPLER_REAL) && PDFDOCUMENTVIEW_POPPLER_REAL
    return std::make_unique<PopplerQtDocumentModel>();
#else
    return std::make_unique<StubPdfDocumentModel>();
#endif
}

} // namespace pdf_document_view

#pragma once

#include <memory>

namespace pdf_document_view {

class PdfRenderBackend;

[[nodiscard]] std::unique_ptr<PdfRenderBackend> createDefaultPdfRenderBackend();

} // namespace pdf_document_view

#include <pdf_document_view/pdf_document_model.hpp>

#include <QtGlobal>

int main() {
    auto model = pdf_document_view::createDefaultPdfDocumentModel();
    return model ? 0 : 1;
}

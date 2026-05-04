#include "pdf_document_view/pdf_document_view_widget.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace pdf_document_view {

PdfDocumentViewWidget::PdfDocumentViewWidget(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* label = new QLabel(tr("PDFDocumentView — placeholder widget"), this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

} // namespace pdf_document_view

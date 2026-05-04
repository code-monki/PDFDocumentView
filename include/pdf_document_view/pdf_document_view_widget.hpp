#pragma once

#include <QWidget>

namespace pdf_document_view {

/// Placeholder embeddable widget. Public API will evolve with backend selection (see docs/concept-pdf-document-view.md).
class PdfDocumentViewWidget final : public QWidget {
    Q_OBJECT

public:
    explicit PdfDocumentViewWidget(QWidget* parent = nullptr);
};

} // namespace pdf_document_view

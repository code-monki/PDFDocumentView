#pragma once

#include "pdf_document_view/pdf_document_model.hpp"

namespace pdf_document_view {

class StubPdfDocumentModel final : public PdfDocumentModel {
public:
    [[nodiscard]] bool openFile(const QString& path, QString* errorMessage) override;
    [[nodiscard]] bool openBuffer(const QByteArray& data, QString* errorMessage) override;
    void close() override;
    [[nodiscard]] bool isOpen() const override;
    [[nodiscard]] int pageCount() const override;
    [[nodiscard]] QSizeF pageSize(int pageIndex) const override;
    [[nodiscard]] QString lastError() const override;
    [[nodiscard]] QString identityKey() const override;
    [[nodiscard]] QString sourceLabel() const override;

private:
    QString m_lastError;
};

} // namespace pdf_document_view

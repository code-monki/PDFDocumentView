/**
 * @file stub_pdf_document_model.cpp
 * @brief Poppler stub document model: `openFile`/`openBuffer` fail with a reconfigure message.
 */

#include "stub_pdf_document_model.hpp"

namespace pdf_document_view {

bool StubPdfDocumentModel::openFile(const QString& /*path*/, QString* errorMessage) {
    m_lastError = QStringLiteral(
        "Poppler stub: no PDF engine linked. Reconfigure with "
        "-DPDFDOCUMENTVIEW_POPPLER_REAL=ON (requires poppler-qt6 / pkg-config poppler-qt6), "
        "or use PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM.");
    if (errorMessage) {
        *errorMessage = m_lastError;
    }
    return false;
}

bool StubPdfDocumentModel::openBuffer(const QByteArray& /*data*/, QString* errorMessage) {
    m_lastError = QStringLiteral(
        "Poppler stub: buffer open is not available. Use -DPDFDOCUMENTVIEW_POPPLER_REAL=ON with "
        "Poppler-Qt6, or PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM.");
    if (errorMessage) {
        *errorMessage = m_lastError;
    }
    return false;
}

void StubPdfDocumentModel::close() {
    m_lastError.clear();
}

bool StubPdfDocumentModel::isOpen() const {
    return false;
}

int StubPdfDocumentModel::pageCount() const {
    return 0;
}

QSizeF StubPdfDocumentModel::pageSize(int /*pageIndex*/) const {
    return {};
}

QString StubPdfDocumentModel::lastError() const {
    return m_lastError;
}

QString StubPdfDocumentModel::identityKey() const {
    return {};
}

QString StubPdfDocumentModel::sourceLabel() const {
    return {};
}

} // namespace pdf_document_view

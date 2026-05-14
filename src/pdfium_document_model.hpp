#pragma once

#include "pdf_document_view/pdf_document_model.hpp"

#include <QByteArray>
#include <QMutex>
#include <QString>
#include <QVector>

namespace pdf_document_view {

class PdfiumBackend;

/// PDFium-backed document model; owns `FPDF_DOCUMENT` and page size cache.
class PdfiumDocumentModel final : public PdfDocumentModel {
    friend class PdfiumBackend;

public:
    PdfiumDocumentModel();
    ~PdfiumDocumentModel() override;

    [[nodiscard]] bool openFile(const QString& path, QString* errorMessage) override;
    [[nodiscard]] bool openBuffer(const QByteArray& data, QString* errorMessage) override;
    [[nodiscard]] bool supportsOpenBuffer() const override;
    void close() override;
    [[nodiscard]] bool isOpen() const override;
    [[nodiscard]] int pageCount() const override;
    [[nodiscard]] QSizeF pageSize(int pageIndex) const override;
    [[nodiscard]] QString lastError() const override;
    [[nodiscard]] QString identityKey() const override;
    [[nodiscard]] QString sourceLabel() const override;

    /// Copy of the in-memory PDF bytes when opened via `openBuffer`; empty when opened from file.
    [[nodiscard]] QByteArray savedCopy() const;
    /// Absolute path when opened via `openFile`; empty for buffer-backed documents.
    [[nodiscard]] QString loadedFilePath() const;

private:
    void closeUnlocked();
    [[nodiscard]] bool loadFromLoadedDocument(void* doc, QString* errorMessage);
    void setLastError(const QString& message);

    mutable QMutex m_mutex;
    void* m_doc = nullptr;
    QVector<QSizeF> m_pageSizes;
    QString m_lastError;
    QString m_absolutePath;
    QByteArray m_ownedBuffer;
    QString m_identityKey;
    quint64 m_bufferOpenSeq = 0;
};

} // namespace pdf_document_view

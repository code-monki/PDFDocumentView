#pragma once

#include "pdf_document_view/pdf_document_model.hpp"

#include <QByteArray>
#include <QMutex>
#include <QString>
#include <QVector>

namespace pdf_document_view {

class PopplerQtBackend;

/// Poppler-Qt6 document model (file open, page sizes). See ADR-0003 / README for GPL obligations.
class PopplerQtDocumentModel final : public PdfDocumentModel {
    friend class PopplerQtBackend; // render path reads m_mutex + m_document under lock

public:
    PopplerQtDocumentModel();
    ~PopplerQtDocumentModel() override;

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

    /// Copy of the in-memory PDF used for buffer opens (empty when opened from file).
    [[nodiscard]] QByteArray savedCopy() const;
    /// Absolute path when the document was opened from a file (empty for buffer-only opens).
    [[nodiscard]] QString loadedFilePath() const;

private:
    void closeUnlocked();
    void setLastError(const QString& message);

    mutable QMutex m_mutex;
    void* m_document = nullptr; // Poppler::Document*
    QVector<QSizeF> m_pageSizes;
    QString m_lastError;
    QString m_absolutePath;
    QByteArray m_ownedBuffer;
    QString m_identityKey;
};

} // namespace pdf_document_view

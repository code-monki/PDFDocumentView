/**
 * @file pdfium_document_model.cpp
 * @brief PDFium-backed `PdfDocumentModel`: `FPDF_LoadDocument` from file or custom access for buffers.
 */

#include "pdfium_document_model.hpp"

#include "pdfium_library.hpp"

#include <QFile>
#include <QFileInfo>

extern "C" {
#include "fpdfview.h"
}

namespace pdf_document_view {
namespace {

QString describePdfiumLoadError(unsigned long code) {
    switch (code) {
    case FPDF_ERR_SUCCESS:
        return QStringLiteral("success");
    case FPDF_ERR_UNKNOWN:
        return QStringLiteral("unknown error");
    case FPDF_ERR_FILE:
        return QStringLiteral("file access error");
    case FPDF_ERR_FORMAT:
        return QStringLiteral("invalid PDF format");
    case FPDF_ERR_PASSWORD:
        return QStringLiteral("password required or incorrect");
    case FPDF_ERR_SECURITY:
        return QStringLiteral("unsupported security scheme");
    case FPDF_ERR_PAGE:
        return QStringLiteral("page error");
    default:
        return QStringLiteral("PDFium error code %1").arg(static_cast<qulonglong>(code));
    }
}

} // namespace

PdfiumDocumentModel::PdfiumDocumentModel() {
    pdfium_library::ensureLibrary();
}

PdfiumDocumentModel::~PdfiumDocumentModel() {
    close();
    pdfium_library::releaseLibrary();
}

void PdfiumDocumentModel::setLastError(const QString& message) {
    m_lastError = message;
}

void PdfiumDocumentModel::closeUnlocked() {
    if (m_doc) {
        FPDF_CloseDocument(reinterpret_cast<FPDF_DOCUMENT>(m_doc));
        m_doc = nullptr;
    }
    m_pageSizes.clear();
    m_absolutePath.clear();
    m_ownedBuffer.clear();
    m_identityKey.clear();
    m_lastError.clear();
}

void PdfiumDocumentModel::close() {
    QMutexLocker locker(&m_mutex);
    closeUnlocked();
}

bool PdfiumDocumentModel::loadFromLoadedDocument(void* docPtr, QString* errorMessage) {
    auto* doc = reinterpret_cast<FPDF_DOCUMENT>(docPtr);
    const int n = FPDF_GetPageCount(doc);
    if (n <= 0) {
        const QString msg = QStringLiteral("Document has no pages.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        FPDF_CloseDocument(doc);
        return false;
    }

    m_pageSizes.resize(n);
    for (int i = 0; i < n; ++i) {
        FPDF_PAGE page = FPDF_LoadPage(doc, i);
        if (!page) {
            m_pageSizes[i] = QSizeF(612.0, 792.0);
            continue;
        }
        m_pageSizes[i] = QSizeF(static_cast<double>(FPDF_GetPageWidthF(page)),
            static_cast<double>(FPDF_GetPageHeightF(page)));
        FPDF_ClosePage(page);
    }

    m_doc = doc;
    m_lastError.clear();
    return true;
}

bool PdfiumDocumentModel::openFile(const QString& path, QString* errorMessage) {
    QMutexLocker locker(&m_mutex);
    closeUnlocked();

    if (path.trimmed().isEmpty()) {
        const QString msg = QStringLiteral("Empty file path.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    const QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) {
        const QString msg = QStringLiteral("Path is not a readable file: %1").arg(path);
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    m_absolutePath = fi.absoluteFilePath();
    m_identityKey = m_absolutePath;
    const QByteArray encoded = QFile::encodeName(m_absolutePath);
    FPDF_DOCUMENT doc = FPDF_LoadDocument(encoded.constData(), nullptr);
    if (!doc) {
        const unsigned long err = FPDF_GetLastError();
        const QString msg =
            QStringLiteral("Failed to load PDF (%1).").arg(describePdfiumLoadError(err));
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        m_absolutePath.clear();
        m_identityKey.clear();
        return false;
    }

    if (!loadFromLoadedDocument(doc, errorMessage)) {
        m_absolutePath.clear();
        m_identityKey.clear();
        return false;
    }

    return true;
}

bool PdfiumDocumentModel::openBuffer(const QByteArray& data, QString* errorMessage) {
    QMutexLocker locker(&m_mutex);
    closeUnlocked();

    if (data.isEmpty()) {
        const QString msg = QStringLiteral("Empty PDF buffer.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    m_ownedBuffer = data;
    ++m_bufferOpenSeq;
    m_identityKey = QStringLiteral("buffer:%1").arg(m_bufferOpenSeq);
    m_absolutePath.clear();

    FPDF_DOCUMENT doc = FPDF_LoadMemDocument64(
        m_ownedBuffer.constData(), static_cast<size_t>(m_ownedBuffer.size()), nullptr);
    if (!doc) {
        const unsigned long err = FPDF_GetLastError();
        const QString msg =
            QStringLiteral("Failed to load PDF from memory (%1).").arg(describePdfiumLoadError(err));
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        m_ownedBuffer.clear();
        m_identityKey.clear();
        return false;
    }

    if (!loadFromLoadedDocument(doc, errorMessage)) {
        m_ownedBuffer.clear();
        m_identityKey.clear();
        return false;
    }

    return true;
}

bool PdfiumDocumentModel::supportsOpenBuffer() const {
    return true;
}

bool PdfiumDocumentModel::isOpen() const {
    QMutexLocker locker(&m_mutex);
    return m_doc != nullptr;
}

int PdfiumDocumentModel::pageCount() const {
    QMutexLocker locker(&m_mutex);
    return m_pageSizes.size();
}

QSizeF PdfiumDocumentModel::pageSize(int pageIndex) const {
    QMutexLocker locker(&m_mutex);
    if (pageIndex < 0 || pageIndex >= m_pageSizes.size()) {
        return {};
    }
    return m_pageSizes.at(pageIndex);
}

QString PdfiumDocumentModel::lastError() const {
    QMutexLocker locker(&m_mutex);
    return m_lastError;
}

QString PdfiumDocumentModel::identityKey() const {
    QMutexLocker locker(&m_mutex);
    return m_identityKey;
}

QString PdfiumDocumentModel::sourceLabel() const {
    QMutexLocker locker(&m_mutex);
    if (!m_absolutePath.isEmpty()) {
        return m_absolutePath;
    }
    if (!m_ownedBuffer.isEmpty()) {
        return QStringLiteral("Memory buffer (%1 bytes)").arg(m_ownedBuffer.size());
    }
    return QString();
}

QByteArray PdfiumDocumentModel::savedCopy() const {
    QMutexLocker locker(&m_mutex);
    return m_ownedBuffer;
}

QString PdfiumDocumentModel::loadedFilePath() const {
    QMutexLocker locker(&m_mutex);
    return m_absolutePath;
}

} // namespace pdf_document_view

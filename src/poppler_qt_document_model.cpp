/**
 * @file poppler_qt_document_model.cpp
 * @brief Poppler-Qt6 document model: open from file or buffer, page sizes, mutex around `Poppler::Document`.
 */

#include "poppler_qt_document_model.hpp"

#include <poppler/qt6/poppler-qt6.h>

#include <memory>

#include <QFileInfo>
#include <QMutexLocker>

namespace pdf_document_view {

PopplerQtDocumentModel::PopplerQtDocumentModel() = default;

PopplerQtDocumentModel::~PopplerQtDocumentModel() {
    close();
}

void PopplerQtDocumentModel::setLastError(const QString& message) {
    m_lastError = message;
}

void PopplerQtDocumentModel::closeUnlocked() {
    if (m_document) {
        delete static_cast<Poppler::Document*>(m_document);
        m_document = nullptr;
    }
    m_pageSizes.clear();
    m_absolutePath.clear();
    m_ownedBuffer.clear();
    m_identityKey.clear();
    m_lastError.clear();
}

void PopplerQtDocumentModel::close() {
    QMutexLocker locker(&m_mutex);
    closeUnlocked();
}

bool PopplerQtDocumentModel::openFile(const QString& path, QString* errorMessage) {
    QMutexLocker locker(&m_mutex);
    closeUnlocked();

    const QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) {
        const QString msg = QStringLiteral("File does not exist or is not a regular file.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    const QString abs = fi.absoluteFilePath();
    std::unique_ptr<Poppler::Document> doc = Poppler::Document::load(abs);
    if (!doc) {
        const QString msg = QStringLiteral("Poppler could not load the PDF (invalid or unsupported).");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }
    if (doc->isLocked()) {
        const QString msg = QStringLiteral("Document is password-locked (not supported in this build path).");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    const int n = doc->numPages();
    if (n <= 0) {
        const QString msg = QStringLiteral("Document has no pages.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    m_pageSizes.resize(n);
    for (int i = 0; i < n; ++i) {
        std::unique_ptr<Poppler::Page> page(doc->page(i));
        if (!page) {
            m_pageSizes[i] = QSizeF(612.0, 792.0);
            continue;
        }
        const QSizeF pagePts = page->pageSizeF();
        m_pageSizes[i] = pagePts;
    }

    m_document = doc.release();
    m_absolutePath = abs;
    m_identityKey = abs;
    m_lastError.clear();
    return true;
}

bool PopplerQtDocumentModel::openBuffer(const QByteArray& data, QString* errorMessage) {
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

    std::unique_ptr<Poppler::Document> doc = Poppler::Document::loadFromData(data);
    if (!doc) {
        const QString msg = QStringLiteral("Poppler could not load the PDF from memory.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }
    if (doc->isLocked()) {
        const QString msg = QStringLiteral("Document is password-locked (not supported in this build path).");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    const int n = doc->numPages();
    if (n <= 0) {
        const QString msg = QStringLiteral("Document has no pages.");
        setLastError(msg);
        if (errorMessage) {
            *errorMessage = msg;
        }
        return false;
    }

    m_pageSizes.resize(n);
    for (int i = 0; i < n; ++i) {
        std::unique_ptr<Poppler::Page> page(doc->page(i));
        if (!page) {
            m_pageSizes[i] = QSizeF(612.0, 792.0);
            continue;
        }
        const QSizeF pagePts = page->pageSizeF();
        m_pageSizes[i] = pagePts;
    }

    m_document = doc.release();
    m_ownedBuffer = data;
    m_identityKey = QStringLiteral("buffer:%1").arg(qHash(data));
    m_lastError.clear();
    return true;
}

bool PopplerQtDocumentModel::supportsOpenBuffer() const {
    return true;
}

bool PopplerQtDocumentModel::isOpen() const {
    QMutexLocker locker(&m_mutex);
    return m_document != nullptr;
}

int PopplerQtDocumentModel::pageCount() const {
    QMutexLocker locker(&m_mutex);
    return m_document ? static_cast<Poppler::Document*>(m_document)->numPages() : 0;
}

QSizeF PopplerQtDocumentModel::pageSize(int pageIndex) const {
    QMutexLocker locker(&m_mutex);
    if (pageIndex < 0 || pageIndex >= m_pageSizes.size()) {
        return {};
    }
    return m_pageSizes.at(pageIndex);
}

QString PopplerQtDocumentModel::lastError() const {
    QMutexLocker locker(&m_mutex);
    return m_lastError;
}

QString PopplerQtDocumentModel::identityKey() const {
    QMutexLocker locker(&m_mutex);
    return m_identityKey;
}

QString PopplerQtDocumentModel::sourceLabel() const {
    QMutexLocker locker(&m_mutex);
    if (!m_document) {
        return {};
    }
    if (!m_absolutePath.isEmpty()) {
        return m_absolutePath;
    }
    return QStringLiteral("memory buffer (%1 bytes)").arg(m_ownedBuffer.size());
}

QByteArray PopplerQtDocumentModel::savedCopy() const {
    QMutexLocker locker(&m_mutex);
    return m_ownedBuffer;
}

QString PopplerQtDocumentModel::loadedFilePath() const {
    QMutexLocker locker(&m_mutex);
    return m_absolutePath;
}

} // namespace pdf_document_view

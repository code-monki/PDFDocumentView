/**
 * @file pdf_document_model.hpp
 * @brief Virtual document layer: open file or buffer, page geometry, errors, identity.
 *
 * @details Implementations are selected at compile time from the render backend
 * (PDFium vs Poppler stub or Poppler-Qt6). Hosts may use the model without the widget.
 * Semantics align with `PdfDocumentViewWidget` document APIs; see `docs/concept-pdf-document-view.md`.
 */

#pragma once

#include <QByteArray>
#include <QSizeF>
#include <QString>
#include <memory>

namespace pdf_document_view {

/**
 * @brief Document layer: open from path or memory, page count, and error surfacing.
 *
 * @details Frozen public surface for 0.1.x per `docs/adrs/0004-public-api-surface-freeze.md`.
 * Page sizes and rectangles use **top-down page space** in PDF user units (see
 * `docs/widget-coordinate-contract.md`).
 */
class PdfDocumentModel {
public:
    virtual ~PdfDocumentModel() = default;

    /**
     * @brief Open a PDF file from disk.
     * @param path Filesystem path to an existing PDF.
     * @param errorMessage If non-null, receives the same text as `lastError()` on failure.
     * @return True on success; on failure sets `lastError()` and optionally `*errorMessage`.
     */
    virtual bool openFile(const QString& path, QString* errorMessage = nullptr) = 0;

    /**
     * @brief Open a PDF from an in-memory byte array.
     * @param data Raw PDF bytes.
     * @param errorMessage Optional failure text out-parameter.
     * @return True on success.
     * @details PDFium: supported. Poppler stub: not implemented (returns false).
     */
    virtual bool openBuffer(const QByteArray& data, QString* errorMessage = nullptr) = 0;

    /**
     * @brief Whether this implementation can load from memory (`openBuffer`).
     * @details The widget intersects this with `PdfWidgetCapabilities::openFromBuffer` in `capabilities()`.
     */
    [[nodiscard]] virtual bool supportsOpenBuffer() const { return false; }

    /** @brief Close the document and release engine resources. */
    virtual void close() = 0;

    /** @brief True when a document is currently open. */
    [[nodiscard]] virtual bool isOpen() const = 0;

    /** @brief Number of pages after a successful open; zero when closed or invalid. */
    [[nodiscard]] virtual int pageCount() const = 0;

    /** @brief Page size in **top-down** PDF points (1/72 in) for @p pageIndex. */
    [[nodiscard]] virtual QSizeF pageSize(int pageIndex) const = 0;

    /**
     * @brief Last failure message from `openFile` / `openBuffer`.
     * @details Cleared on successful open. Thread affinity: same as the object (typically GUI).
     */
    [[nodiscard]] virtual QString lastError() const = 0;

    /**
     * @brief Stable cache key for the current document (e.g. absolute path or opaque buffer token).
     */
    [[nodiscard]] virtual QString identityKey() const = 0;

    /**
     * @brief Human-readable source label (path or short buffer summary).
     */
    [[nodiscard]] virtual QString sourceLabel() const = 0;
};

/**
 * @brief Factory for the document model matching the active CMake render backend.
 * @return A new model instance; never null.
 */
[[nodiscard]] std::unique_ptr<PdfDocumentModel> createDefaultPdfDocumentModel();

} // namespace pdf_document_view

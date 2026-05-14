#pragma once

#include "pdf_render_backend.hpp"

#include <QString>

namespace pdf_document_view {

class PopplerQtDocumentModel;

class PopplerQtBackend final : public PdfRenderBackend {
public:
    PopplerQtBackend();
    ~PopplerQtBackend() override;

    void attachDocumentModel(PopplerQtDocumentModel* model);

    [[nodiscard]] QString id() const override;
    [[nodiscard]] QString displayName() const override;
    [[nodiscard]] PdfWidgetCapabilities capabilities() const override;
    [[nodiscard]] int pageCount() const override;
    [[nodiscard]] QSizeF pageSize(int pageIndex) const override;
    void renderPage(const PageRenderRequest& request, QPainter& painter, const QRectF& targetRect) override;
    [[nodiscard]] bool renderPageTile(const PageRenderRequest& request,
        const QRect& tileRectInFullPageDeviceSpace,
        QImage* out) override;
    [[nodiscard]] bool renderPageTile(const QByteArray& detachedPdfBytes,
        const QString& detachedAbsolutePathIfBytesEmpty,
        const PageRenderRequest& request,
        const QRect& tileRectInFullPageDeviceSpace,
        QImage* out) override;
    void setDocumentPath(const QString& path) override;
    [[nodiscard]] int findTextMatches(const QString& needle, QVector<TextMatch>* out) const override;

private:
    PopplerQtDocumentModel* m_model = nullptr;
};

} // namespace pdf_document_view

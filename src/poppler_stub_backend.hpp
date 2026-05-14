#pragma once

#include "pdf_render_backend.hpp"

namespace pdf_document_view {

class PopplerStubBackend final : public PdfRenderBackend {
public:
    [[nodiscard]] QString id() const override;
    [[nodiscard]] QString displayName() const override;
    [[nodiscard]] PdfWidgetCapabilities capabilities() const override;
    [[nodiscard]] int pageCount() const override;
    [[nodiscard]] QSizeF pageSize(int pageIndex) const override;
    void renderPage(const PageRenderRequest& request, QPainter& painter, const QRectF& targetRect) override;
    void setDocumentPath(const QString& path) override;
    [[nodiscard]] int findTextMatches(const QString& needle, QVector<TextMatch>* out) const override;
};

} // namespace pdf_document_view

/**
 * @file poppler_stub_backend.cpp
 * @brief Placeholder Poppler backend when `PDFDOCUMENTVIEW_POPPLER_REAL` is off (no engine link).
 */

#include "poppler_stub_backend.hpp"

#include <pdf_document_view/pdf_capabilities.hpp>

#include <QColor>
#include <QDebug>
#include <QPainter>

namespace pdf_document_view {

QString PopplerStubBackend::id() const {
    return QStringLiteral("poppler");
}

QString PopplerStubBackend::displayName() const {
    return QStringLiteral("Poppler (stub)");
}

PdfWidgetCapabilities PopplerStubBackend::capabilities() const {
    return PdfWidgetCapabilities{.textSearch = false,
        .renderPages = true,
        .openFromBuffer = false,
        .tileRendering = false,
        .asyncTileRendering = false};
}

int PopplerStubBackend::pageCount() const {
    return 1;
}

QSizeF PopplerStubBackend::pageSize(int /*pageIndex*/) const {
    return QSizeF(612.0, 792.0);
}

void PopplerStubBackend::renderPage(const PageRenderRequest& request, QPainter& painter, const QRectF& targetRect) {
    (void)request.pageIndex;
    (void)request.devicePixelRatio;
    (void)request.devicePixelSize;
    painter.fillRect(targetRect, QColor(230, 255, 220));
    painter.setPen(QColor(30, 100, 45));
    painter.drawText(targetRect, Qt::AlignCenter, id());
}

void PopplerStubBackend::setDocumentPath(const QString& path) {
    if (!path.trimmed().isEmpty()) {
        qWarning() << "PopplerStubBackend: Poppler engine is not linked; ignoring document path.";
    }
}

int PopplerStubBackend::findTextMatches(const QString& needle, QVector<TextMatch>* out) const {
    if (out) {
        out->clear();
    }
    if (!needle.trimmed().isEmpty()) {
        qWarning() << "PopplerStubBackend: text search is not implemented for the stub backend.";
    }
    return 0;
}

} // namespace pdf_document_view

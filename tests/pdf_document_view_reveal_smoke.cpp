#include <pdf_document_view/page_highlight.hpp>
#include <pdf_document_view/pdf_document_model.hpp>
#include <pdf_document_view/pdf_document_view_widget.hpp>

#include <QApplication>
#include <QDir>
#include <QRectF>
#include <QScrollBar>
#include <QString>

#include <cstdlib>

#ifndef FIXTURE_PDF_PATH
#error "pdf_document_view_reveal_smoke requires FIXTURE_PDF_PATH from CMake"
#endif

namespace {

QString resolveFixturePath(int argc, char** argv) {
    if (argc > 1 && argv[1] != nullptr && argv[1][0] != '\0') {
        return QDir::cleanPath(QString::fromLocal8Bit(argv[1]));
    }
    return QDir::cleanPath(QString::fromUtf8(FIXTURE_PDF_PATH));
}

} // namespace

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    const QString path = resolveFixturePath(argc, argv);
    pdf_document_view::PdfDocumentViewWidget w;
    w.resize(200, 200);
    w.setDocumentPath(path);
    if (!w.isDocumentOpen()) {
        return 1;
    }
    w.setZoom(2.0);

    auto* model = w.documentModel();
    if (!model || model->pageCount() < 1) {
        return 2;
    }
    const QSizeF pts = model->pageSize(0);
    if (pts.height() < 80.0 || pts.width() < 40.0) {
        return 3;
    }

    QScrollBar* v = w.verticalScrollBar();
    const int y0 = v->value();
    const QRectF nearBottom(10.0, pts.height() - 100.0, 60.0, 40.0);
    w.revealPageRect(0, nearBottom);
    const int y1 = v->value();
    if (y1 <= y0) {
        return 4;
    }

    pdf_document_view::PageHighlight keep;
    keep.pageIndex = 0;
    keep.pageRect = QRectF(5.0, 5.0, 20.0, 15.0);
    keep.fill = QColor(255, 0, 0, 80);
    keep.border = QColor(0, 0, 255);
    w.setPageHighlights({keep});
    w.navigateToHostSearchHit(0, nearBottom, false);
    if (w.pageHighlights().size() != 1 || w.pageHighlights().at(0).pageRect != keep.pageRect) {
        return 5;
    }

    w.navigateToHostSearchHit(0, nearBottom, true);
    if (w.pageHighlights().size() != 1 || w.pageHighlights().at(0).pageRect != nearBottom) {
        return 6;
    }

    w.setDocumentPath(QString());
    const QRectF hostHit(12.0, 20.0, 50.0, 35.0);
    w.openDocumentAndRevealSearchHit(path, QStringLiteral("xyzzy"), 0, hostHit);
    if (!w.isDocumentOpen()) {
        return 7;
    }
    if (w.currentPageIndex() != 0) {
        return 8;
    }
    if (w.pageHighlights().size() != 1 || w.pageHighlights().at(0).pageRect != hostHit) {
        return 9;
    }

    return EXIT_SUCCESS;
}

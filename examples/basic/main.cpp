#include "pdf_document_view/pdf_document_view_widget.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    pdf_document_view::PdfDocumentViewWidget w;
    w.setWindowTitle(QObject::tr("PDFDocumentView — basic integration demo"));
    w.resize(640, 480);
    w.show();
    return app.exec();
}

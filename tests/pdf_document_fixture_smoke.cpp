#include <pdf_document_view/pdf_document_model.hpp>

#include <QCoreApplication>
#include <QDir>
#include <QString>

#include <cstdlib>

#ifndef FIXTURE_PDF_PATH
#error "pdf_document_fixture_smoke requires FIXTURE_PDF_PATH from CMake"
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
    QCoreApplication app(argc, argv);

    auto model = pdf_document_view::createDefaultPdfDocumentModel();
    if (!model) {
        return 3;
    }

    const QString path = resolveFixturePath(argc, argv);
    QString err;
    const bool ok = model->openFile(path, &err);
    if (!ok) {
        return 1;
    }
    if (model->pageCount() < 1) {
        return 2;
    }
    if (!model->lastError().isEmpty()) {
        return 4;
    }

    return EXIT_SUCCESS;
}

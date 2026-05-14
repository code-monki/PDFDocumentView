#include <pdf_document_view/pdf_document_model.hpp>

#include <QByteArray>
#include <QCoreApplication>
#include <QString>

#include <cstdlib>

namespace {

bool expectOpenBufferFails(pdf_document_view::PdfDocumentModel& model, const QByteArray& data,
    const char* contextTag) {
    QString err;
    const bool ok = model.openBuffer(data, &err);
    if (ok) {
        qWarning("%s: expected openBuffer failure, got success", contextTag);
        return false;
    }
    if (err.trimmed().isEmpty()) {
        qWarning("%s: expected non-empty error message", contextTag);
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    auto model = pdf_document_view::createDefaultPdfDocumentModel();
    if (!model) {
        return 3;
    }

    if (!expectOpenBufferFails(*model, QByteArray(), "empty_buffer")) {
        return 1;
    }

    if (!expectOpenBufferFails(*model, QByteArray("notpdf"), "invalid_buffer")) {
        return 2;
    }

    return EXIT_SUCCESS;
}

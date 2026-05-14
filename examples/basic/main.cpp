#include "pdf_document_view/pdf_document_view_widget.hpp"

#include <pdf_document_view/page_highlight.hpp>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>
#include <QRectF>
#include <QString>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QString resolveDemoPdfPath(int argc, char* argv[]) {
    if (argc > 1) {
        const QFileInfo fi(QString::fromLocal8Bit(argv[1]));
        if (fi.exists()) {
            return fi.absoluteFilePath();
        }
        qWarning() << "PDF path does not exist (trying defaults):" << argv[1];
    }
#ifdef PDFDOCUMENTVIEW_EXAMPLE_DEFAULT_PDF
    {
        const QFileInfo fi(QStringLiteral(PDFDOCUMENTVIEW_EXAMPLE_DEFAULT_PDF));
        if (fi.exists()) {
            return fi.absoluteFilePath();
        }
    }
#endif
    const QFileInfo def(QStringLiteral("gnu-c-language-manual.pdf"));
    if (def.exists()) {
        return def.absoluteFilePath();
    }
    return {};
}

QString pageStatusLine(const pdf_document_view::PdfDocumentViewWidget& w) {
    QString pagePart;
    const int n = w.pageCount();
    if (n <= 0) {
        pagePart = QObject::tr("No pages (empty or not loaded)");
    } else {
        pagePart = QObject::tr("Page %1 of %2 — menus, keys, or Ctrl+wheel to zoom")
            .arg(w.currentPageIndex() + 1)
            .arg(n);
    }
    const QString err = w.documentError().trimmed();
    if (!err.isEmpty()) {
        return pagePart + QLatin1Char('\n') + err;
    }
    return pagePart;
}

void updateWindowTitle(QWidget& shell,
    const pdf_document_view::PdfDocumentViewWidget& w,
    const QString& pdfPath) {
    const QString shownName = pdfPath.isEmpty() ? w.documentSourceLabel() : QFileInfo(pdfPath).fileName();
    const QString windowTitle =
        QObject::tr("PDFDocumentView — basic demo (%1) — %2")
            .arg(w.backendId(),
                 shownName.isEmpty() ? QObject::tr("(no document)") : shownName);
    shell.setWindowTitle(windowTitle);
}

} // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget shell;
    pdf_document_view::PdfDocumentViewWidget w(&shell);
    w.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const QString pdfPath = resolveDemoPdfPath(argc, argv);
    if (!pdfPath.isEmpty()) {
        w.setDocumentPath(pdfPath);
    }

    auto* status = new QLabel(pageStatusLine(w), &shell);
    status->setWordWrap(true);
    status->setFocusPolicy(Qt::NoFocus);
    status->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    auto* menuBar = new QMenuBar(&shell);
    // macOS defaults to a system-global menu bar; keep menus in-window for this basic demo.
    menuBar->setNativeMenuBar(false);
    QMenu* fileMenu = menuBar->addMenu(QObject::tr("&File"));
    QMenu* navigateMenu = menuBar->addMenu(QObject::tr("&Navigate"));
    QMenu* viewMenu = menuBar->addMenu(QObject::tr("&View"));
    QMenu* searchMenu = menuBar->addMenu(QObject::tr("&Search"));

    auto* actOpen = new QAction(QObject::tr("&Open…"), &shell);
    actOpen->setShortcut(QKeySequence::Open);

    auto* actCloseDoc = new QAction(QObject::tr("&Close document"), &shell);
    actCloseDoc->setShortcut(QKeySequence::Close);

    auto* actQuit = new QAction(QObject::tr("&Quit"), &shell);
    actQuit->setShortcut(QKeySequence::Quit);

    auto* actNextPage = new QAction(QObject::tr("&Next page"), &shell);
    actNextPage->setShortcut(QKeySequence::MoveToNextPage);

    auto* actPrevPage = new QAction(QObject::tr("&Previous page"), &shell);
    actPrevPage->setShortcut(QKeySequence::MoveToPreviousPage);

    auto* actFirstPage = new QAction(QObject::tr("&First page"), &shell);
    actFirstPage->setShortcut(QKeySequence::MoveToStartOfDocument);

    auto* actLastPage = new QAction(QObject::tr("&Last page"), &shell);
    actLastPage->setShortcut(QKeySequence::MoveToEndOfDocument);

    auto* actZoomIn = new QAction(QObject::tr("Zoom &in"), &shell);
    actZoomIn->setShortcut(QKeySequence::ZoomIn);

    auto* actZoomOut = new QAction(QObject::tr("Zoom &out"), &shell);
    actZoomOut->setShortcut(QKeySequence::ZoomOut);

    auto* actResetZoom = new QAction(QObject::tr("&Reset zoom"), &shell);

    auto* actFitWidth = new QAction(QObject::tr("&Fit width"), &shell);

    auto* actTileRender = new QAction(QObject::tr("Tile-based page &rendering"), &shell);
    actTileRender->setCheckable(true);
    actTileRender->setChecked(w.tileRenderingEnabled());

    auto* actAsyncTileRender = new QAction(QObject::tr("&Async tile rendering"), &shell);
    actAsyncTileRender->setCheckable(true);
    actAsyncTileRender->setChecked(w.asyncTileRenderingEnabled());

    auto* actDemoOverlay = new QAction(QObject::tr("Demo &overlay (page 1)"), &shell);
    actDemoOverlay->setCheckable(true);

    auto* actFind = new QAction(QObject::tr("&Find…"), &shell);
    actFind->setShortcut(QKeySequence::Find);

    auto* actFindNext = new QAction(QObject::tr("Find &next"), &shell);
    actFindNext->setShortcut(QKeySequence::FindNext);

    auto* actFindPrev = new QAction(QObject::tr("Find &previous"), &shell);
    actFindPrev->setShortcut(QKeySequence::FindPrevious);

    auto* actClearHighlights = new QAction(QObject::tr("&Clear highlights"), &shell);

    for (QAction* a : {actOpen,
             actCloseDoc,
             actQuit,
             actNextPage,
             actPrevPage,
             actFirstPage,
             actLastPage,
             actZoomIn,
             actZoomOut,
             actResetZoom,
             actFitWidth,
             actTileRender,
             actAsyncTileRender,
             actDemoOverlay,
             actFind,
             actFindNext,
             actFindPrev,
             actClearHighlights}) {
        // Application-wide so shortcuts fire even when the PDF viewport has focus.
        a->setShortcutContext(Qt::ApplicationShortcut);
        shell.addAction(a);
    }

    fileMenu->addAction(actOpen);
    fileMenu->addAction(actCloseDoc);
    fileMenu->addSeparator();
    fileMenu->addAction(actQuit);

    navigateMenu->addAction(actNextPage);
    navigateMenu->addAction(actPrevPage);
    navigateMenu->addSeparator();
    navigateMenu->addAction(actFirstPage);
    navigateMenu->addAction(actLastPage);

    viewMenu->addAction(actZoomIn);
    viewMenu->addAction(actZoomOut);
    viewMenu->addSeparator();
    viewMenu->addAction(actResetZoom);
    viewMenu->addAction(actFitWidth);
    viewMenu->addSeparator();
    viewMenu->addAction(actTileRender);
    viewMenu->addAction(actAsyncTileRender);
    viewMenu->addSeparator();
    viewMenu->addAction(actDemoOverlay);

    searchMenu->addAction(actFind);
    searchMenu->addAction(actFindNext);
    searchMenu->addAction(actFindPrev);
    searchMenu->addSeparator();
    searchMenu->addAction(actClearHighlights);

    QObject::connect(actOpen, &QAction::triggered, &shell, [&]() {
        const QString startDir =
            w.documentPath().isEmpty() ? QString() : QFileInfo(w.documentPath()).absolutePath();
        const QString path = QFileDialog::getOpenFileName(&shell,
            QObject::tr("Open PDF"),
            startDir,
            QObject::tr("PDF files (*.pdf);;All files (*)"));
        if (path.isEmpty()) {
            return;
        }
        w.setDocumentPath(path);
        updateWindowTitle(shell, w, w.documentPath());
    });

    QObject::connect(actCloseDoc, &QAction::triggered, &shell, [&]() {
        w.setDocumentPath(QString());
        updateWindowTitle(shell, w, QString());
    });

    QObject::connect(actQuit, &QAction::triggered, &app, &QApplication::quit);

    QObject::connect(actNextPage, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::nextPage);
    QObject::connect(actPrevPage, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::previousPage);
    QObject::connect(actFirstPage, &QAction::triggered, &w, [&]() { w.setCurrentPageIndex(0); });
    QObject::connect(actLastPage, &QAction::triggered, &w, [&]() {
        const int n = w.pageCount();
        if (n > 0) {
            w.setCurrentPageIndex(n - 1);
        }
    });

    QObject::connect(actZoomIn, &QAction::triggered, &w, [&]() { w.setZoom(w.zoom() * 1.2); });
    QObject::connect(actZoomOut, &QAction::triggered, &w, [&]() { w.setZoom(w.zoom() / 1.2); });
    QObject::connect(actResetZoom, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::resetZoom);
    QObject::connect(actFitWidth, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::fitWidth);

    QObject::connect(actTileRender, &QAction::toggled, &w, &pdf_document_view::PdfDocumentViewWidget::setTileRenderingEnabled);

    QObject::connect(actAsyncTileRender, &QAction::toggled, &w, &pdf_document_view::PdfDocumentViewWidget::setAsyncTileRenderingEnabled);

    QObject::connect(actDemoOverlay, &QAction::toggled, &w, [&](bool on) {
        if (!on) {
            w.clearPageHighlights();
            return;
        }
        const int n = w.pageCount();
        if (n <= 0) {
            return;
        }
        const int pageIdx = (n >= 2) ? 1 : 0;
        pdf_document_view::PageHighlight ph;
        ph.pageIndex = pageIdx;
        ph.pageRect = QRectF(72, 144, 360, 180);
        ph.fill = QColor(0, 120, 215, 60);
        ph.border = QColor(0, 90, 160, 220);
        w.setPageHighlights({ph});
    });

    QObject::connect(actFind, &QAction::triggered, &shell, [&]() {
        const bool hasDoc = w.isDocumentOpen();
        if (!hasDoc) {
            return;
        }
        const QString needle =
            QInputDialog::getText(&shell, QObject::tr("Find"), QObject::tr("Search for:"));
        w.findText(needle);
    });

    QObject::connect(actFindNext, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::findNext);
    QObject::connect(
        actFindPrev, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::findPrevious);
    QObject::connect(actClearHighlights, &QAction::triggered, &w, &pdf_document_view::PdfDocumentViewWidget::clearFind);

    const auto refreshActionStates = [&]() {
        const bool hasDoc = w.isDocumentOpen();
        const int n = w.pageCount();
        const bool hasPages = hasDoc && n > 0;
        const int cur = w.currentPageIndex();

        actCloseDoc->setEnabled(hasDoc);
        actNextPage->setEnabled(hasPages && cur < n - 1);
        actPrevPage->setEnabled(hasPages && cur > 0);
        actFirstPage->setEnabled(hasPages && cur > 0);
        actLastPage->setEnabled(hasPages && cur < n - 1);

        actZoomIn->setEnabled(hasPages);
        actZoomOut->setEnabled(hasPages);
        actResetZoom->setEnabled(hasPages);
        actFitWidth->setEnabled(hasPages);

        const bool tileCap = w.capabilities().tileRendering;
        actTileRender->setEnabled(hasPages && tileCap);
        actTileRender->setChecked(tileCap && w.tileRenderingEnabled());

        actAsyncTileRender->setEnabled(hasPages && w.asyncTileRenderingSupported());
        actAsyncTileRender->setChecked(w.asyncTileRenderingEnabled());

        actDemoOverlay->setEnabled(hasPages);
        if (!hasPages) {
            if (actDemoOverlay->isChecked()) {
                actDemoOverlay->blockSignals(true);
                actDemoOverlay->setChecked(false);
                actDemoOverlay->blockSignals(false);
            }
            w.clearPageHighlights();
        }

        actFind->setEnabled(hasDoc);
        const int matches = w.findMatchCount();
        actFindNext->setEnabled(matches > 0);
        actFindPrev->setEnabled(matches > 0);
        actClearHighlights->setEnabled(matches > 0);
    };

    QObject::connect(&w, &pdf_document_view::PdfDocumentViewWidget::viewStateChanged, &shell, [&]() {
        status->setText(pageStatusLine(w));
        updateWindowTitle(shell, w, w.documentPath());
        refreshActionStates();
    });
    QObject::connect(
        &w, &pdf_document_view::PdfDocumentViewWidget::findResultsChanged, &shell, [&](int, int) {
            refreshActionStates();
        });

    // PdfDocumentViewWidget fills the viewport; menus (above) and status (below) demonstrate the
    // public widget API. No second raster surface — see AGENTS.md "Demo vs Product".
    auto* mainLayout = new QVBoxLayout(&shell);
    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(&w, /*stretch*/ 1);
    mainLayout->addWidget(status, /*stretch*/ 0);

    updateWindowTitle(shell, w, pdfPath);
    shell.resize(640, 480);

    refreshActionStates();

    shell.show();
    shell.activateWindow();
    w.setFocus(Qt::OtherFocusReason);
    return app.exec();
}

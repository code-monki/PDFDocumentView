/**
 * @file pdfium_backend.cpp
 * @brief PDFium render backend: full-page and tile raster, text search, mutex around document access.
 */

#include "pdfium_backend.hpp"

#include "pdfium_document_model.hpp"
#include "pdfium_library.hpp"
#include "pdfium_tile_raster.hpp"

#include <pdf_document_view/pdf_capabilities.hpp>

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QMutexLocker>
#include <QtGlobal>

#include <cmath>

extern "C" {
#include "fpdfview.h"
#include "fpdf_text.h"
}

namespace pdf_document_view {
namespace {

constexpr int kMaxFindMatches = 2000;

[[nodiscard]] unsigned pdfiumFillArgb(const QColor& c) {
    return static_cast<unsigned>((qBound(0, c.alpha(), 255) << 24) | (qBound(0, c.red(), 255) << 16)
        | (qBound(0, c.green(), 255) << 8) | static_cast<unsigned>(qBound(0, c.blue(), 255)));
}

/// PDF user space is Y-up; widget / `pageSize()` use Y-down from top of page (points).
QRectF pdfUserRectToPageTopDown(double left,
    double top,
    double right,
    double bottom,
    double pageHeightPts) {
    const double qtLeft = left;
    const double qtRight = right;
    const double qtTop = pageHeightPts - top;
    const double qtBottom = pageHeightPts - bottom;
    return QRectF(qtLeft, qtTop, qtRight - qtLeft, qtBottom - qtTop).normalized();
}

QImage bitmapToImage(FPDF_BITMAP bitmap, int bw, int bh) {
    const int stride = FPDFBitmap_GetStride(bitmap);
    void* buffer = FPDFBitmap_GetBuffer(bitmap);
    if (!buffer || stride <= 0 || bw <= 0 || bh <= 0) {
        return {};
    }

    QImage view(static_cast<const uchar*>(buffer), bw, bh, stride, QImage::Format_ARGB32);
    QImage owned = view.copy();
    owned = owned.convertToFormat(QImage::Format_RGB32);
    return owned;
}

/// Solve 3×3 linear system (Gauss–Jordan). @p rows[i][j] for j<3 are coefficients, rows[i][3] is RHS.
[[nodiscard]] bool solveLinear3(double rows[3][4], double out[3]) {
    for (int col = 0; col < 3; ++col) {
        int pivot = col;
        double best = std::fabs(rows[col][col]);
        for (int r = col + 1; r < 3; ++r) {
            const double v = std::fabs(rows[r][col]);
            if (v > best) {
                best = v;
                pivot = r;
            }
        }
        if (best < 1e-9) {
            return false;
        }
        if (pivot != col) {
            for (int k = 0; k < 4; ++k) {
                std::swap(rows[col][k], rows[pivot][k]);
            }
        }
        const double div = rows[col][col];
        for (int k = col; k < 4; ++k) {
            rows[col][k] /= div;
        }
        for (int r = 0; r < 3; ++r) {
            if (r == col) {
                continue;
            }
            const double f = rows[r][col];
            if (f == 0.0) {
                continue;
            }
            for (int k = col; k < 4; ++k) {
                rows[r][k] -= f * rows[col][k];
            }
        }
    }
    out[0] = rows[0][3];
    out[1] = rows[1][3];
    out[2] = rows[2][3];
    return true;
}

/// Page (PDF user) → full-page device pixels (same convention as `FPDF_RenderPageBitmap(..., 0,0,iw,ih,0,...)`).
[[nodiscard]] bool pageToDeviceMatrix(FPDF_PAGE page, int iw, int ih, FS_MATRIX* outMatrix) {
    if (!page || !outMatrix || iw <= 0 || ih <= 0) {
        return false;
    }
    const float pw = FPDF_GetPageWidthF(page);
    const float ph = FPDF_GetPageHeightF(page);
    if (pw <= 0.0f || ph <= 0.0f) {
        return false;
    }

    auto pageToDevInt = [&](double px, double py, double* odx, double* ody) -> bool {
        int ix = 0;
        int iy = 0;
        if (!FPDF_PageToDevice(page, 0, 0, iw, ih, 0, px, py, &ix, &iy)) {
            return false;
        }
        *odx = static_cast<double>(ix);
        *ody = static_cast<double>(iy);
        return true;
    };

    double x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    if (!pageToDevInt(0.0, 0.0, &x0, &y0) || !pageToDevInt(static_cast<double>(pw), 0.0, &x1, &y1)
        || !pageToDevInt(0.0, static_cast<double>(ph), &x2, &y2)) {
        return false;
    }

    double rx[3][4] = {{0.0, 0.0, 1.0, x0},
        {static_cast<double>(pw), 0.0, 1.0, x1},
        {0.0, static_cast<double>(ph), 1.0, x2}};
    double ry[3][4] = {{0.0, 0.0, 1.0, y0},
        {static_cast<double>(pw), 0.0, 1.0, y1},
        {0.0, static_cast<double>(ph), 1.0, y2}};

    double ax[3], ay[3];
    if (!solveLinear3(rx, ax) || !solveLinear3(ry, ay)) {
        return false;
    }

    outMatrix->a = static_cast<float>(ax[0]);
    outMatrix->c = static_cast<float>(ax[1]);
    outMatrix->e = static_cast<float>(ax[2]);
    outMatrix->b = static_cast<float>(ay[0]);
    outMatrix->d = static_cast<float>(ay[1]);
    outMatrix->f = static_cast<float>(ay[2]);
    return true;
}

} // namespace

PdfiumBackend::PdfiumBackend() {
    pdfium_library::ensureLibrary();
}

PdfiumBackend::~PdfiumBackend() {
    attachDocumentModel(nullptr);
    pdfium_library::releaseLibrary();
}

void PdfiumBackend::attachDocumentModel(PdfiumDocumentModel* model) {
    m_model = model;
}

QString PdfiumBackend::id() const {
    return QStringLiteral("pdfium");
}

QString PdfiumBackend::displayName() const {
    return QStringLiteral("PDFium");
}

PdfWidgetCapabilities PdfiumBackend::capabilities() const {
    return PdfWidgetCapabilities{.textSearch = true,
        .renderPages = true,
        .openFromBuffer = true,
        .tileRendering = true,
        .asyncTileRendering = true};
}

void PdfiumBackend::setDocumentPath(const QString& path) {
    (void)path;
    // Document lifetime is owned by `PdfDocumentModel`; hosts use `PdfDocumentViewWidget::setDocumentPath` /
    // `openDocumentBuffer` which open the model.
}

int PdfiumBackend::pageCount() const {
    return m_model ? m_model->pageCount() : 0;
}

QSizeF PdfiumBackend::pageSize(int pageIndex) const {
    return m_model ? m_model->pageSize(pageIndex) : QSizeF{};
}

void PdfiumBackend::renderPage(const PageRenderRequest& request, QPainter& painter, const QRectF& targetRect) {
    (void)request.devicePixelRatio;

    if (!m_model) {
        painter.fillRect(targetRect, QColor(235, 235, 235));
        painter.setPen(QColor(90, 90, 90));
        painter.drawText(targetRect, Qt::AlignCenter, QStringLiteral("No document"));
        return;
    }

    QMutexLocker locker(&m_model->m_mutex);

    const int pageIndex = request.pageIndex;
    if (!m_model->m_doc || pageIndex < 0 || pageIndex >= m_model->m_pageSizes.size()) {
        painter.fillRect(targetRect, QColor(235, 235, 235));
        painter.setPen(QColor(90, 90, 90));
        painter.drawText(targetRect, Qt::AlignCenter, QStringLiteral("No document"));
        return;
    }

    auto* const doc = reinterpret_cast<FPDF_DOCUMENT>(m_model->m_doc);
    FPDF_PAGE page = FPDF_LoadPage(doc, pageIndex);
    if (!page) {
        painter.fillRect(targetRect, QColor(255, 220, 220));
        painter.setPen(Qt::darkRed);
        painter.drawText(targetRect, Qt::AlignCenter, QStringLiteral("Page load failed"));
        return;
    }

    const int bw = qMax(1, request.devicePixelSize.width());
    const int bh = qMax(1, request.devicePixelSize.height());

    FPDF_BITMAP bitmap = FPDFBitmap_Create(bw, bh, 0);
    if (!bitmap) {
        FPDF_ClosePage(page);
        painter.fillRect(targetRect, QColor(255, 200, 200));
        return;
    }

    FPDFBitmap_FillRect(bitmap, 0, 0, bw, bh, pdfiumFillArgb(request.paperColor));
    FPDF_RenderPageBitmap(bitmap, page, 0, 0, bw, bh, 0, FPDF_ANNOT);
    FPDF_ClosePage(page);

    QImage owned = bitmapToImage(bitmap, bw, bh);
    FPDFBitmap_Destroy(bitmap);

    painter.save();
    painter.drawImage(targetRect, owned);
    painter.restore();
}

bool PdfiumBackend::renderPageTile(const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out) {
    if (!out) {
        return false;
    }
    *out = QImage();

    if (!m_model) {
        return false;
    }

    QMutexLocker locker(&m_model->m_mutex);

    const int pageIndex = request.pageIndex;
    if (!m_model->m_doc || pageIndex < 0 || pageIndex >= m_model->m_pageSizes.size()) {
        return false;
    }

    const int iw = qMax(1, request.devicePixelSize.width());
    const int ih = qMax(1, request.devicePixelSize.height());
    const QRect tr = tileRectInFullPageDeviceSpace.intersected(QRect(0, 0, iw, ih));
    if (!tr.isValid() || tr.isEmpty()) {
        return false;
    }

    const int tw = tr.width();
    const int th = tr.height();

    auto* const doc = reinterpret_cast<FPDF_DOCUMENT>(m_model->m_doc);
    FPDF_PAGE page = FPDF_LoadPage(doc, pageIndex);
    if (!page) {
        return false;
    }

    FS_MATRIX matrix{};
    if (!pageToDeviceMatrix(page, iw, ih, &matrix)) {
        FPDF_ClosePage(page);
        return false;
    }

    matrix.e -= static_cast<float>(tr.x());
    matrix.f -= static_cast<float>(tr.y());

    FPDF_BITMAP bitmap = FPDFBitmap_Create(tw, th, 0);
    if (!bitmap) {
        FPDF_ClosePage(page);
        return false;
    }

    FPDFBitmap_FillRect(bitmap, 0, 0, tw, th, pdfiumFillArgb(request.paperColor));

    FS_RECTF clip{};
    clip.left = 0.0f;
    clip.top = 0.0f;
    clip.right = static_cast<float>(tw);
    clip.bottom = static_cast<float>(th);

    // Per-tile device bitmap + `FPDF_RenderPageBitmapWithMatrix` (no MVP-1 full-page intermediate).
    FPDF_RenderPageBitmapWithMatrix(bitmap, page, &matrix, &clip, FPDF_ANNOT);
    FPDF_ClosePage(page);

    QImage tile = bitmapToImage(bitmap, tw, th);
    FPDFBitmap_Destroy(bitmap);
    if (tile.isNull()) {
        return false;
    }
    *out = std::move(tile);
    return true;
}

bool PdfiumBackend::renderPageTile(const QByteArray& detachedPdfBytes,
    const QString& detachedAbsolutePathIfBytesEmpty,
    const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out) {
    return pdfiumRenderPageTileIsolated(
        detachedPdfBytes, detachedAbsolutePathIfBytesEmpty, request, tileRectInFullPageDeviceSpace, out);
}

int PdfiumBackend::findTextMatches(const QString& needle, QVector<TextMatch>* out) const {
    if (needle.isEmpty()) {
        if (out) {
            out->clear();
        }
        return 0;
    }

    if (!m_model) {
        if (out) {
            out->clear();
        }
        return 0;
    }

    QMutexLocker locker(&m_model->m_mutex);

    if (!m_model->m_doc) {
        if (out) {
            out->clear();
        }
        return 0;
    }

    QVector<unsigned short> wide(static_cast<int>(needle.size()) + 1);
    for (int i = 0; i < needle.size(); ++i) {
        wide[i] = static_cast<unsigned short>(needle.at(i).unicode());
    }
    wide[needle.size()] = 0;

    auto* const doc = reinterpret_cast<FPDF_DOCUMENT>(m_model->m_doc);
    const int n = m_model->m_pageSizes.size();
    int total = 0;

    if (out) {
        out->clear();
    }

    for (int pi = 0; pi < n && total < kMaxFindMatches; ++pi) {
        const double pageH = m_model->m_pageSizes.at(pi).height();
        FPDF_PAGE page = FPDF_LoadPage(doc, pi);
        if (!page) {
            continue;
        }
        FPDF_TEXTPAGE textPage = FPDFText_LoadPage(page);
        if (!textPage) {
            FPDF_ClosePage(page);
            continue;
        }

        FPDF_SCHHANDLE sch = FPDFText_FindStart(textPage,
            reinterpret_cast<FPDF_WIDESTRING>(wide.data()),
            0,
            0);
        if (sch) {
            while (FPDFText_FindNext(sch) && total < kMaxFindMatches) {
                const int startIdx = FPDFText_GetSchResultIndex(sch);
                const int matchLen = FPDFText_GetSchCount(sch);
                if (matchLen <= 0) {
                    continue;
                }

                const int rectCount = FPDFText_CountRects(textPage, startIdx, matchLen);
                if (rectCount <= 0) {
                    continue;
                }

                double unionLeft = 0.0;
                double unionTop = 0.0;
                double unionRight = 0.0;
                double unionBottom = 0.0;
                bool haveUnion = false;

                for (int ri = 0; ri < rectCount; ++ri) {
                    double rl = 0.0;
                    double rt = 0.0;
                    double rr = 0.0;
                    double rb = 0.0;
                    if (!FPDFText_GetRect(textPage, ri, &rl, &rt, &rr, &rb)) {
                        continue;
                    }
                    if (!haveUnion) {
                        unionLeft = rl;
                        unionTop = rt;
                        unionRight = rr;
                        unionBottom = rb;
                        haveUnion = true;
                    } else {
                        unionLeft = qMin(unionLeft, rl);
                        unionTop = qMax(unionTop, rt);
                        unionRight = qMax(unionRight, rr);
                        unionBottom = qMin(unionBottom, rb);
                    }
                }

                if (haveUnion) {
                    ++total;
                    if (out) {
                        out->push_back(
                            TextMatch{pi, pdfUserRectToPageTopDown(unionLeft, unionTop, unionRight, unionBottom, pageH)});
                    }
                }
            }
            FPDFText_FindClose(sch);
        }

        FPDFText_ClosePage(textPage);
        FPDF_ClosePage(page);
    }

    return total;
}

} // namespace pdf_document_view

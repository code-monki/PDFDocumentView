/**
 * @file pdfium_tile_raster.cpp
 * @brief Rasterizes a PDFium page region into a device-pixel tile bitmap.
 */

#include "pdfium_tile_raster.hpp"

#include <QColor>
#include <QFile>

#include <cmath>

extern "C" {
#include "fpdfview.h"
}

namespace pdf_document_view {
namespace {

[[nodiscard]] unsigned pdfiumFillArgb(const QColor& c) {
    return static_cast<unsigned>((qBound(0, c.alpha(), 255) << 24) | (qBound(0, c.red(), 255) << 16)
        | (qBound(0, c.green(), 255) << 8) | static_cast<unsigned>(qBound(0, c.blue(), 255)));
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

[[nodiscard]] FPDF_DOCUMENT loadIsolatedDocument(const QByteArray& pdfBytes, const QString& pathIfEmpty) {
    if (!pdfBytes.isEmpty()) {
        return FPDF_LoadMemDocument64(pdfBytes.constData(), static_cast<size_t>(pdfBytes.size()), nullptr);
    }
    if (!pathIfEmpty.isEmpty()) {
        const QByteArray encoded = QFile::encodeName(pathIfEmpty);
        return FPDF_LoadDocument(encoded.constData(), nullptr);
    }
    return nullptr;
}

} // namespace

bool pdfiumRenderPageTileIsolated(const QByteArray& pdfBytes,
    const QString& absolutePathIfBytesEmpty,
    const PageRenderRequest& request,
    const QRect& tileRectInFullPageDeviceSpace,
    QImage* out) {
    if (!out) {
        return false;
    }
    *out = QImage();

    FPDF_DOCUMENT doc = loadIsolatedDocument(pdfBytes, absolutePathIfBytesEmpty);
    if (!doc) {
        return false;
    }

    const int pageIndex = request.pageIndex;
    const int n = FPDF_GetPageCount(doc);
    if (pageIndex < 0 || pageIndex >= n) {
        FPDF_CloseDocument(doc);
        return false;
    }

    const int iw = qMax(1, request.devicePixelSize.width());
    const int ih = qMax(1, request.devicePixelSize.height());
    const QRect tr = tileRectInFullPageDeviceSpace.intersected(QRect(0, 0, iw, ih));
    if (!tr.isValid() || tr.isEmpty()) {
        FPDF_CloseDocument(doc);
        return false;
    }

    const int tw = tr.width();
    const int th = tr.height();

    FPDF_PAGE page = FPDF_LoadPage(doc, pageIndex);
    if (!page) {
        FPDF_CloseDocument(doc);
        return false;
    }

    FS_MATRIX matrix{};
    if (!pageToDeviceMatrix(page, iw, ih, &matrix)) {
        FPDF_ClosePage(page);
        FPDF_CloseDocument(doc);
        return false;
    }

    matrix.e -= static_cast<float>(tr.x());
    matrix.f -= static_cast<float>(tr.y());

    FPDF_BITMAP bitmap = FPDFBitmap_Create(tw, th, 0);
    if (!bitmap) {
        FPDF_ClosePage(page);
        FPDF_CloseDocument(doc);
        return false;
    }

    FPDFBitmap_FillRect(bitmap, 0, 0, tw, th, pdfiumFillArgb(request.paperColor));

    FS_RECTF clip{};
    clip.left = 0.0f;
    clip.top = 0.0f;
    clip.right = static_cast<float>(tw);
    clip.bottom = static_cast<float>(th);

    FPDF_RenderPageBitmapWithMatrix(bitmap, page, &matrix, &clip, FPDF_ANNOT);
    FPDF_ClosePage(page);

    QImage tile = bitmapToImage(bitmap, tw, th);
    FPDFBitmap_Destroy(bitmap);
    FPDF_CloseDocument(doc);

    if (tile.isNull()) {
        return false;
    }
    *out = std::move(tile);
    return true;
}

} // namespace pdf_document_view

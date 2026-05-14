#pragma once

#include "pdf_render_backend.hpp"
#include "pdf_tile_cache.hpp"

#include <QByteArray>
#include <QColor>
#include <QMutex>
#include <QObject>
#include <QRect>
#include <QSize>
#include <QString>
#include <deque>

namespace pdf_document_view {

struct TileJob {
    quint64 generation = 0;
    TileKey key;
    /// Raster entry point for async tiles (same engine as the widget's `m_backend`); must stay alive while jobs run.
    PdfRenderBackend* renderBackend = nullptr;
    QByteArray pdfBytes;
    QString filePath;
    QSize devicePixelSize;
    QRect tileRect;
    QColor paperColor;
};

class PdfTileRenderWorker final : public QObject {
    Q_OBJECT

public:
    explicit PdfTileRenderWorker(QObject* parent = nullptr);
    ~PdfTileRenderWorker() override = default;

public slots:
    void enqueue(TileJob job);
    void processQueue();
    void purgeQueue(quint64 minValidGeneration);
    /// Clears queued jobs only (used at widget shutdown; does not advance validity generation).
    void clearPendingJobs();

signals:
    void tileReady(quint64 generation,
        QString docIdentity,
        int pageIndex,
        int zoomMilli,
        int dprMilli,
        int col,
        int row,
        QImage image);

private:
    mutable QMutex m_mutex;
    std::deque<TileJob> m_queue;
    quint64 m_minValidGeneration = 0;
};

} // namespace pdf_document_view

Q_DECLARE_METATYPE(pdf_document_view::TileKey)
Q_DECLARE_METATYPE(pdf_document_view::TileJob)

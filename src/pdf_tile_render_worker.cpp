/**
 * @file pdf_tile_render_worker.cpp
 * @brief QObject worker: receives tile jobs on a dedicated thread, emits `tileReady` to the GUI thread.
 */

#include "pdf_tile_render_worker.hpp"

#include <pdf_document_view/page_render_request.hpp>

#include <QMutexLocker>

namespace pdf_document_view {

PdfTileRenderWorker::PdfTileRenderWorker(QObject* parent)
    : QObject(parent) {
    qRegisterMetaType<pdf_document_view::TileKey>("pdf_document_view::TileKey");
    qRegisterMetaType<pdf_document_view::TileJob>("pdf_document_view::TileJob");
}

void PdfTileRenderWorker::enqueue(TileJob job) {
    {
        QMutexLocker locker(&m_mutex);
        if (job.generation < m_minValidGeneration) {
            return;
        }
        m_queue.push_back(std::move(job));
    }
    QMetaObject::invokeMethod(this, "processQueue", Qt::QueuedConnection);
}

void PdfTileRenderWorker::purgeQueue(quint64 minValidGeneration) {
    QMutexLocker locker(&m_mutex);
    m_minValidGeneration = minValidGeneration;
    m_queue.clear();
}

void PdfTileRenderWorker::clearPendingJobs() {
    QMutexLocker locker(&m_mutex);
    m_queue.clear();
}

void PdfTileRenderWorker::processQueue() {
    for (;;) {
        TileJob job;
        {
            QMutexLocker locker(&m_mutex);
            if (m_queue.empty()) {
                return;
            }
            job = std::move(m_queue.front());
            m_queue.pop_front();
        }
        if (job.generation < m_minValidGeneration) {
            continue;
        }

        PageRenderRequest req;
        req.pageIndex = job.key.pageIndex;
        req.devicePixelRatio = 1.0;
        req.devicePixelSize = job.devicePixelSize;
        req.paperColor = job.paperColor;

        QImage img;
        if (!job.renderBackend
            || !job.renderBackend->renderPageTile(job.pdfBytes, job.filePath, req, job.tileRect, &img)
            || img.isNull()) {
            continue;
        }

        emit tileReady(job.generation,
            job.key.docIdentity,
            job.key.pageIndex,
            job.key.zoomMilli,
            job.key.dprMilli,
            job.key.col,
            job.key.row,
            std::move(img));
    }
}

} // namespace pdf_document_view

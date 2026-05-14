# Rendering, threading, and the GUI thread

This document records the **current** rasterization and threading policy for **`PdfDocumentViewWidget`** in this repository (PDFium by default; optional **Poppler-Qt6** when configured). It is the operational companion to **`docs/adrs/0002-render-threading-and-tiles-policy.md`**.

---

## v1 policy: main-thread, synchronous, full-page raster

- **Where work runs:** Page paint goes through **`PdfiumBackend::renderPage`**, which is invoked from normal **Qt widget painting** — typically the **GUI thread** (`QThread::currentThread() == qApp->thread()`).
- **What is rasterized:** When **tile rendering is off** (or the backend does not expose `PdfWidgetCapabilities::tileRendering`), each paint pass may build a **single bitmap for the entire page** at the target device size (respecting **device pixel ratio**), then **`QPainter::drawImage`** blits it into the widget.
- **Synchronous contract:** **`FPDF_RenderPageBitmap`** (and surrounding load/close page calls) **complete before** paint returns. Large pages at high DPR can therefore **block the GUI thread** for the duration of that PDFium call chain.

This path remains available for **simplicity and correctness** when tiles are disabled, and for backends without tile support.

---

## MVP tile path (PDFium, GUI thread)

When **`PdfDocumentViewWidget::tileRenderingEnabled()`** is true and the backend reports **`tileRendering`**, **`PdfDocumentViewWidget`** paints the letterboxed page area from a **device-space tile grid** (default **512×512** device pixels; see `kTileDevicePixels` in `src/pdf_document_view_widget.cpp`).

- **Cache:** `TileImageCache` (`src/pdf_tile_cache.hpp`) holds up to **64** tiles keyed by document identity, page index, zoom and DPR (milli-quantized), and tile column/row. LRU eviction on insert.
- **Rasterization (PDFium):** **`PdfiumBackend::renderPageTile`** calls **`FPDF_RenderPageBitmapWithMatrix`** with a page→device matrix derived from **`FPDF_PageToDevice`** (same mapping as a full `FPDF_RenderPageBitmap(..., 0, 0, iw, ih, 0, …)` pass), translated so the tile’s origin in the full-page device bitmap maps to `(0,0)` in the tile bitmap. This avoids an MVP-1 “full page per tile” intermediate buffer.
- **Rasterization (Poppler-Qt6, when linked):** **`PopplerQtBackend::renderPageTile`** uses **`Poppler::Page::renderToImage`** with a **region** matching the same device-space tile rectangle (GUI-thread path; same cache keys as PDFium).
- **Progressive fill:** Missing tiles draw a **light gray placeholder**; **`QTimer::singleShot(0, …)`** drains a FIFO of at most **one tile** per queued callback so the event loop stays responsive. When **`asyncTileRenderingEnabled()`** is **false**, the backend’s live-model **`renderPageTile`** runs on the **GUI thread** (see **Async MVP** below for the optional worker path).
- **Invalidation:** Document / page / zoom / paper / tile-flag changes clear the tile cache and full-page raster cache. When a tile finishes, **`viewport()->update(QRect)`** targets that tile’s intersection with the viewport when possible.
- **Capabilities:** **`PdfDocumentViewWidget::capabilities()`** reports **`tileRendering`** only when the widget flag and backend capability agree.

---

## Async MVP (optional worker thread)

When **`PdfDocumentViewWidget::asyncTileRenderingEnabled()`** is **true** (default **OFF**), **`PdfDocumentViewWidget::capabilities().asyncTileRendering`** is **true** only while that widget flag stays enabled **and** the backend advertises support (PDFium; linked **Poppler-Qt6** when **`PDFDOCUMENTVIEW_POPPLER_REAL`** is on). **Poppler stub:** async stays **off**; the capability is **false**.

- **Thread model:** A dedicated **`QThread`** named **`PdfTileRenderThread`** hosts **`PdfTileRenderWorker`** (`src/pdf_tile_render_worker.hpp`). Slots **`enqueue(TileJob)`** and **`processQueue()`** run on that thread. A **FIFO queue** serializes work so **all tile raster API** for a job runs on **one** background thread (no pool).
- **Isolation (PDFium):** Each job carries either a **`QByteArray` copy** of the PDF (`PdfiumDocumentModel::savedCopy()` after buffer open) or an absolute file path (`PdfiumDocumentModel::loadedFilePath()`). The worker calls **`FPDF_LoadMemDocument64`** or **`FPDF_LoadDocument`**, renders **one** tile, then **`FPDF_CloseDocument`** — **no** sharing of the GUI-thread **`FPDF_DOCUMENT`**. **`pdfium_library`** init uses the existing refcount; the worker calls **`ensureLibrary()`** once for its lifetime and **`releaseLibrary()`** in **`~PdfTileRenderWorker`**.
- **Isolation (Poppler-Qt6):** The same **`TileJob`** queue and **`PdfTileRenderWorker`** invoke **`PdfRenderBackend::renderPageTile(QByteArray, QString, …)`** on the worker thread. **`PopplerQtBackend`** opens a **fresh** **`Poppler::Document`** from the job’s bytes or absolute path, renders **one** tile with **`Page::renderToImage`**, and destroys the document — **no** sharing of the GUI-thread document handle.
- **GUI thread:** With async **on**, live-model **`renderPageTile`** is **not** used for tiles; the GUI thread only enqueues **`TileJob`** values and applies **`tileReady`** results. **Full-page** raster when tiles are off still uses **`renderPage`** on the GUI thread (unchanged).
- **Cancellation:** **`m_renderGeneration`** (quint64) increments on tile invalidations (`clearAllRasterAndTileState` while async is on: document / page / zoom / paper / tile-flag changes, etc.). **`purgeQueue(minGeneration)`** clears the worker queue and sets a **minimum valid generation**; jobs with a lower generation are skipped. **`tileReady`** carries the job’s generation; the widget accepts results only when **`generation == m_renderGeneration`** and the **tile key** still matches the current document identity, page, zoom, and DPR. In-flight renders for a **superseded** generation may still finish; their **`tileReady`** emissions are **ignored** (no cache write).
- **Shutdown:** **`clearPendingJobs()`** drops queued jobs without poisoning the generation counter (used when destroying the widget after disconnecting **`tileReady`**). The thread is stopped with **`quit()`** / **`wait()`**; the worker is moved back to the application thread for deletion.
- **Trade-offs:** Per-tile **document load + close** is **CPU and I/O heavy** (honest MVP for PDFium threading). Prefer **OFF** for small PDFs, low latency, or when profiling shows load dominates. **Buffer-backed** documents pay a **byte copy per tile job** unless you disable async.

---

## `PdfiumDocumentModel` mutex (`m_mutex`)

The PDFium-backed document type holds a **`QMutex`** (`mutable QMutex m_mutex` on **`PdfiumDocumentModel`**) used with **`QMutexLocker`** for:

- **Document lifecycle and metadata:** `openFile`, `openBuffer`, `close`, `pageCount`, `pageSize`, error and identity accessors — all serialized against concurrent mutation of **`FPDF_DOCUMENT`** and cached page sizes.
- **Render and find (via backend):** **`PdfiumBackend::renderPage`**, **`PdfiumBackend::renderPageTile`**, and **`PdfiumBackend::findTextMatches`** take the **same** mutex for the duration of PDFium page/text work so **find** and **paint** do not interleave unsafe **`FPDF_PAGE`** / **`FPDF_TEXTPAGE`** use on the same document.

**Note:** `m_mutex` is an internal implementation detail (friend access from the backend); the **public** contract is still “call the widget and model from the **GUI thread**” per Qt norms. The mutex exists to guard **re-entrancy and future call sites** on the **shared** `FPDF_DOCUMENT`. **Async tiles** (optional) do not use that shared handle for raster work; see **`docs/rendering-threading.md`** “Async MVP”.

---

## Non-goals (today)

- **Multi-page / continuous scroll tile scheduling** — the MVP tile grid covers the **current page** bitmap only; there is no independent priority system across pages.
- **Worker pool / multi-threaded engines** — async tiles use **one** `PdfTileRenderWorker` thread and **no** shared per-process document handle across threads for tile jobs (each job loads an isolated document).
- **Mid-tile cooperative cancel** — async jobs are not interrupted inside **`FPDF_RenderPageBitmapWithMatrix`**; cancellation is **generation-based** after the call returns.
- **Overlapping render with unrelated GUI work** on the **synchronous** tile path: each **`PdfiumBackend::renderPageTile`** call still holds **`m_mutex`** for its PDFium call chain.

A separate **`std::mutex`** in **`pdfium_library.cpp`** serializes **global** PDFium init/shutdown (`FPDF_InitLibrary` / `FPDF_DestroyLibrary` style usage); that is **library-wide**, not a per-document render pipeline.

---

## When async or tiles would be reconsidered

Revisit this policy when **measurable** GUI hitches (large pages, 4K DPR, slow storage) or product requirements (smooth scrolling, **continuous** mode with multiple pages visible) demand it **and** the project is ready to own:

- **Thread affinity rules** for PDFium (upstream expectations, page handle lifetimes, and whether a **single** background thread vs pool is sufficient).
- **Stronger cancellation** than generation drops (e.g. worker-held document snapshots to avoid **per-tile** reload cost).
- **Memory caps** for tile caches and peak decode size.
- **Qt integration:** `QImage`/`QPixmap` updates from workers must still **marshal to the GUI thread** for painting (`QMetaObject::invokeMethod`, `QTimer::singleShot(0, ...)`, or `Qt::QueuedConnection`), consistent with **widgets may only be touched from the GUI thread**.

Hosts may disable **tiles** to recover the single full-page blit path, keep **tiles on + async off** for the GUI-thread progressive drain, or enable **async tiles** when the per-job load cost is acceptable.

---

## Interaction with Qt GUI thread rules

- **Widgets and `QPainter`:** Per Qt documentation, painting and most widget API should run on the **GUI thread**. The **default** tile path still runs **tile raster** on the GUI thread; **optional async tiles** run engine work on **`PdfTileRenderThread`** and deliver pixels via **`Qt::QueuedConnection`**.
- **Re-entrancy:** Holding a mutex during paint means **nested** event processing (e.g. a dialog spun up from a paint path — which hosts should avoid) could deadlock or stall; hosts should keep paint cheap and **non-reentrant** where possible.
- **Background raster:** Worker code must **not** call arbitrary widget methods; **`tileReady`** is received on the GUI thread and only updates **`TileImageCache`** plus **`viewport()->update`**.

---

## Document status

**Working technical note** — aligned with current code; update when the render pipeline gains further off-thread raster workers or a revised public threading contract.

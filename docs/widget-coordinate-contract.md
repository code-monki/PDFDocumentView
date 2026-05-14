# Embeddable widget — coordinate contract

This document defines how **`PdfDocumentViewWidget`** maps **PDF geometry** to **Qt paint coordinates** so hosts can align custom overlays, tooltips, and accessibility metadata with the painted page. It complements **`docs/concept-pdf-document-view.md`** (layering) and **`docs/rendering-threading.md`** (when and where raster work runs).

---

## 1. Page space (canonical geometry)

**Page space** is the library’s **canonical** coordinate system for a single page:

- **Units:** PDF **user units** interpreted as **1/72 inch** (typical “points”) per axis.
- **Origin:** **Top-left** of the page; **X** increases to the right; **Y** increases **downward**.

Engines such as PDFium often expose geometry in a **Y-up** user space. Implementations convert to **top-down page space** before surfacing public types (for example **`TextMatch::pageRect`** in **`include/pdf_document_view/text_match.hpp`**). The same convention applies to **`PdfDocumentModel::pageSize(int)`** (`QSizeF` width/height in points) and to the matrices used when rasterizing—hosts should treat **`pageSize()`** and **`TextMatch::pageRect`** as the same basis.

---

## 2. Layout document pixels vs device pixels

Two pixel grids appear in the widget pipeline; both derive from the current **zoom** (`zoom()`, `setZoom()`, `resetZoom()`, `fitWidth()`), the viewport **logical DPI**, and the viewport **`devicePixelRatio`**.

### 2.1 Layout document pixels (scroll/document coordinates)

The widget lays out the current page as a rectangle in **logical** coordinates used for **`QAbstractScrollArea`** scrolling:

- Size follows **`ceil(pagePts × zoom × logicalDpi / 72)`** in each axis (see implementation: **`documentPixelSize()`** in `src/pdf_document_view_widget.cpp`).
- The **scrollable document** has its origin at **(0, 0)** in this space; **`horizontalScrollBar()->value()`** / **`verticalScrollBar()->value()`** select the visible window.

When painting the viewport, the widget applies **`QPainter::translate(-scrollX, -scrollY)`** so that **document (0, 0)** maps to the **top-left of the page** as seen through the viewport.

### 2.2 Device pixels (raster backing store)

Rasterization targets a **high-resolution** bitmap whose integer size is approximately **`ceil(layoutDocPx × devicePixelRatio)`** per axis. Those dimensions are passed to the backend as **`PageRenderRequest::devicePixelSize`** together with **`PageRenderRequest::devicePixelRatio`** (see **`include/pdf_document_view/page_render_request.hpp`**).

- **`PageRenderRequest`** fields: **`pageIndex`**, **`devicePixelRatio`**, **`devicePixelSize`**, **`paperColor`**.
- The backend draws PDF content into that device-pixel grid; the widget then **scales** that image back into **layout document pixels** for composition (full-page path) or paints **tiles** at the same logical scale (tile path).

So: **page points** → **layout document pixels** (zoom + logical DPI) → **physical device pixels** in **`PageRenderRequest`** (DPR); the painter maps back to viewport **logical** coordinates for on-screen composition.

---

## 3. Widget viewport vs page rect

| Concept | Meaning |
|--------|---------|
| **Viewport** | The **`QAbstractScrollArea`** inner **`viewport()`** widget: the **visible** region in **widget-local logical pixels** (origin top-left of the viewport). |
| **Page rect (layout)** | **`QRectF(0, 0, documentWidth, documentHeight)`** in **document/scroll coordinates**—one page at the current zoom. |
| **Visible slice** | Intersection of the page rect with the rectangle **`(scrollX, scrollY, viewportWidth, viewportHeight)`** in document space; the paint path clips/translates so the user sees that slice. |

Hosts that **subclass or compose around** the widget should not assume the page fills the viewport: margins, scroll position, and zoom can all leave **letterboxing** or **off-screen** areas.

---

## 4. How `PageRenderRequest` relates

**`PageRenderRequest`** is the **backend-facing** description of **one rasterization** of a page at a given **device-pixel** resolution. It does **not** carry a PDF matrix from the host; the widget and backend agree on scale through **`devicePixelSize`** and **`devicePixelRatio`** relative to **`pageSize()`** and the widget’s **zoom**.

- **Full-page paint:** The widget builds an offscreen image of size **`devicePixelSize`**, calls **`PdfRenderBackend::renderPage`**, then draws that image into the **layout** page rect (so one PDF page maps to the full scrollable area for that page index).
- **Tiles:** **`renderPageTile`** receives an additional **device-space tile rectangle**; each tile still uses the same **`PageRenderRequest`** basis for matrix consistency (see **`docs/rendering-threading.md`**).

Integrators **outside** the backend typically do **not** construct **`PageRenderRequest`**; they rely on **`zoom()`** / **`fitWidth()`** and **`capabilities()`** to understand what the active backend can do.

---

## 5. Host responsibilities (chrome, dialogs, activation)

Per **`docs/concept-pdf-document-view.md`** and **`AGENTS.md`**:

- **Menus, toolbars, shortcuts, persistence, and file dialogs** belong to the **host application**. The widget exposes **document open** entry points (**`setDocumentPath`**, **`openDocumentBuffer`**) but does not mandate **how** paths are chosen.
- **Window activation** and **native vs non-native** dialogs (especially on **macOS**) are **host** concerns; the library avoids hard-coding dialog policy beyond the **minimal demo**.

---

## 6. Mapping `TextMatch::pageRect` to paint coordinates

**`TextMatch`** carries **`pageIndex`** and **`pageRect`** in **top-down page space** (points). The widget’s built-in find highlights use the same mapping a host should use for **custom overlays** on the **current page**:

1. Obtain **`QSizeF pagePts = pageSize(pageIndex)`** (via **`documentModel()`** / backend as appropriate) and **`QSize docPx = documentPixelSize()`** for the widget’s **current** zoom and DPI (the widget computes this internally for the visible page).
2. Scale independently in X and Y:

   - **`sx = docPx.width() / pagePts.width()`**
   - **`sy = docPx.height() / pagePts.height()`**

3. Convert the match rectangle to **layout document** coordinates:

   - **`QRectF layout(m.pageRect.x() * sx, m.pageRect.y() * sy, m.pageRect.width() * sx, m.pageRect.height() * sy)`**

4. Paint in the **viewport** painter after **`translate(-scrollX, -scrollY)`**, or equivalently subtract **`(scrollX, scrollY)`** from **`layout`** to get **viewport-local** coordinates.

This matches **`paintFindHighlights`** / **`ensureFindMatchVisible`** in **`pdf_document_view_widget.cpp`**: highlights align with the **same** scale used for the page bitmap. If **`pageIndex`** does not match the **currently displayed** page, map only when that page is visible (or switch pages first via **`setCurrentPageIndex`** / **`scrollToFindMatch`**).

**Reveal API:** **`revealPageRect`** / **`navigateToHostSearchHit`** take **`pageIndex`** and **`pageRect`** in the **same top-down page space** as **`TextMatch::pageRect`** and **`PageHighlight::pageRect`**; scroll positioning reuses the same page-rect → layout-document mapping as the built-in find “scroll hit into view” path.

**Open + external hit (one call):** **`openDocumentAndRevealSearchHit(filePath, searchTerm, pageIndex, pageRect)`** uses **`setDocumentPath`** for **`filePath`**, then applies **`pageIndex`** / **`pageRect`** as the **source of truth** for page and scroll (plus one **`PageHighlight`** with find-style colors). **`searchTerm`** only drives **best-effort** population of widget find state **after** that scroll; it does **not** replace geometry when the backend’s matches differ.

---

## 7. Related public API (scale and capability discovery)

| Method / type | Role |
|---------------|------|
| **`zoom()`**, **`setZoom(double)`**, **`resetZoom()`**, **`fitWidth()`** | User-visible scale; **`fitWidth()`** sets zoom from viewport width vs page width. |
| **`capabilities()`** | Backend and widget flags (**tiles**, **async tiles**, **buffer open**, **text search**, etc.); see **`pdf_capabilities.hpp`**. |
| **`findMatches()`**, **`currentFindMatch()`**, **`scrollToFindMatch`** | Access **`TextMatch::pageRect`** in page space and scroll the widget. |
| **`revealPageRect`**, **`navigateToHostSearchHit`** | Scroll an arbitrary page-space rect into view (and optionally set one host highlight) without using widget find state. |
| **`openDocumentAndRevealSearchHit`** | Open a **file path**, then jump to **`(pageIndex, pageRect)`** in page space; **`searchTerm`** optionally fills find matches **without** auto-scrolling away from **`pageRect`** unless a match equals that rect. |

For **threading and tile/async** behavior, see **`docs/rendering-threading.md`** and **`docs/adrs/0002-render-threading-and-tiles-policy.md`**.

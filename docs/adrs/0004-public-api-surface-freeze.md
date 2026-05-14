# ADR-0004: Public API surface freeze (0.1.x)

## Status

**Accepted** — defines the **stability contract** for headers and types under `include/pdf_document_view/` for **`0.1.x`** releases. **Implementation** in `src/` may change within that line as long as the frozen surface stays compatible per the semver rules below.

---

## Context

- The repository delivers an **embeddable Qt widget library**; the **integration contract** is the **installed public headers** and their documented behavior, not demo-only wiring (`AGENTS.md`, `docs/concept-pdf-document-view.md`).
- **ADR-0003** records **packaging and engine-selection policy** (PDFium default, Poppler opt-in, split-package intent). Engine choice affects **capabilities** and **runtime behavior**, but must not blur what counts as **semver-governed** API.
- **Geometry and paint mapping** for hosts and overlays are specified in **`docs/widget-coordinate-contract.md`** (page space, layout vs device pixels, `PageRenderRequest`, `TextMatch::pageRect` → `QPainter`). This ADR **references** that document; coordinate semantics are **stable for 0.1.x** unless bumped with a **MAJOR** and an ADR update.
- Root **`CMakeLists.txt`** sets **`project(PDFDocumentView VERSION 0.1.0 …)`**; this ADR aligns **API freeze** with the **`0.1.x`** line.

---

## Decision

1. **Frozen public surface** — The bullet lists in **Frozen surface** (headers, types, `PdfDocumentViewWidget` entry points and signals) are **stable for `0.1.x`**: **PATCH** and **MINOR** releases must not break them without a **MAJOR** version bump and an **update to this ADR** (or a successor ADR that explicitly supersedes the frozen list).
2. **Semantic versioning (this repo, `0.1.x`)**
   - **PATCH (`0.1.z`)** — Compatible **bug fixes**, documentation, and **non-breaking** behavior corrections; no removals or signature changes on the frozen list.
   - **MINOR (`0.1.y`)** — May add **optional, backward-compatible** API (e.g. new methods, new struct fields with safe defaults, new signals) without breaking existing callers.
   - **MAJOR (`1.0.0` or next breaking epoch)** — **Removals**, **breaking signature or behavior changes**, or **incompatible coordinate/render contract changes** require a **MAJOR** bump and **ADR** revision.
3. **Internal implementation** — Types, classes, and functions that live **only** under `src/` (and any future `detail/` headers) are **not** covered by this freeze; they may change across **PATCH** releases if the frozen headers and behavior remain compatible.

---

## Frozen surface

### Public headers (installed)

All of the following are installed under **`include/pdf_document_view/`** when **`PDFDOCUMENTVIEW_INSTALL`** is enabled:

| Header | Role |
|--------|------|
| **`pdf_document_view_widget.hpp`** | `PdfDocumentViewWidget` — primary embeddable `QWidget`. |
| **`pdf_document_model.hpp`** | `PdfDocumentModel`, `createDefaultPdfDocumentModel()`. |
| **`pdf_capabilities.hpp`** | `PdfWidgetCapabilities`. |
| **`page_render_request.hpp`** | `PageRenderRequest`. |
| **`page_highlight.hpp`** | `PageHighlight`. |
| **`text_match.hpp`** | `TextMatch`. |

### Types (stable)

- **`PdfDocumentModel`** — Abstract document interface: **`openFile`**, **`openBuffer`**, **`supportsOpenBuffer`**, **`close`**, **`isOpen`**, **`pageCount`**, **`pageSize`**, **`lastError`**, **`identityKey`**, **`sourceLabel`**, virtual destructor.
- **`createDefaultPdfDocumentModel()`** — Factory returning the model implementation for the active CMake render backend.
- **`PdfWidgetCapabilities`** — Struct and its **data members** (`textSearch`, `renderPages`, `openFromBuffer`, `tileRendering`, `asyncTileRendering`).
- **`PageRenderRequest`** — Struct and its **data members** (`pageIndex`, `devicePixelRatio`, `devicePixelSize`, `paperColor`).
- **`PageHighlight`** — Struct and its **data members** (`pageIndex`, `pageRect`, `fill`, `border`).
- **`TextMatch`** — Struct and its **data members** (`pageIndex`, `pageRect`).

### `PdfDocumentViewWidget` — stable methods and signals

**Construction / lifetime:** `PdfDocumentViewWidget(QWidget* parent = nullptr)`, destructor, deleted copy and move operations.

**Backend / model / document:** `backendId`, `capabilities`, `documentModel`, `setDocumentPath`, `openDocumentBuffer`, `documentPath`, `documentSourceLabel`, `isDocumentOpen`, `documentError`.

**Navigation:** `pageCount`, `currentPageIndex`, `setCurrentPageIndex`, `nextPage`, `previousPage`, `revealPageRect`, `navigateToHostSearchHit`, `openDocumentAndRevealSearchHit`.

**Zoom / layout:** `zoom`, `setZoom`, `resetZoom`, `fitWidth`.

**Appearance:** `paperColor`, `setPaperColor`.

**Find:** `findText`, `findNext`, `findPrevious`, `clearFind`, `findMatchCount`, `findCurrentIndex`, `findMatches`, `currentFindMatch`, `scrollToFindMatch(int)`, `scrollToFindMatch(const TextMatch&)`, `findHighlightFillColor`, `setFindHighlightFillColor`, `findActiveMatchOutlineColor`, `setFindActiveMatchOutlineColor`, `findActiveMatchOutlineWidth`, `setFindActiveMatchOutlineWidth`.

**Host highlights:** `setPageHighlights`, `clearPageHighlights`, `pageHighlights`.

**Rendering policy (widget flags):** `tileRenderingEnabled`, `setTileRenderingEnabled`, `asyncTileRenderingEnabled`, `setAsyncTileRenderingEnabled`, `asyncTileRenderingSupported`.

**Signals:** `viewStateChanged()`, `documentOpened(bool ok)`, `documentError(const QString& message)`, `findResultsChanged(int matchCount, int currentIndex)`.

**Note on `protected` members:** The class exposes Qt **`protected`** overrides (`viewportEvent`, `resizeEvent`, `keyPressEvent`, `eventFilter`). They are **not** documented as supported extension points for hosts; **composition** and the **public** API above are the supported integration model. Subclassing that relies on overriding these remains **at integrator risk** across releases within **0.1.x**.

---

## Non-stable / internal (explicit)

- **All translation units and headers under `src/`** — including but not limited to **`PdfRenderBackend`** (`src/pdf_render_backend.hpp`), concrete backends (e.g. PDFium / stub), **tile cache**, **`PdfTileRenderWorker`** / **`PdfDocumentViewTileRuntime`**, and any worker-thread plumbing.
- **Forward declarations** in public headers that only exist to support **private** implementation (e.g. worker types behind **`#if PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM`**) — not a supported public type surface.
- **Future `detail/` headers** — if introduced, treat as **non-stable** unless promoted into this ADR’s frozen list with a **MINOR** or **MAJOR** as appropriate.

---

## Consequences

- **Hosts** can depend on the **frozen headers** and **coordinate contract** (`docs/widget-coordinate-contract.md`) for **0.1.x** without expecting silent breaking changes in **PATCH** releases.
- **Maintainers** must update **this ADR** (or add a superseding ADR) when intentionally changing the frozen list or coordinate semantics tied to **MAJOR** bumps.
- **Backend packaging** choices remain governed by **ADR-0003** and **ADR-0001**; capability flags may gain new fields under **MINOR** per the rules above.

---

### Additive note (MINOR, host indexer / external search)

- **`revealPageRect(int, const QRectF&)`** — Scrolls an arbitrary **page-space** rectangle into view using the same layout mapping as find scroll-to (shared with **`scrollToFindMatch`** via an internal helper). Lets hosts jump to hits from an external index without populating widget find state.
- **`navigateToHostSearchHit(int, const QRectF&, bool showHighlight)`** — Composes **`revealPageRect`** with optional **`setPageHighlights`** (find-style default colors when `showHighlight` is true).
- **`openDocumentAndRevealSearchHit(const QString& filePath, const QString& searchTerm, int pageIndex, const QRectF& pageRect)`** — **`setDocumentPath(filePath)`** then, on success, **`setCurrentPageIndex`**, one host **`PageHighlight`** for **`pageRect`**, **`revealPageRect`**, and a **non-scrolling** find pass for **`searchTerm`** (same match list as **`findText`**) that activates **`findCurrentIndex`** only when a hit equals **`(pageIndex, pageRect)`**; otherwise **`findCurrentIndex`** stays **-1** so host coordinates remain authoritative.

---

## Links

- **ADR-0003** — Split packages, Poppler opt-in, default engine packaging: [`0003-split-packages-and-poppler-opt-in.md`](0003-split-packages-and-poppler-opt-in.md)
- **Coordinate contract** — Page space, pixels, scroll, `PageRenderRequest`, overlay mapping: [`../widget-coordinate-contract.md`](../widget-coordinate-contract.md)
- **Concept** — Widget library scope and layering: [`../concept-pdf-document-view.md`](../concept-pdf-document-view.md)

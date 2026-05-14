# Tutorial: embedding PDFDocumentView

This tutorial shows how to **embed** `PdfDocumentViewWidget` in a host application. It stays at **library integration** scope: it is not a guide for building a full document viewer product (see `AGENTS.md` and `docs/concept-pdf-document-view.md`).

**Source-first / integrator obligations:** This project’s **default** deliverable is **source**—you vendor or `find_package` the library, build against **your** Qt 6 kit, and own **how** you ship (see **`README.md` → MVP → full ship**). If you publish **binaries** or store-bound apps, **signing**, **relocatable Qt bundles**, and **application-level** third-party notices are **host-product** work; **`docs/concept-pdf-document-view.md`** states boundaries, and **`docs/legal-integration-checklist.md`** lists a **bounded** engineering checklist (**not** legal advice).

## Prerequisites

- CMake 3.25+, C++20, Qt 6 Widgets.
- This repository configured with your Qt prefix (`CMAKE_PREFIX_PATH` or `QT_PREFIX` preset).
- Optional: a **small synthetic** PDF for local testing (for example `tests/fixtures/minimal-one-page.pdf`). Do not rely on copyrighted PDFs in examples or docs.

## 1. Embed the widget

Add the library to your project (`find_package(PDFDocumentView CONFIG)` after `find_package(Qt6 …)` or `add_subdirectory` / `FetchContent` per your layout). Link `PDFDocumentView::pdf_document_view`.

```cpp
#include <pdf_document_view/pdf_document_view_widget.hpp>

// …
auto* pdfView = new pdf_document_view::PdfDocumentViewWidget(this);
layout->addWidget(pdfView);
```

Include paths expose headers under `pdf_document_view/`.

## 2. Open a file or buffer

- **File:** `pdfView->setDocumentPath(QStringLiteral("/path/to/doc.pdf"));`  
  Listen for `documentOpened(bool)` and `documentError(const QString&)` for success/failure.
- **Buffer:** `pdfView->openDocumentBuffer(bytes, &err);`  
  Only when `pdfView->capabilities().openFromBuffer` is true (PDFium; intersected with the model). On failure, check `pdfView->documentError()` or `err`.

Use `pdfView->documentModel()` if you need the same open/close semantics without the widget surface.

## 3. Page navigation

- `setCurrentPageIndex(int)`, `nextPage()`, `previousPage()`.
- `pageCount()` and `currentPageIndex()` query state after a successful open.

## 4. Zoom

- `zoom()`, `setZoom(double)`, `resetZoom()`, `fitWidth()`.
- `paperColor()` / `setPaperColor()` control the page background behind transparent PDF areas.

## 5. Find-in-document (widget)

- `findText(needle)` runs a capped backend search and scrolls to the first hit when matches exist.
- `findNext()` / `findPrevious()` cycle; `clearFind()` clears overlays and indices.
- `findMatches()`, `findCurrentIndex()`, `currentFindMatch()`, and signal `findResultsChanged(int matchCount, int currentIndex)` drive host UI without logging matched text.

Match geometry is in **page space** (see below).

## 6. Host highlights (separate from find)

- `setPageHighlights({ … })` / `clearPageHighlights()` / `pageHighlights()` for rectangles you own (search-from-indexer, annotations, etc.).
- `navigateToHostSearchHit(pageIndex, pageRect, showHighlight)` scrolls; when `showHighlight` is true it replaces highlights with one find-styled box.

## 7. External search: `openDocumentAndRevealSearchHit`

Use this when another subsystem (database, indexer, web result) already knows **file path**, **page**, and **rectangle** for a hit:

```cpp
pdfView->openDocumentAndRevealSearchHit(
    filePath,
    searchTermForBestEffortFind,  // may be empty to skip find pass
    pageIndex,
    pageRectInTopDownPageSpace);
```

Behavior (summary):

1. Opens via `setDocumentPath` (same errors/signals as a normal open).
2. On success: sets page, applies one highlight using find colors, scrolls `pageRect` into view.
3. If `searchTerm` is non-empty after trim: repopulates find matches **without** auto-scrolling to the first hit. If some match has the same `(pageIndex, pageRect)` as your rectangle, `findCurrentIndex` aligns; otherwise the list may still fill for next/previous but the active index stays `-1` so built-in navigation does not override your scroll.

Always pass `pageRect` in the same **top-down page coordinates** as `TextMatch::pageRect` (see contract doc).

## 8. Coordinate contract (pointer)

All public rectangles for hits and host highlights use one contract:

- **Page space:** origin top-left, Y downward, units **1/72 in** aligned with `PdfDocumentModel::pageSize()` and rendering.
- Mapping to paint/device pixels (layout, DPR, scroll offsets) is specified in **`docs/widget-coordinate-contract.md`**.

If an external engine gives different axes, convert before calling the widget.

## 9. Backend and capabilities

- CMake option **`PDFDOCUMENTVIEW_RENDER_BACKEND`** selects **PDFIUM** (default) or **POPPLER**.
- Call `pdfView->capabilities()` for `textSearch`, `openFromBuffer`, `tileRendering`, `asyncTileRendering`, etc. Do not assume parity between engines; gate UI by flags. **Note:** merged `asyncTileRendering` is false until the opt-in is on; use `asyncTileRenderingSupported()` to enable a control that calls `setAsyncTileRenderingEnabled(true)`.
- Threading and tile/async behavior: **`docs/rendering-threading.md`** and ADR-0002.

## 10. Install and `find_package`

After building this project, `cmake --install` with your prefix installs headers, the shared library, and **`PDFDocumentViewConfig.cmake`**. In your app:

```cmake
find_package(PDFDocumentView CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE PDFDocumentView::pdf_document_view)
```

Point CMake at Qt 6 before `find_package(PDFDocumentView)`. PDFium runtime layout (library next to `pdf_document_view`) is described in the top-level **README.md**.

If you maintain **more than one** product line (for example **Poppler-linked** vs **PDFium** builds, or PDFium with vs without the shipped **`libpdfium`** drop), use **separate build trees and install prefixes** — see **README.md** (*Two install prefixes (optional “SKU” layout)*) and **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET`**. Binary distribution, notices, and GPL handling for those SKUs follow **`docs/legal-integration-checklist.md`** and **ADR-0003**.

## 11. API reference (Doxygen)

HTML reference for public headers is generated with **`PDFDOCUMENTVIEW_BUILD_DOCS=ON`** and target **`docs`**, or manually from **`docs/doxygen/`** — see **README.md** section *API reference (Doxygen)*.

## Further reading

- `docs/concept-pdf-document-view.md` — goals and boundaries.
- `docs/adrs/0004-public-api-surface-freeze.md` — semver rules for the public surface.
- `examples/basic/` — minimal menu-driven harness (integration only, not the product).

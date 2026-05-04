# PDFDocumentView

Reusable **Qt widget–centric** library for embedding PDF display and related behavior (navigation, search, highlights). See **`docs/concept-pdf-document-view.md`** for goals, boundaries, backend notes (PDFium vs Poppler), and demo-app policy.

## Repository layout

| Path | Purpose |
|------|---------|
| `include/pdf_document_view/` | Public headers |
| `src/` | Library implementation |
| `examples/basic/` | Minimal integration demo (`main.cpp` only demonstrates wiring) |
| `ai-toolkit/` | Governance and templates (same lineage as other Codemonki projects) |
| `AGENTS.md` | Agent / automation contract for this repo |

## Build (orchestrated)

Requires **CMake 3.25+**, a **C++20** compiler, and **Qt 6 Widgets**. Set **`QT_PREFIX`** to your Qt 6 installation (same idea as `CMAKE_PREFIX_PATH`):

```sh
make all QT_PREFIX="$HOME/Qt/6.9.3/macos"
make demo-run
```

Or configure manually:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$HOME/Qt/6.9.3/macos"
cmake --build build
```

On macOS the demo is built as `build/examples/basic/pdf_document_view_basic.app` (Debug multi-config layouts may nest `Debug/` — `make demo-run` searches common paths).

## Status

**Bootstrap:** placeholder `PdfDocumentViewWidget` and **basic** demo compile; PDF rendering backends are not wired yet.

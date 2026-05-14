# PDFDocumentView

Reusable **Qt widget–centric** library for embedding PDF display and related behavior (navigation, search, highlights). See **`docs/concept-pdf-document-view.md`** for goals, boundaries, backend notes (PDFium vs Poppler), and demo-app policy. **Render threading (default full-page sync on the GUI thread; optional PDFium tiles; optional async tile worker):** **`docs/rendering-threading.md`**.

Requirements traceability (RTM-0.1, forward + reverse): **`docs/requirements-traceability-matrix.md`**.

**Qt PDF replacement (host apps):** **`PdfDocumentViewWidget` with the default PDFium backend** is the **intended substitute** for **Qt PDF** (`QtPdf`, `QPdfDocument`, `QPdfView`, etc.) when a host needs **deeper PDFium-style capability** (geometry, find overlays, rendering policy) than Qt’s wrapper exposes. The library does **not** depend on Qt Pdf.

The document surface is painted as **light paper** (white page, light grey margins) so PDFs stay readable when the host or system uses a **dark Qt palette** or stylesheet.

### CI

[![CI](https://github.com/code-monki/PDFDocumentView/actions/workflows/ci.yml/badge.svg)](https://github.com/code-monki/PDFDocumentView/actions/workflows/ci.yml)

Manual **release / CPack** skeleton (no secrets): **[`.github/workflows/release.yml`](.github/workflows/release.yml)** (`workflow_dispatch` — configure with **`PDFDOCUMENTVIEW_ENABLE_CPACK=ON`**, build, **`cpack -G TGZ`**, upload artifacts).

Multi-arch smoke on **GitHub-hosted** runners: configure (Release), build library + examples, **CTest**, then an **install-prefix** `find_package(PDFDocumentView)` consumer build under **`tests/cmake_consumer_smoke/`**. Workflow: **[`.github/workflows/ci.yml`](.github/workflows/ci.yml)**. PDFium prebuilts are fetched from public [bblanchon/pdfium-binaries](https://github.com/bblanchon/pdfium-binaries) releases (pinned tag + **`file(DOWNLOAD … EXPECTED_HASH SHA256=…)`** in **`cmake/PdfiumPrebuilt.cmake`** — trust model: TLS + digest match to the in-tree pin, no CI secrets).

| OS | `runs-on` | CPU | Backend / variant | Qt via **install-qt-action** |
|----|-----------|-----|-------------------|------------------------------|
| macOS | `macos-15` | arm64 | PDFIUM (default fetch) | host/arch defaults (mac kit) |
| macOS | `macos-15-intel` | x86_64 | PDFIUM | host/arch defaults |
| Linux | `ubuntu-24.04` | x86_64 | PDFIUM | `linux` + `linux_gcc_64` |
| Linux | `ubuntu-24.04-arm` | arm64 | PDFIUM | `linux_arm64` + `linux_gcc_arm64` |
| Linux | `ubuntu-24.04` | x86_64 | Poppler-Qt6 real (`PDFDOCUMENTVIEW_POPPLER_REAL=ON`) | `linux` + `linux_gcc_64` |
| Linux | `ubuntu-24.04-arm` | arm64 | Poppler-Qt6 real | `linux_arm64` + `linux_gcc_arm64` |
| Windows | `windows-latest` | x86_64 | PDFIUM | `windows` + `win64_msvc2022_64` (Qt ≥ 6.8) |
| Windows | `windows-11-arm` | arm64 | PDFIUM | `windows_arm64` + `win64_msvc2022_arm64` |

**Runner notes:** Linux **arm64** and Windows **arm64** use **partner** images (`ubuntu-24.04-arm`, `windows-11-arm`); see [GitHub-hosted runners reference](https://docs.github.com/en/actions/reference/runners/github-hosted-runners#standard-github-hosted-runners-for-public-repositories) for specs and image churn. Intel macOS uses the **`macos-*-intel`** labels (for example **`macos-15-intel`**), not deprecated `macos-13`. Jobs use **`bash`** (including on Windows) so configure/install steps stay consistent. A separate job **`pdfium-root-no-fetch`** on **`ubuntu-24.04`** caches the extracted **linux x64** prebuilt (same pin as **`cmake/PdfiumPrebuilt.cmake`**), configures with **`PDFDOCUMENTVIEW_FETCH_PDFIUM=OFF`** and **`PDFIUM_ROOT`** (no CMake-time PDFium download), runs **CTest**, and smoke-runs **`tools/aggregate_pdfium_third_party_notices.py`**.

If CMake ever mis-identifies the CPU for a PDFium tarball, set cache **`PDFDOCUMENTVIEW_CMAKE_ARCH`** (see **PDFium prebuilt** table below).

Expanded CI/test ↔ requirement mapping: **`docs/requirements-traceability-matrix.md`** (incl. **NFR-002**).

- **Tests:** **`pdf_document_fixture_smoke`** opens the synthetic tiny PDF at **`tests/fixtures/minimal-one-page.pdf`** (PDFium builds); **`pdf_document_model_smoke`** covers invalid buffer paths without a fixture.

### Public API stability

Headers under **`include/pdf_document_view/`** follow the **`0.1.x`** rules in **`docs/adrs/0004-public-api-surface-freeze.md`**: **PATCH** releases keep the frozen surface compatible; **MINOR** may add optional API (e.g. new struct fields with defaults); **MAJOR** and an ADR revision are required for breaking removals or contract changes. Coordinate semantics for overlays are spelled out in **`docs/widget-coordinate-contract.md`**.

### API reference (Doxygen)

- **CMake (optional):** configure with **`-DPDFDOCUMENTVIEW_BUILD_DOCS=ON`**, then build target **`docs`** (requires the **`doxygen`** executable on `PATH`). Output: **`${CMAKE_BINARY_DIR}/docs/api-html/html/index.html`** (from `docs/doxygen/Doxyfile.in`).
- **Without CMake:** install [Doxygen](https://www.doxygen.nl/), then from the repository:

```sh
cd docs/doxygen
doxygen Doxyfile
```

Open **`docs/doxygen/html/index.html`** (paths relative to that directory as generated by the checked-in `Doxyfile`).

If **`PDFDOCUMENTVIEW_BUILD_DOCS=ON`** but Doxygen is not found, CMake emits a **warning** and skips the `docs` target; use the standalone **`docs/doxygen/Doxyfile`** flow above.

**Tutorial:** **`docs/tutorial-pdf-document-view.md`** — embed widget, open file/buffer, navigation, zoom, find, host highlights, `openDocumentAndRevealSearchHit`, coordinates, capabilities, `find_package`.

## Repository layout

| Path | Purpose |
|------|---------|
| `include/pdf_document_view/` | Public headers |
| `src/` | Library implementation |
| `docs/doxygen/` | Doxygen `Doxyfile` / `Doxyfile.in` (API HTML); see README |
| `examples/basic/` | Minimal **integration harness** (not a product viewer): menus call `PdfDocumentViewWidget` — open/close via `setDocumentPath`, navigation, zoom/fit, PDFium tile + async toggles, find, demo `PageHighlight` overlay; no `openDocumentBuffer` in this binary. Hosts own chrome, dialogs policy, persistence. |
| `ai-toolkit/` | Governance and templates (same lineage as other Codemonki projects) |
| `AGENTS.md` | Agent / automation contract for this repo |

## Document model

The **document layer** is the virtual interface **`PdfDocumentModel`** in **`include/pdf_document_view/pdf_document_model.hpp`**. Use **`createDefaultPdfDocumentModel()`** to obtain the implementation that matches the CMake render backend (**PDFium** loads files and memory buffers; **Poppler** is a stub unless **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`** with Poppler-Qt6 installed).

| API | Role |
|-----|------|
| **`openFile(path, errorMessage?)`** | Load from disk; on failure sets **`lastError()`** and optionally fills `errorMessage`. |
| **`openBuffer(data, errorMessage?)`** | Load from bytes (PDFium: in-memory; stub: not supported). |
| **`close()`** / **`isOpen()`** | Release the document and query open state. |
| **`pageCount()`** | Number of pages after a successful open. |
| **`lastError()`** | Last failure from **`openFile`** / **`openBuffer`**; cleared on successful open. |
| **`identityKey()`** / **`sourceLabel()`** | Stable cache key and human-readable source (path vs buffer summary). |

**`PdfDocumentViewWidget`** owns a default model and exposes the same behavior through **`setDocumentPath`**, **`openDocumentBuffer`**, **`documentModel()`**, **`documentError()`**, **`pageCount()`**, **`capabilities()`** (see **`pdf_capabilities.hpp`**; `openFromBuffer` is intersected with **`PdfDocumentModel::supportsOpenBuffer()`**), and signals **`documentOpened(bool ok)`** and **`documentError(const QString& message)`**. Hosts that only need open/page-count/error semantics can use the model without the widget.

### Embeddable API / coordinates

**Page space** (top-down PDF points), **layout vs device pixels**, how the viewport scroll maps to the page, **`PageRenderRequest`**, and mapping **`TextMatch::pageRect`** into paint coordinates are defined in **`docs/widget-coordinate-contract.md`**. For **scale** and **capability gating**, the widget exposes **`zoom()`**, **`setZoom(double)`**, **`resetZoom()`**, **`fitWidth()`**, and **`capabilities()`** (with backend flags merged per `pdf_capabilities.hpp`).

## Planning dashboard (optional)

For a local **offline WBS / RTM summary** view driven by JSON under `public/`, see **[`tools/wbs-dashboard/README.md`](tools/wbs-dashboard/README.md)** (`npm install` and `npm run dev` from that directory).

## Build (orchestrated)

Requires **CMake 3.25+**, a **C++20** compiler, and **Qt 6 Widgets**. Set **`QT_PREFIX`** to your Qt 6 installation (same idea as `CMAKE_PREFIX_PATH`):

```sh
make all QT_PREFIX="$HOME/Qt/6.11.0/macos"
make demo-run
```

For **in-tree** configures you can **`export QT_PREFIX=…`** once per shell session: root **`CMakeLists.txt`** appends it to **`CMAKE_PREFIX_PATH`**, so you do not need **`-DCMAKE_PREFIX_PATH=…`** on every **`cmake -S …`** line unless you want extra prefixes or prefer an explicit cache value. **CI** (`.github/workflows/ci.yml`) still wires Qt via **`jurplel/install-qt-action`** and **`QT_ROOT_DIR`**, passing **`CMAKE_PREFIX_PATH`** from that install path.

Or configure manually:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.0/macos"
cmake --build build
```

### Qt 6 preset (`mac-qt6`)

The `mac-qt6` preset is **user-configurable**: it sets `CMAKE_PREFIX_PATH` from the environment variable **`QT_PREFIX`** (no hardcoded paths). Point it at the kit directory that contains `Qt6Config.cmake` — for example a Qt **6.11.x** (or any 6.x) macOS install such as `$HOME/Qt/6.11.2/macos`:

```sh
export QT_PREFIX="$HOME/Qt/6.11.2/macos"
cmake --preset mac-qt6
cmake --build --preset mac-qt6
```

The older name **`PDFDOCUMENTVIEW_QT_PREFIX`** is not read by this preset; use **`QT_PREFIX`** (or invoke CMake without the preset and set **`CMAKE_PREFIX_PATH`** yourself).

Without the preset, pass the prefix explicitly (same path you would put in `QT_PREFIX`):

```sh
cmake -S . -B build-qt6 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.2/macos"
cmake --build build-qt6
```

**Downstream consumers of the installed CMake package** (`find_package(PDFDocumentView CONFIG)`) must likewise point CMake at *their* Qt 6 prefix (via `CMAKE_PREFIX_PATH`, `Qt6_DIR`, or an equivalent) before the `find_package` call — this is normal CMake behavior; PDFDocumentView never bakes a Qt path into its installed config.

### Render backend selection (CMake)

Set cache variable **`PDFDOCUMENTVIEW_RENDER_BACKEND`** to **`PDFIUM`** (default) or **`POPPLER`**.

- **`PDFIUM`:** links **`libpdfium`** (see **PDFium prebuilt** below). Document open/page count/errors are handled by **`PdfDocumentModel`** (see `pdf_document_model.hpp`); **`PdfDocumentViewWidget::setDocumentPath`** and **`openDocumentBuffer`** delegate to that model and emit **`documentOpened`** / **`documentError`** on failure.
- **`POPPLER`:** default build uses a **stub** model/render path (no Poppler link). For a **linked Poppler-Qt6** build (GPL obligations on distributors), set **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`** and install **`poppler-qt6`** (`pkg-config` name **`poppler-qt6`**) so **`createDefaultPdfDocumentModel()`** / **`createDefaultPdfRenderBackend()`** use the real Qt6-backed path (file + buffer open, full-page raster, **`Page::search`**-backed find with the same **`TextMatch::pageRect`** convention as PDFium, **GUI-thread** tiles via **`renderPageTile`**, and **async** tiles via the same **`PdfTileRenderWorker`** path as PDFium when **`asyncTileRenderingEnabled()`** is on — see **`docs/rendering-threading.md`**).

**Packaging / split artifacts (policy):** default **PDFium** vs optional **Poppler**-linked flavors, single install vs split prefixes or CMake **`COMPONENT`** names, and integrator notice expectations are recorded in **`docs/adrs/0003-split-packages-and-poppler-opt-in.md`** (complements ADR-0001). Install rules label **`PDFDocumentView_Runtime`**, **`PDFDocumentView_PdfiumRuntime`** (PDFIUM: shipped **`libpdfium`** / **`pdfium.dll`** only, omitted when **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=RUNTIME_WITHOUT_SHIPPED_PDFIUM`**), **`PDFDocumentView_Development`**, and **`PDFDocumentView_Documentation`** components (see ADR-0003).

**CPack (optional, default off):** set **`PDFDOCUMENTVIEW_ENABLE_CPACK=ON`** at configure time to emit CPack variables; baseline generator **`TGZ`** is set in **`cmake/PDFDocumentViewCPack.cmake`**. **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET`** controls whether the **shipped** PDFium shared library is part of **both** `cmake --install` and CPack (PDFIUM builds only; **`FULL`** default). This does **not** produce signed macOS app bundles; it is a transport/prefix sketch only.

| Goal | Configure | Install / archive |
|------|-----------|-------------------|
| **Hermetic** widget + shipped **`libpdfium`** / **`pdfium.dll`** in one prefix or **`cpack -G TGZ`** | **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=FULL`** (default) | Default **`cmake --install`** or **`cpack`** — **`PDFDocumentView_Runtime`** CPack-depends on **`PDFDocumentView_PdfiumRuntime`**. |
| **Omit shipped PDFium** (integrator supplies matching **`libpdfium`** / **`pdfium.dll`**) | **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=RUNTIME_WITHOUT_SHIPPED_PDFIUM`** on a **PDFIUM** build | **`cmake --install`** skips **`PDFDocumentView_PdfiumRuntime`** rules; **`cpack -G TGZ`** lists **`Runtime` / `Development` / `Documentation`** only. Installed **`PDFDocumentViewConfig.cmake`** sets **`PDFDOCUMENTVIEW_HAVE_PDFIUM_RUNTIME=OFF`** (no imported **`PDFDocumentView::PDFium`**). |
| **Poppler** backend | Same cache var; **no** **`PdfiumRuntime`** component exists | Setting behaves like **`FULL`** for PDFium omission (documented no-op). |

CMake preset **`cpack-without-shipped-pdfium`** (see **`CMakePresets.json`**) turns on CPack with **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** for smoke configures that already set **`QT_PREFIX`** / **`CMAKE_PREFIX_PATH`**.

Without reconfiguring, you can also split **`cmake --install`** by **component** (for example install **`PDFDocumentView_Runtime`** in one pass and **`PDFDocumentView_PdfiumRuntime`** in another) when the tree was built with **`FULL`** — see CMake **`--component`** documentation.

**Two install prefixes (optional “SKU” layout):** If you ship **two** downstream products—one **Poppler-linked** (GPL-aware) and one **PDFium**—use **two separate build directories** and **two install prefixes**. CMake does not need a single tree to satisfy both backends at once.

1. **Poppler REAL** prefix (example Linux; requires **`poppler-qt6`** dev packages on the build machine):

```sh
cmake -S . -B build-poppler -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/myorg/pdfwidget-poppler \
  -DPDFDOCUMENTVIEW_RENDER_BACKEND=POPPLER \
  -DPDFDOCUMENTVIEW_POPPLER_REAL=ON
cmake --build build-poppler
cmake --install build-poppler
```

That prefix contains **`PDFDocumentView::pdf_document_view`** built against **Poppler**; it does **not** install system **`libpoppler`** (or Qt)—your installer or OS packages still supply those at runtime.

2. **PDFIUM** prefix with **shipped** PDFium (default component set):

```sh
cmake -S . -B build-pdfium -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/myorg/pdfwidget-pdfium \
  -DPDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM \
  -DPDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=FULL
cmake --build build-pdfium
cmake --install build-pdfium
```

3. **PDFIUM** prefix **without** shipped **`libpdfium`** (you vendor PDFium elsewhere):

```sh
cmake -S . -B build-pdfium-nodrop -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/myorg/pdfwidget-pdfium-nodrop \
  -DPDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM \
  -DPDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=RUNTIME_WITHOUT_SHIPPED_PDFIUM
cmake --build build-pdfium-nodrop
cmake --install build-pdfium-nodrop
```

Each consumer app then points **`CMAKE_PREFIX_PATH`** (or **`PDFDocumentView_DIR`**) at **one** of those prefixes. Legal / notice obligations for **binary** SKUs stay with the integrator—see **`docs/legal-integration-checklist.md`** and **ADR-0003**.

**Aggregated PDFium notices (optional):** after configure has resolved a prebuilt tree that includes **`licenses/`** (fetch extract root or **`PDFIUM_ROOT`**), you can merge **`docs/about/THIRD_PARTY_NOTICES.template.md`** with verbatim **`licenses/*`** into a single markdown file:

```sh
python3 tools/aggregate_pdfium_third_party_notices.py \
  --pdfium-root /path/to/extracted/pdfium-prebuilt \
  --out docs/about/THIRD_PARTY_NOTICES.pdfium.md
```

Default **`--out`** is **`docs/about/THIRD_PARTY_NOTICES.pdfium.md`** (that path is **gitignored** as generated). Pass **`--include-root-license`** to append the drop’s top-level **`LICENSE`**. The script aggregates **`licenses/`** text only; it does **not** replace a Chromium **`DEPS`** walk for **from-source** PDFium pins. With **`PDFDOCUMENTVIEW_ENABLE_CPACK=ON`**, CMake also defines target **`aggregate-pdfium-third-party-notices`** (requires **Python 3** on `PATH`) writing **`${CMAKE_BINARY_DIR}/THIRD_PARTY_NOTICES.pdfium.md`** when **`licenses/`** exists on the resolved root.

Installs record the backend name for `find_package(PDFDocumentView CONFIG)`; third-party notice placeholders live in **`docs/third-party-notices.template.md`** (also installed under the doc directory when install is enabled).

The **minimum supported upstream PDFium revision for bundled *source-tree* legal text*** lives in `third_party/pdfium/REVISION` and is exposed as **`PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA`** (see `cmake/PDFiumMinimumRevision.cmake`). That SHA is **not required to match** the prebuilt binary revision from **bblanchon/pdfium-binaries**; it is the floor for the vendored `LICENSE` / `AUTHORS` snapshot. Refresh both when policy requires (see `docs/third-party-notices.md`).

**Install / `find_package` (optional):** after `cmake --install` with a prefix, downstream projects can use `find_package(PDFDocumentView CONFIG)` and link **`PDFDocumentView::pdf_document_view`**. When the installed tree was built with **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`** and **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=FULL`**, the package config also defines imported **`PDFDocumentView::PDFium`** (and an alias **`PDFDocumentView::Pdfium`**) pointing at the installed **`libpdfium`** / **`pdfium.dll`**, and propagates **`$<LINK_ONLY:PDFDocumentView::PDFium>`** on the main target so a single **`target_link_libraries(… PDFDocumentView::pdf_document_view)`** pulls in the PDFium link line. With **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`**, **`PDFDOCUMENTVIEW_HAVE_PDFIUM_RUNTIME`** is **`OFF`** and integrators supply PDFium for link/runtime. **Poppler-only** installs omit **`PDFDocumentView::PDFium`**. Set **`PDFDOCUMENTVIEW_INSTALL=OFF`** when embedding via `FetchContent` / `add_subdirectory` if you want to skip install rules and generated package files.

**Shipped PDF engine notices (install tree):** With install on and **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`**, the prefix’s doc dir (GNU **`CMAKE_INSTALL_DOCDIR`**, e.g. `share/doc/<project>/…` on many Unix installs) gains **`pdfdocumentview/third_party/pdfium/`** (vendored minimum-revision **`LICENSE`** / **`AUTHORS`** / **`REVISION`**) and **`pdfdocumentview/third_party/pdfium-prebuilt/`**: **`NOTICES.pdfium-prebuilt.md`** (always — repo-authored index of **`licenses/`** filenames for the default prebuilt tag), plus **`LICENSE.pdfium-binary-drop`** and **`licenses/`** when CMake could read those from the resolved prebuilt layout at configure. See **`docs/shipped-third-party-notices-pdfium-prebuilt.md`** and **`docs/third-party-notices.md`**.

After **`cmake --install`**, open **`…/pdfdocumentview/third_party/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`** first: it points at the installed **`licenses/`** files (when present) without duplicating their text.

### PDFium prebuilt (fetch + `PDFIUM_ROOT`)

When **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`**, CMake loads **`cmake/PdfiumPrebuilt.cmake`**, which resolves PDFium in this order:

1. **`PDFIUM_INCLUDE_DIR`** and **`PDFIUM_LIBRARY`** if both are set (manual / CI cache).
2. Else **`PDFIUM_ROOT`**: expects `include/fpdfview.h` and `lib/libpdfium.{dylib,so}` (or `bin/pdfium.dll` on Windows — layout from [bblanchon/pdfium-binaries](https://github.com/bblanchon/pdfium-binaries) tarballs).
3. Else, if **`PDFDOCUMENTVIEW_FETCH_PDFIUM`** is **ON** (default **on** macOS, Linux glibc, and **Windows**), downloads a **pinned** release asset with **`file(DOWNLOAD … EXPECTED_HASH SHA256=…)`**, extracts under **`${CMAKE_BINARY_DIR}/_deps/pdfium/extracted/…`**, and sets the cache paths above.

Useful cache variables:

| Variable | Role |
|----------|------|
| `PDFDOCUMENTVIEW_FETCH_PDFIUM` | **ON** by default on **macOS**, **Linux (glibc)**, and **Windows**; set **OFF** for air-gapped or policy-restricted builds and supply **`PDFIUM_*`** paths. |
| `PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI` | **`GLIBC`** (default) vs **`MUSL`** when selecting the Linux prebuilt tarball. |
| `PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG` | Release tag for the fetch path (default **`chromium/7834`**). Changing this requires updating **SHA-256** checks in `cmake/PdfiumPrebuilt.cmake`. |
| `PDFDOCUMENTVIEW_CMAKE_ARCH` | If set, **override** host CPU detection **only** for choosing the bblanchon tarball (e.g. `arm64`, `x86_64`, `AMD64`, `ARM64`). Empty = use `CMAKE_(HOST_)SYSTEM_PROCESSOR`. |
| `PDFIUM_ROOT` | Root of an extracted prebuilt tree (`include/` + `lib/`). |
| `PDFIUM_INCLUDE_DIR` / `PDFIUM_LIBRARY` | Explicit header dir and `libpdfium` shared library path. |

**Off-macOS / musl / integrator-only** notes: **`docs/off-mac-pdfium-build.md`**.

**Exact configure line (mac + Qt 6 + default fetch):**

```sh
export QT_PREFIX="$HOME/Qt/6.11.0/macos"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

On Apple Silicon the fetch uses **`pdfium-mac-arm64.tgz`**; on Intel macOS, **`pdfium-mac-x64.tgz`**. On Linux x86_64 / arm64, **`pdfium-linux-x64.tgz`** / **`pdfium-linux-arm64.tgz`** (or **`pdfium-linux-musl-*`** when **`PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI=MUSL`**). The library target **`pdf_document_view`** is built as a **shared** library, links PDFium **privately**, stages **`libpdfium`** next to it at build time, and uses **`$ORIGIN` / `@loader_path`** on the library so the loader resolves PDFium beside **`libpdf_document_view`** without absolute paths in exports. The **basic** demo uses **`BUILD_RPATH`** (and **`INSTALL_RPATH`** when install is enabled) toward the built/installed **`pdf_document_view`** directory so the **`.app`** does not copy **`libpdfium`** into **`Contents/MacOS`** or run **`install_name_tool`** on the demo binary. On **macOS** (and **Linux** when installing the demo), **`INSTALL_RPATH`** also lists Qt’s **`lib/`** directory resolved from **`Qt6_DIR`** at configure time so **`QtWidgets.framework`** (and siblings) load after **`cmake --install`**; moving the install prefix to another machine without that Qt layout requires **`macdeployqt`** (or equivalent) to embed Qt, or rebuilding against a Qt at the destination.

When fetch succeeds, the prebuilt’s top-level **`LICENSE`** is recorded as **`PDFIUM_PREBUILT_LICENSE_FILE`** and may be installed as **`…/third_party/pdfium-prebuilt/LICENSE.pdfium-binary-drop`**; CMake always installs **`…/third_party/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`**; when **`licenses/`** exists on the extracted tree at configure time, CMake also installs **`…/third_party/pdfium-prebuilt/licenses/`**. See **`docs/shipped-third-party-notices-pdfium-prebuilt.md`** alongside the vendored upstream snapshot under **`third_party/pdfium/`** (see **`docs/third-party-notices.md`**).

### Basic demo and PDF path

The demo shows **`PdfDocumentViewWidget`** (library render path for **PDFIUM**). The widget draws each page on **`paperColor()`** over a light margin, supports **scrollbars** when the page is larger than the viewport, **Ctrl+wheel zoom**, and **find-in-document** (PDFium text APIs) with on-page highlights. A **menu bar** maps to the public widget API (integration harness, not a full viewer):

- **File:** `setDocumentPath` — Open…, Close document (empty path), Quit. **`PdfDocumentModel`** (file/buffer open, `pageCount`, `lastError`) backs the widget on PDFium; hosts may also use **`createDefaultPdfDocumentModel()`** without the widget.
- **Navigate:** `nextPage` / `previousPage` / `setCurrentPageIndex` — next, previous, first, last page
- **View:** `setZoom` / `resetZoom` / `fitWidth` — zoom in/out, reset, fit width
- **Search:** `findText` / `findNext` / `findPrevious` / `clearFind` — find dialog, next/previous match, clear highlights
- **Find geometry & navigation (PDFium):** each hit is a **`TextMatch`** (`pageIndex`, **`pageRect`** in **top-down page space**, 1/72 in — same convention as **`pageSize()`** and the render matrix). Use **`findMatches()`**, **`findCurrentIndex()`**, **`currentFindMatch()`**, and **`scrollToFindMatch(int)`** or **`scrollToFindMatch(const TextMatch&)`** (match must be from the current `findMatches()` list) for overlays and scroll-to; **`findResultsChanged(int matchCount, int currentIndex)`** covers match-list updates without a separate rects signal. For **external** index hits, **`revealPageRect(pageIndex, pageRect)`** scrolls that region into view (same page space); **`navigateToHostSearchHit(…, showHighlight)`** optionally applies one **`PageHighlight`** with find-like styling. **`openDocumentAndRevealSearchHit(path, term, pageIndex, pageRect)`** composes file open with that jump and best-effort find alignment when a backend hit matches **`(pageIndex, pageRect)`** exactly.
- **Find styling:** highlight fill and active-match outline color/width are configurable; `findMatches()` returns a **copy** of hits (indices align with `findCurrentIndex()` / `findResultsChanged`).

**Usability (demo + widget)**

- **Scroll:** mouse wheel (when scrollbars are needed), drag scrollbars; with focus on the widget, **arrow keys** / **PgUp** / **PgDn** change pages; **Home** / **End** jump to first / last page.
- **Zoom:** **Ctrl+mouse wheel** and **View** menu actions; match counts and hits are not written to logs.
- **Find:** **Search → Find…**; **Find next** / **Find previous** cycle matches; **Clear highlights** calls `clearFind()`.

Path resolution: explicit **`argv[1]`** when that file exists; otherwise **`demo-synthetic-3page.pdf`** next to the demo executable (**`QCoreApplication::applicationDirPath()`**), staged from **`examples/basic/demo-synthetic-3page.pdf`** at **`POST_BUILD`** (and installed beside the **`.app`** **`MacOS`** binary or next to the **`bin`** executable when **`PDFDOCUMENTVIEW_INSTALL`** is on). Otherwise no document loads until **File → Open**. You may always pass a path:

```sh
./build/examples/basic/pdf_document_view_basic.app/Contents/MacOS/pdf_document_view_basic /path/to/file.pdf
```

Historical: we do not use Qt Pdf in the demo. **`PdfDocumentViewWidget`** is the only PDF surface, so the demo exercises the library's public API rather than a parallel raster path.

The committed **`examples/basic/demo-synthetic-3page.pdf`** is a tiny synthetic three-page placeholder for the basic demo, local runs, and CI; regenerate with **`tools/generate_demo_synthetic_3page_pdf.sh`** when **`gs`** is available. Integrators may substitute their own small PDFs. Other curated test fixtures live under **`tests/fixtures/`** (see **`tests/fixtures/README.md`**).

On macOS the demo is built as **`build/examples/basic/pdf_document_view_basic.app`** (multi-config generators may nest **`Debug/`** — **`make demo-run`** searches common paths).

## MVP → full ship

**Distribution model (read this first):** This project’s **default deliverable** is **source**—integrators build the library inside **their** CMake tree (**`find_package`**, **`add_subdirectory`**, **FetchContent**, or a vendored copy). That path **does not** require this repository to ship **code-signed** or **notarized** **`.dylib` / `.dll`** artifacts; macOS and Windows still **load unsigned libraries** you built locally. Regulated / air-gapped networks use **`PDFDOCUMENTVIEW_FETCH_PDFIUM=OFF`** and a local **`PDFIUM_*`** tree (see **`docs/off-mac-pdfium-build.md`**; CI **`pdfium-root-no-fetch`**). What follows splits **“you ship source”** (most headaches stay with the host app team) from **“you also ship prebuilt binaries or a shrink-wrapped GUI app”** (signing, store rules, and relocatable Qt bundles become real).

### Closed in this repository (library + build + docs)

These used to be called out as gaps; they are now implemented or automated here:

- **CMake consumer + PDFIUM:** installed **`PDFDocumentViewConfig.cmake`** defines imported **`PDFDocumentView::PDFium`** (alias **`PDFDocumentView::Pdfium`**) with **`IMPORTED_LOCATION`** / Windows **`IMPORTED_IMPLIB`** under the install prefix when **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=FULL`**, and appends **`$<LINK_ONLY:PDFDocumentView::PDFium>`** on **`PDFDocumentView::pdf_document_view`** so a single **`target_link_libraries(… PDFDocumentView::pdf_document_view)`** is enough when the PDFIUM runtime component was installed. With **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`**, **`PDFDOCUMENTVIEW_HAVE_PDFIUM_RUNTIME`** is **`OFF`** — integrators wire PDFium themselves. **`PDFDOCUMENTVIEW_HAVE_PDFIUM_RUNTIME`** is set for downstream **`if()`** logic.
- **CPack hermetic PDFIUM archive:** with **`PDFDOCUMENTVIEW_ENABLE_CPACK=ON`** and **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=FULL`**, **`PDFDocumentView_Runtime`** CPack-depends on **`PDFDocumentView_PdfiumRuntime`** so a plain **`cpack -G TGZ`** includes **both** the widget library and shipped **`libpdfium`** / **`pdfium.dll`**. Use **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** to omit shipped PDFium from install + TGZ (integrator supplies PDFium).
- **Demo install / RPATH:** the basic **`.app`** / **`bin`** demo uses **`BUILD_RPATH`** to the built **`pdf_document_view`** target directory (PDFium staged beside it). **`INSTALL_RPATH`** points at **`CMAKE_INSTALL_LIBDIR`** for **`libpdf_document_view`** / PDFium **and** at Qt’s **`lib/`** directory (from **`Qt6_DIR`** at configure time) so **`cmake --install`** runs without **`install_name_tool`** on the demo binary. Relocating the prefix **without** that Qt tree still requires **`macdeployqt`** (or shipping Qt yourself).
- **Poppler REAL parity (find + async tiles):** with **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`**, find uses **`Poppler::Page::search`** (same **`TextMatch::pageRect`** convention as PDFium). **Async off-GUI tiles** use the same **`PdfTileRenderWorker`** queue as PDFium (isolated **`Poppler::Document`** per tile job); **`PdfDocumentViewWidget::asyncTileRenderingSupported()`** exposes when the menu may enable async before the opt-in is on.
- **CI / supply chain:** multi-arch matrix (**macOS** arm64 + Intel, **Linux** amd64 + arm64, **Windows** amd64 + arm64 where runners exist), optional **`pdfium-root-no-fetch`** job (**`PDFDOCUMENTVIEW_FETCH_PDFIUM=OFF`** + cached **`PDFIUM_ROOT`**) plus smoke of **`tools/aggregate_pdfium_third_party_notices.py`**.
- **Third-party notices helper:** **`tools/aggregate_pdfium_third_party_notices.py`** and CMake target **`aggregate-pdfium-third-party-notices`** (when CPack is on and a **`licenses/`** tree is visible) — see **Aggregated PDFium notices** above.
- **Release workflow skeleton:** **`.github/workflows/release.yml`** (**`workflow_dispatch`**) builds and uploads a **`cpack`** artifact (no signing secrets in-repo).

### Still open — only if you ship **prebuilt binaries** or a **consumer app bundle**

If you **only** distribute **source** (or your fork) and downstream teams compile themselves, **you** generally do **not** inherit macOS **notarization** or Windows **Authenticode** obligations **for this library’s object code**—those teams handle signing for **their** executables and installers. You still maintain accurate **docs** (CMake options, ADRs, notices pointers) so their build and legal review are straightforward.

If **you** (or CI) publish **prebuilt** **`libpdf_document_view`** / **`libpdfium`** archives, a **notarized `.app`**, or a **store** submission, then the items below apply to **that** release artifact:

- **Code signing / notarization (macOS)** and **Authenticode / Smart App Control (Windows)** for **downloaded** binaries or quarantined installs.
- **Relocatable GUI apps:** **`macdeployqt`** / **`windeployqt`** (or your installer) to bundle **Qt** when an install tree must run on machines **without** the Qt kit that was on the build host — the **demo** **`INSTALL_RPATH`** still embeds the **Qt `lib/`** path from **`Qt6_DIR`** at configure time unless you bundle Qt into the **`.app`**.
- **Legal depth for a shipped binary line:** full Chromium **`DEPS`** transitive review and product-specific **About** / **`third-party-notices-<release>.md`** (this repo ships indexes + **`aggregate_pdfium_third_party_notices.py`**, not legal counsel). Bounded engineering checklist: **`docs/legal-integration-checklist.md`** (**not** legal advice).
- **Optional product packaging:** **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET`** + optional CPack **`TGZ`** (see **ADR-0003**); separate Poppler-only **prefix** SKUs / signed bundles remain host work.

## Status

**Phase-1 MVP (library):** **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`** on **macOS**, **Linux (glibc)**, and **Windows** resolves **bblanchon/pdfium-binaries** (pinned tag + SHA-256 fetch when enabled) or integrator-supplied **`PDFIUM_*`**; **`PdfDocumentViewWidget`** renders via **`PdfiumBackend`** attached to a **`PdfiumDocumentModel`**. **Poppler-Qt6** real path is optional behind **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`**. CI covers the matrix above.

The **`examples/basic`** binary remains a **menu-driven integration harness** (see **Basic demo and PDF path** above): it exercises open/close, navigation, zoom, tile + async toggles (when supported), find, and host-style highlights on the **library widget API**. Hosts own product chrome, persistence, and **how they ship** (source vs binaries). **Signing, notarization, store compliance, and product-level third-party notices** for **applications** built with this library are **integrator** work unless this repo explicitly publishes signed release binaries (not the default).

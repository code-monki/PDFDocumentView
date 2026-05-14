# PDFDocumentView — concept

## 1. Purpose

This document captures the **initial product concept** for a **reusable, embeddable Qt widget** for displaying PDF pages, driving navigation, and (eventually) supporting **search** and **highlight overlays** with **geometry** (rectangles in page space) suitable for multiple host applications.

The **primary deliverable** is a **library** whose public face is a **`QWidget` (or closely related) subclass** that integrators place in a layout. Supporting types (document handles, find results, optional render workers) are **secondary APIs** that exist to make the widget useful; they are not a separate “viewer application” product.

A **small sample program** (`main.cpp` and a dedicated CMake target) is **in scope** as an **integration and smoke-test demo** only: it should show how to construct the widget, open a file, and exercise documented entry points. It must **not** grow into a second product or a feature-complete desktop viewer unless that becomes an explicit, separate goal.

---

## 2. Relationship to Qt’s built-in PDF widgets

**Intended role:** **`PdfDocumentViewWidget` plus the PDFium-backed render path** is the **deliberate substitute** for **Qt PDF** (`QtPdf` / `QPdfDocument` / `QPdfView` and related types) in host applications that need **richer PDFium-level behavior** than Qt’s PDF module exposes. That is **not** a commitment to match every Qt PDF convenience API one-for-one; it is a **widget + engine** choice under this repository’s control.

Qt provides **high-level PDF types** (`QPdfDocument`, `QPdfView`, Quick PDF types, etc.) that are sufficient for **simple** embedded preview. This component is aimed at cases where **Qt’s surface area is too narrow** for application needs—for example:

- **Scroll-to-rectangle** and **tight sync** between search hits and painting.
- **Control of highlight appearance** (multiple hit kinds, current vs. secondary selection).
- **Optional** continuous or facing-page modes and **tile-style** rendering policies.

This project is **not** seeking to contribute the implementation **into** the Qt Project at this time. Shipping an **independent** library keeps **release cadence**, **API shape**, and **backend choice** under repository control.

**Basic demo note:** historical: we do not use Qt Pdf in the demo. `PdfDocumentViewWidget` is the only PDF surface under `examples/basic`; the **supported integration story** is the widget and its **PDFium** (or future Poppler) backend.

---

## 3. Independent repository and reusable component

**Goals:**

- **One embeddable widget** (and minimal supporting API) consumable from **many** applications without copying source.
- **Clear CMake package** (or equivalent) so downstream projects can `find_package` / `add_subdirectory` / FetchContent as they prefer — **shipped** today (`PDFDocumentViewConfig.cmake`, `PDFDocumentView::pdf_document_view`; see root **README**).
- **Backend pluggability** at the **rendering and text-geometry** layer, not only at the “which `QWidget` to use” layer.
- **Reproducible smoke verification:** **macOS** CI (**.github/workflows/ci.yml**) configures Release, builds library + **`examples/basic`**, runs **CTest** (including PDFium fixture smoke where enabled); see **README** / WBS **`demo-ci`** for scope (e.g. synthetic **`tests/fixtures/minimal-one-page.pdf`**).

**Distribution (source-first):** The **primary** ship vehicle for this component is **source**—integrators obtain the repository (or a fork / tarball), run **CMake** with their **Qt 6** prefix and chosen backend (**PDFIUM** / **Poppler**), and link **`PDFDocumentView::pdf_document_view`** into **their** application. **Code signing**, **notarization**, **`macdeployqt`** / **`windeployqt`**, store submission, and **application-level** third-party notices are **host-product** responsibilities unless this project explicitly publishes **signed prebuilt** library binaries (not the default expectation; see **README** → **MVP → full ship**). A **bounded** checklist of repo vs integrator obligations (still **not** counsel) is **`docs/legal-integration-checklist.md`**.

**Embeddable public API (0.1.x):** headers under `include/pdf_document_view/` and the **`PdfDocumentViewWidget`** contract are **stability-governed** per **`docs/adrs/0004-public-api-surface-freeze.md`** (PATCH = compatible fixes; MINOR = optional additive API; MAJOR + ADR update for breaks).

**Non-goals (initially):**

- Replacing a full **PDF editor** or **print composition** tool.
- Owning **license management** or **DRM**.
- Mandating a single **application shell**; hosts bring their own menus, toolbars, and persistence.

---

## 4. Architectural sketch (Evince-style layering, widget-sized)

The **GNOME Evince** project is a useful **structural** reference: separation between **document model**, **render pipeline**, **text and find**, and **presentation**. Here, “presentation” collapses to **your** **`QWidget`** and optional thin controllers—not shipping Evince itself.

Suggested boundaries:

| Layer              | Responsibility                                                                               |
| ------------------ | -------------------------------------------------------------------------------------------- |
| **Document**       | Open path or buffer, page count, page size cache, error surfacing. Implemented as **`PdfDocumentModel`** + PDFium concrete type in `src/`; the widget delegates **`setDocumentPath`** / **`openDocumentBuffer`** and exposes **`documentModel()`**, **`documentError()`**, and signals **`documentOpened`** / **`documentError`**. |
| **Render service** | Page index + scale (+ optional tile rect) → raster or pixmap; threading policy. **PDFium today:** default **full-page, synchronous** paint; optional **GUI-thread tiles** and optional **async tile worker** (off by default) per **`docs/rendering-threading.md`** and **ADR-0002**. |
| **Text / find**    | Search and **quads** (or equivalent) in **page coordinates** for overlay and scroll-to. **MVP (PDFium):** `TextMatch::pageRect` in top-down page units; widget exposes `findMatches()` / `scrollToFindMatch` for hosts and internal highlights.      |
| **Widget**         | Input, scroll/zoom, coordinate transforms, **paint** document page + **overlay** highlights. |
| **Capabilities**   | Optional flags per backend (forms, annotations, color intents)—avoid pretending parity.      |

**Embeddable widget coordinates:** **`docs/widget-coordinate-contract.md`** spells out **page space** vs **layout and device pixels**, the **viewport** / scroll mapping, how **`PageRenderRequest`** ties to rasterization, host responsibilities (chrome and dialogs), and how **`TextMatch::pageRect`** maps into **`QPainter`** coordinates for custom overlays.

**Render service (threading):** The **default** path remains **full-page, synchronous** rasterization on the **Qt GUI thread** with PDFium work serialized against the shared document model (**`docs/rendering-threading.md`**, **ADR-0002**). **Optional (PDFium and Poppler REAL):** **GUI-thread tile** rendering (`tileRenderingEnabled` + backend **`tileRendering`**) and an **opt-in async tile** path (`asyncTileRenderingEnabled`, default **OFF**) that rasterizes tiles on a **dedicated worker thread** with per-job document isolation — same references. **Poppler stub:** no tiles / no async; capabilities report accordingly. Broader **worker pools**, **shared-document** cross-thread raster, and **continuous-scroll** tile scheduling remain **out of scope** until a future ADR promotes them.

**Document model (file, buffer, errors):** whether integrators call **`PdfDocumentModel`** directly (**`createDefaultPdfDocumentModel()`**, **`openFile`**, **`openBuffer`**, **`close`**, **`pageCount`**, **`lastError`**) or go through **`PdfDocumentViewWidget`** (**`setDocumentPath`**, **`openDocumentBuffer`**, plus **`documentOpened`** / **`documentError`**), the same contract applies: one open document, explicit failure messages, and page geometry only after a successful load.

**Coordinate contract (summary):** prefer **page space** / PDF user units plus an explicit **scale**; the widget maps to **device pixels** for rasterization and back to **layout** coordinates for painting — full detail in **`docs/widget-coordinate-contract.md`**.

---

## 5. Rendering backends and licensing

**Default (locked for now):** **PDFium** — proceed on this basis unless it proves inadequate for required capabilities, then revisit.

**Qt PDF note:** In the split-from-parent workstream, **Qt PDF / QTPDF** looked insufficient versus **direct PDFium** for the rendering and geometry-style surface this widget targets.

Two engines remain in architectural scope:

| Backend | Notes |
|---------|--------|
| **PDFium** | Broad feature surface aligned with Chromium; often BSD-style licensing for the engine itself—**verify** the exact artifact and third-party notices you ship. |
| **Poppler** | Mature text and geometry; widely used (e.g. Evince). Licensing is often **copyleft** for typical builds—**distribution** of combined works must be evaluated carefully. |

**The choice of backend is expected to influence** the **license under which this widget repository is released**, possibly via:

- a **single** default backend per release line, or  
- **split packages** (e.g. core + `pdfdocumentview-pdfium` vs `pdfdocumentview-poppler`) so consumers **opt in** to obligations explicitly.

**Engine choice, license, and third-party notices** for PDFium (and hypotheses for linked binaries) are governed by **`docs/adrs/0001-pdf-engine-license-and-third-party-notices.md`**. **Accepted policy detail** for default PDFium, Poppler opt-in, and split-package / install-shape options lives in **`docs/adrs/0003-split-packages-and-poppler-opt-in.md`**.

This document does **not** provide legal advice; record decisions in the project **decision log** and, when needed, project **ADRs** (0001–0004 as applicable).

---

## 6. macOS / Qt file dialogs (context from sibling work)

On **macOS**, **native** Qt file dialogs can **fail to present** in some launch or activation contexts while still returning an **empty path**—behavior that resembles “cancel” rather than a crash. Host applications may need to ensure **window activation** and, where appropriate, **non-native** dialog options. That concern belongs to **host app** integration and testing; the widget library should **not** hard-code file-picker policy beyond optional **demo** code.

---

## 7. Sample application policy

Shipping a **minimal** `examples/` (or `demo/`) target is **recommended**:

- **Purpose:** integration documentation, CI smoke build, manual sanity check.
- **Scope:** one window, one widget instance, and only actions needed to demonstrate **document open**, **page change**, and any **documented** overlay or find API.
- **Avoid:** corpus-specific logic, branding, or feature growth that duplicates a full **viewer app**.

---

## 8. Source PDF and derived content

Treat **sample PDFs** and **rendered output** as **local** or **fixture** material. Do not commit **copyrighted** source PDFs or **substantive full-text** extracts unless **explicitly** approved for the repository. Prefer **synthetic** or **clearly licensed** tiny fixtures for automated tests.

---

## 9. Next artifacts (when stabilizing)

When the concept stabilizes, align with `ai-toolkit` governance: requirements/architecture artifacts, ADRs for **backend selection** and **public API**, and a **traceability** slice appropriate to an open-source widget (even if lighter than a safety-critical system). A **minimal RTM shell** (requirement IDs, statements, sources, verification placeholders) is started in **`docs/requirements-traceability-matrix.md`**; expand and gate per **`ai-toolkit/02-governance/09-traceability-guardrail.md`**.

---

## Document status

**Steering document** — aligned with the **Phase-1 MVP** in this repository (PDFium default on macOS CI, frozen public API per **ADR-0004**, coordinate contract and ADRs 0001–0003 as referenced above). **`AGENTS.md`** is the authoritative **agent/project contract** (embeddable library vs demo, no upstream-Qt goal, corpus policy); this concept and **ADR-0004** stay aligned on the **0.1.x** frozen headers + **`PdfDocumentViewWidget`** surface only. **Primary distribution:** **source** to integrators; signing/notarization and product-level notices for **host applications** are integrator scope unless the project explicitly ships signed prebuilt binaries (**README**, **MVP → full ship**). Suitable for integrators and maintainers; refine or supersede with a formal SRS if the project needs a heavier requirements baseline.

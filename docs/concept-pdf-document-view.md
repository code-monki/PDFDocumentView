# PDFDocumentView — concept

## 1. Purpose

This document captures the **initial product concept** for a **reusable, embeddable Qt widget** for displaying PDF pages, driving navigation, and (eventually) supporting **search** and **highlight overlays** with **geometry** (rectangles in page space) suitable for multiple host applications.

The **primary deliverable** is a **library** whose public face is a **`QWidget` (or closely related) subclass** that integrators place in a layout. Supporting types (document handles, find results, optional render workers) are **secondary APIs** that exist to make the widget useful; they are not a separate “viewer application” product.

A **small sample program** (`main.cpp` and a dedicated CMake target) is **in scope** as an **integration and smoke-test demo** only: it should show how to construct the widget, open a file, and exercise documented entry points. It must **not** grow into a second product or a feature-complete desktop viewer unless that becomes an explicit, separate goal.

---

## 2. Relationship to Qt’s built-in PDF widgets

Qt provides **high-level PDF types** (`QPdfDocument`, `QPdfView`, Quick PDF types, etc.) that are sufficient for **simple** embedded preview. This component is aimed at cases where **Qt’s surface area is too narrow** for application needs—for example:

- **Scroll-to-rectangle** and **tight sync** between search hits and painting.
- **Control of highlight appearance** (multiple hit kinds, current vs. secondary selection).
- **Optional** continuous or facing-page modes and **tile-style** rendering policies.

This project is **not** seeking to contribute the implementation **into** the Qt Project at this time. Shipping an **independent** library keeps **release cadence**, **API shape**, and **backend choice** under repository control.

---

## 3. Independent repository and reusable component

**Goals:**

- **One embeddable widget** (and minimal supporting API) consumable from **many** applications without copying source.
- **Clear CMake package** (or equivalent) so downstream projects can `find_package` / `add_subdirectory` / FetchContent as they prefer.
- **Backend pluggability** at the **rendering and text-geometry** layer, not only at the “which `QWidget` to use” layer.

**Non-goals (initially):**

- Replacing a full **PDF editor** or **print composition** tool.
- Owning **license management** or **DRM**.
- Mandating a single **application shell**; hosts bring their own menus, toolbars, and persistence.

---

## 4. Architectural sketch (Evince-style layering, widget-sized)

The **GNOME Evince** project is a useful **structural** reference: separation between **document model**, **render pipeline**, **text and find**, and **presentation**. Here, “presentation” collapses to **your** **`QWidget`** and optional thin controllers—not shipping Evince itself.

Suggested boundaries:

| Layer | Responsibility |
|-------|------------------|
| **Document** | Open path or buffer, page count, page size cache, error surfacing. |
| **Render service** | Page index + scale (+ optional tile rect) → raster or pixmap; threading policy. |
| **Text / find** | Search and **quads** (or equivalent) in **page coordinates** for overlay and scroll-to. |
| **Widget** | Input, scroll/zoom, coordinate transforms, **paint** document page + **overlay** highlights. |
| **Capabilities** | Optional flags per backend (forms, annotations, color intents)—avoid pretending parity. |

**Coordinate contract:** internal contracts should prefer **page space** / PDF user units plus an explicit **scale**; the widget maps to **device pixels** for painting and hit-testing.

---

## 5. Rendering backends and licensing

Two candidate engines are under discussion:

| Backend | Notes |
|---------|--------|
| **PDFium** | Broad feature surface aligned with Chromium; often BSD-style licensing for the engine itself—**verify** the exact artifact and third-party notices you ship. |
| **Poppler** | Mature text and geometry; widely used (e.g. Evince). Licensing is often **copyleft** for typical builds—**distribution** of combined works must be evaluated carefully. |

**The choice of backend is expected to influence** the **license under which this widget repository is released**, possibly via:

- a **single** default backend per release line, or  
- **split packages** (e.g. core + `pdfdocumentview-pdfium` vs `pdfdocumentview-poppler`) so consumers **opt in** to obligations explicitly.

This document does **not** provide legal advice; record decisions in the project **decision log** and, when needed, project **ADRs**.

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

When the concept stabilizes, align with `ai-toolkit` governance: requirements/architecture artifacts, ADRs for **backend selection** and **public API**, and a **traceability** slice appropriate to an open-source widget (even if lighter than a safety-critical system).

---

## Document status

**Pre-concept / working notes** — suitable for steering implementation and repo bootstrap; refine or supersede with formal SRS/ADR when the project exits exploration.

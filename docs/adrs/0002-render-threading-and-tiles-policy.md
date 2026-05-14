# ADR-0002: Render threading and tiles policy

## Status

- **v1 render path (full-page, synchronous, GUI-thread paint with document mutex during PDFium render/find):** **Accepted** — matches current **`PdfiumBackend`** + **`PdfiumDocumentModel`** behavior; rationale and Qt threading notes live in **`docs/rendering-threading.md`**.
- **Tile-based rendering and/or background-thread rasterization:** **Hypothesis** for unbounded pools and **shared-document** cross-thread use — **not** adopted; revisit when a future ADR promotes additional threading rules. **Update (MVP):** an **optional** single-thread **`PdfTileRenderWorker`** path exists behind **`PdfDocumentViewWidget::asyncTileRenderingEnabled()`** for **PDFium** and **linked Poppler-Qt6** (see **`docs/rendering-threading.md`** “Async MVP”); the default remains **GUI-thread** tile drain.

---

## Context

The concept document describes a **render service** layer (page index, scale, optional tiles, threading policy). The first shipped integration prioritizes **correctness and a single serialization point** over maximum frame rate on huge pages.

---

## Decision

**Ship v1 as full-page synchronous raster on the paint thread**, with **`PdfiumDocumentModel::m_mutex`** held across PDFium **render** and **find** paths that share the loaded document, until a future ADR promotes tiles and/or async work with explicit affinity, cancellation, and memory rules.

---

## Consequences

- Large documents at high zoom may **stutter** the UI; mitigations are **host-side** (zoom limits, progress UX) until a future tile/async design.
- Documentation and WBS items can point integrators here instead of inferring behavior from implementation alone.

---

## References

- **`docs/rendering-threading.md`** — detailed policy, non-goals, and reconsideration gates.
- **`docs/concept-pdf-document-view.md`** — architectural layering and render row pointer.

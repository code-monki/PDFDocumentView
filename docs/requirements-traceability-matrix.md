# Requirements traceability matrix (RTM)

**Purpose:** Forward and reverse traceability from steering requirements to architecture, design, implementation, and verification. This matrix follows the **minimum column set** described in `ai-toolkit/02-governance/09-traceability-guardrail.md` §3. It is a **governed MVP slice** for this repository: rows are reviewable and bidirectional at the test-ID level, but this is **not** a full lifecycle release gate (no owner/risk columns, no claim that every requirement has passing automated geometric or license audits).

**Steering source:** High-level statements remain aligned with **`docs/concept-pdf-document-view.md`**, **`AGENTS.md`**, and linked ADRs.

**Version (RTM artifact):** `RTM-0.1` — **Last updated:** `2026-05-13` (ISO 8601 date).

## Architectural component IDs

| ID | Component |
|----|-----------|
| **ARCH-001** | Embeddable widget (`PdfDocumentViewWidget` and paint/scroll/find wiring) |
| **ARCH-002** | Document abstraction (`PdfDocumentModel`, `createDefaultPdfDocumentModel`) |
| **ARCH-003** | Render backend (`PdfRenderBackend`, `PdfiumBackend`, capabilities) |
| **ARCH-004** | Public API / headers (`include/pdf_document_view/*.hpp`) |
| **ARCH-005** | Integration harness (`examples/basic/`, not a product viewer) |
| **ARCH-006** | Build, install, notices, CI (CMake, `third_party/`, GitHub Actions) |

## Forward traceability

| Req ID | Requirement (short) | Arch component ID(s) | Design ref(s) | Implementation ref(s) | Test case ID(s) | Validation status | Version | Last updated |
|--------|----------------------|----------------------|---------------|-------------------------|-----------------|---------------------|---------|--------------|
| FR-001 | Reusable **embeddable Qt widget** (and minimal API) for PDF display, navigation, and host-driven extension (find, highlights); not a standalone viewer product. | ARCH-001, ARCH-002, ARCH-003 | `docs/concept-pdf-document-view.md` §1, §3; `AGENTS.md` | `include/pdf_document_view/pdf_document_view_widget.hpp`; `include/pdf_document_view/pdf_document_model.hpp`; `include/pdf_document_view/pdf_capabilities.hpp`; `src/pdf_document_view_widget.cpp`; `src/pdfium_backend.cpp` | `pdf_document_model_smoke`; `pdf_document_fixture_smoke` | Partial — CTest smoke; demo behavior manual | RTM-0.1 | 2026-05-13 |
| FR-002 | **Stability-governed public API** (headers under `include/pdf_document_view/`, semver per ADR-0004). | ARCH-004 | `docs/concept-pdf-document-view.md` §3; `docs/adrs/0004-public-api-surface-freeze.md` | `include/pdf_document_view/*.hpp` | `pdf_document_model_smoke`; `pdf_document_fixture_smoke` | Partial — smokes exercise public entry points only | RTM-0.1 | 2026-05-13 |
| FR-003 | **Page vs layout vs device** coordinate semantics for rendering, find geometry, and overlays. | ARCH-001, ARCH-004 | `docs/concept-pdf-document-view.md` §4; `docs/widget-coordinate-contract.md` | `include/pdf_document_view/pdf_document_view_widget.hpp`; `include/pdf_document_view/page_render_request.hpp`; `include/pdf_document_view/text_match.hpp`; `include/pdf_document_view/page_highlight.hpp`; `src/pdf_document_view_widget.cpp` | `pdf_document_fixture_smoke` | Partial — contract is doc-led; smokes do not assert geometry | RTM-0.1 | 2026-05-13 |
| FR-004 | **PDFium** default engine; **capabilities** reported honestly (incl. Poppler stub limits). | ARCH-002, ARCH-003 | `docs/concept-pdf-document-view.md` §5; `docs/adrs/0001-pdf-engine-license-and-third-party-notices.md`; `docs/adrs/0003-split-packages-and-poppler-opt-in.md`; `docs/rendering-threading.md`; `docs/adrs/0002-render-threading-and-tiles-policy.md` | `src/pdfium_backend.cpp`; `include/pdf_document_view/pdf_capabilities.hpp`; `CMakeLists.txt` (backend option) | `pdf_document_model_smoke`; `pdf_document_fixture_smoke` | Partial — PDFium path in CI; Poppler stub not parity-tested here | RTM-0.1 | 2026-05-13 |
| FR-005 | **Minimal integration demo** only (`examples/basic`): construct widget, open document, exercise documented entry points without becoming a full viewer. | ARCH-005 | `docs/concept-pdf-document-view.md` §1, §7; `AGENTS.md` §4 | `examples/basic/main.cpp` | — (no dedicated CTest; **CI** builds `examples/basic`) | Partial — build verified in `.github/workflows/ci.yml`; UX/API coverage manual | RTM-0.1 | 2026-05-13 |
| NFR-001 | **Third-party licensing and notices** for PDFium (and optional prebuilt): install-time doc layout, aggregate notices, integrator obligations remain honest. | ARCH-006 | `docs/adrs/0001-pdf-engine-license-and-third-party-notices.md`; `docs/adrs/0003-split-packages-and-poppler-opt-in.md`; `docs/third-party-notices.md`; `docs/shipped-third-party-notices-pdfium-prebuilt.md` | `CMakeLists.txt`; `cmake/` (e.g. `PdfiumPrebuilt.cmake`, `PDFiumMinimumRevision.cmake`); `third_party/pdfium/`; `docs/notices/`; `docs/about/` templates as referenced from ADR-0001 | — | Manual / policy — no automated license scan in repo | RTM-0.1 | 2026-05-13 |
| NFR-002 | **Reproducible CI**: configure Release, build library + examples, run **CTest** on a pinned matrix. | ARCH-006 | `docs/concept-pdf-document-view.md` §3 (CI); `AGENTS.md` | `.github/workflows/ci.yml`; `tests/CMakeLists.txt`; `tests/pdf_document_model_smoke.cpp`; `tests/pdf_document_fixture_smoke.cpp` | `pdf_document_model_smoke`; `pdf_document_fixture_smoke` (PDFium configurations) | Partial — **macOS-only** matrix today (PDFium fetch policy); reproducible within that slice | RTM-0.1 | 2026-05-13 |

## Reverse traceability (test case ID → FR IDs)

CTest **names** are the canonical test case IDs.

| Test case ID | FR ID(s) |
|--------------|----------|
| `pdf_document_model_smoke` | FR-001, FR-002, FR-004 |
| `pdf_document_fixture_smoke` | FR-001, FR-002, FR-003, FR-004 |

The same CTest runs also support **NFR-002** (CI + CTest reproducibility on the configured matrix). **NFR-001** has no dedicated CTest; validation is install-tree / counsel review per ADR-0001 and related docs. **FR-005** is evidenced by the **examples** target built in CI (see `.github/workflows/ci.yml`), not by a named CTest.

## Maintainer note

Expand **Version** / **Last updated** when rows change. For stricter gates (full §4–§6 obligations in `ai-toolkit/02-governance/09-traceability-guardrail.md`), add owner, risk, and release-target columns and tie **FR-005** / **NFR-001** to explicit test or audit IDs.

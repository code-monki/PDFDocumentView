# ADR-0003: Split packages, Poppler opt-in, and default engine packaging

## Status

**Accepted** — **policy and packaging intent** for how hosts consume **PDFium** (default) versus an optional **Poppler**-linked flavor. **Source-first default:** this repository’s **baseline** deliverable is **source**; **binary** installers, signed frameworks, store SKUs, and **complete** third-party notice passes remain **integrator / host-product** scope unless the project explicitly publishes signed prebuilts (see **`README.md` → MVP → full ship**, **`docs/concept-pdf-document-view.md`**). **Implementation (2026-05-13):** CMake install rules tag **`PDFDocumentView_Runtime`**, **`PDFDocumentView_PdfiumRuntime`** (shipped PDFium shared library only, **PDFIUM** builds), **`PDFDocumentView_Development`**, and **`PDFDocumentView_Documentation`**. **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET`** (**`FULL`** default, or **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** on **PDFIUM**) omits **`PdfiumRuntime`** from **`cmake --install`** and from CPack **`TGZ`** when set, and sets installed **`PDFDOCUMENTVIEW_HAVE_PDFIUM_RUNTIME=OFF`** so **`PDFDocumentView::PDFium`** is not exported for a prefix without shipped PDFium. **Poppler:** optional **Poppler-Qt6** in-process path behind **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`** (`pkg-config poppler-qt6`); default **`POPPLER`** build remains **stub** when REAL is **OFF**. **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** is a **no-op** for **POPPLER** builds (no shipped PDFium component). Optional **`PDFDOCUMENTVIEW_ENABLE_CPACK`** wires **`TGZ`** (see **`CMakeLists.txt`**, **`cmake/PDFDocumentViewCPack.cmake`**, preset **`cpack-without-shipped-pdfium`**). Split-prefix-only Poppler packages / signed bundles remain product/host work.

This document is **not legal advice**; it is an engineering and governance record only. Bounded integrator checklist (still not counsel): **`docs/legal-integration-checklist.md`**.

---

## Context

- The widget library supports **pluggable** PDF render backends at the implementation boundary (`docs/concept-pdf-document-view.md`, `AGENTS.md`).
- **ADR-0001** locks **PDFium** as the **default** engine for implementation and documents license / third-party notice obligations for PDFium artifacts and hosts.
- **Poppler** is retained as an **architectural alternate** for integrators who **choose** a GPL-class engine path and accept the corresponding **distribution and source** obligations for **their** product (exact terms depend on the Poppler revision, Qt edition, static vs dynamic linking, and what is shipped — see ADR-0001’s Poppler row and upstream **`COPYING`** at the pin you use).
- Host applications **own** shipping **matching** notices, **`About`** / product notice files, and counsel review where needed; the repo installs **templates** and **PDFium-path** doc slices when install is on — not a complete downstream stack audit.

---

## Decision

1. **Default engine:** **PDFium** — same as ADR-0001 and `PDFDOCUMENTVIEW_RENDER_BACKEND` default **`PDFIUM`** in root `CMakeLists.txt`.
2. **Poppler path:** **Opt-in** — integrators select a **Poppler** build flavor only when they **intend** to accept **GPL-style** (or otherwise copyleft) obligations for **in-process** linkage and combined works as their counsel defines them. The repository may ship a **separately named** consumable (library target, CMake package suffix, or **separate install prefix**) so permissive hosts do not accidentally pull a Poppler-linked binary into the same deliverable as a proprietary app without an explicit decision.
3. **Single configure, single backend:** **`PDFDOCUMENTVIEW_RENDER_BACKEND=POPPLER`** yields a **stub** when **`PDFDOCUMENTVIEW_POPPLER_REAL=OFF`**, or a **linked Poppler-Qt6** backend when **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`**; **`PDFIUM`** remains the other exclusive choice at configure time. Anything marketed as “Poppler-enabled” must be honest about **stub vs linked** state in release notes and CMake package metadata.

---

## Split package options

The project may ship **one** combined package or **split** packages. The table below is **concrete** guidance; not all rows need to be implemented in the first release.

| Approach | What gets installed | Integrator experience |
|----------|---------------------|------------------------|
| **A — Single prefix, single binary** (current direction) | One `libpdf_document_view` (or platform equivalent) built for **either** PDFium **or** Poppler at configure time; headers + **`PDFDocumentViewConfig.cmake`** + **`PDFDocumentViewTargets.cmake`** under one **`CMAKE_INSTALL_PREFIX`**. | Consumer **`find_package(PDFDocumentView CONFIG)`** reads exported **`PDFDOCUMENTVIEW_RENDER_BACKEND`** (see ADR-0001). Host must not assume Poppler exists unless their **vendor** built and labeled a Poppler flavor. |
| **B — Single prefix, CMake `COMPONENT`s** (partial) | **`PDFDocumentView_Runtime`** — `pdf_document_view`; **`PDFDocumentView_PdfiumRuntime`** — shipped **`libpdfium`** / **`pdfium.dll`** (**PDFIUM** only, omitted when **`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET=RUNTIME_WITHOUT_SHIPPED_PDFIUM`**); **`PDFDocumentView_Development`** — headers + exported CMake files; **`PDFDocumentView_Documentation`** — **`NOTICES`**, templates, **`LICENSE`** trees under `${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/…`. | `cmake --install … --component …` for split artifacts. **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** + **`PDFDOCUMENTVIEW_ENABLE_CPACK`**: **`cpack -G TGZ`** omits **`PdfiumRuntime`** (integrator supplies PDFium). **Poppler REAL** builds ship **`Runtime`** + Poppler-linked **`pdf_document_view`** only (Poppler shared libs come from the **system** / your redistributor pipeline, not this repo). A future **`PDFDocumentView_PopplerRuntime`** component could isolate GPL-linked runtime files if we ever vendor them. |
| **C — Split prefixes / split packages** | **`PDFDocumentView`** core: headers + PDFium-linked (or header-only glue) artifact under prefix **P1**. Optional **`PDFDocumentView-Poppler`** (or **`pdfdocumentview-poppler`**): second prefix **P2** or separate **CPack** / distro package that contains **only** the Poppler-linked library and its notice set. | Proprietary app links **P1** only; GPL-aware app or distro maintainer adds **P2**. Avoids a single tarball that **mixes** two mutually exclusive linked engines without the integrator noticing. |
| **D — Out-of-process / plugin** (architectural, not mandated) | Core widget stays permissive-friendly; **optional** helper binary or **Qt plugin** loads Poppler **outside** the proprietary address space — packaging and compliance are **host + counsel** problems, but the **split** is explicit at the process boundary. | Same as C for “optional add-on,” with stronger isolation; higher engineering cost. |

**Naming:** Public CMake target namespace stays **`PDFDocumentView::`**; a Poppler-specific export might use a **distinct** config file (e.g. **`PDFDocumentViewPopplerConfig.cmake`**) or a **cache variable** guarded target alias — exact CMake names are **implementation details** once split packages land; this ADR only requires that **PDFium-default** and **Poppler-opt-in** artifacts be **discoverably different** in **package name, prefix, or component**, not silently identical.

---

## Consequences

- **Hosts must ship matching notices** for whatever they link and redistribute: PDFium paths per ADR-0001; Poppler **`COPYING`** / build-time obligations per upstream and their stack.
- **No maintainer guarantee** that a **proprietary** application can **legally** ship **in-process** Poppler-linked **`pdf_document_view`**; that is a **product and license architecture** decision (Qt edition, static vs dynamic, combined work). The library documents **capabilities** and **build flags**; **compliance** remains with the **integrator** and **counsel**.
- **Split packages** reduce accidental misuse (wrong engine in the wrong product) at the cost of **more** packaging, CI matrix, and documentation surface.
- **Stub Poppler** builds must be clearly labeled so downstream does not assume text search or render parity.

---

## Links

- **ADR-0001** — PDF engine license posture, default PDFium, Poppler options summary, notices checklist: [`0001-pdf-engine-license-and-third-party-notices.md`](0001-pdf-engine-license-and-third-party-notices.md)
- Concept — backends and split-package idea: [`../concept-pdf-document-view.md`](../concept-pdf-document-view.md)
- Integrator scope checklist (not counsel): [`../legal-integration-checklist.md`](../legal-integration-checklist.md)

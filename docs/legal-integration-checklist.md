# Legal integration checklist (bounded, engineering-only)

**Not legal advice.** This document **scopes** what this repository provides versus what **host products** must own when they ship **compiled artifacts**. It does **not** claim completeness for any jurisdiction, license stack, or distribution channel. **Counsel** reviews **binary** distribution lines.

---

## 1. What this repository already ships (when install is on)

When **`PDFDOCUMENTVIEW_INSTALL=ON`** and **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`**, CMake can install, under the prefix’s documentation tree, a **maintainer-authored index** of prebuilt **`licenses/`** filenames (**`NOTICES.pdfium-prebuilt.md`**), optional verbatim **`licenses/`** from the **resolved** prebuilt tree at configure time, optional **`LICENSE.pdfium-binary-drop`**, and the **minimum-revision** upstream snapshot under **`third_party/pdfium/`** (**`LICENSE`**, **`AUTHORS`**, **`REVISION`**) — see **`README.md`** (install / shipped notices), **`docs/third-party-notices.md`**, and **ADR-0001**.

Optional tooling:

- **`tools/aggregate_pdfium_third_party_notices.py`** — merges **`docs/about/THIRD_PARTY_NOTICES.template.md`** with verbatim **`licenses/*`** from a given **`PDFIUM_ROOT`** (or fetch extract layout). It does **not** replace a Chromium **`DEPS`** review for **from-source** PDFium pins; it only aggregates what is present under that root’s **`licenses/`** tree.
- CMake target **`aggregate-pdfium-third-party-notices`** when **CPack** is enabled and a **`licenses/`** directory exists on the resolved PDFium root (**`README.md`**).

These artifacts support **honest attribution** and **engineering traceability**; they are **not** a substitute for product-level legal review.

---

## 2. Source-only distribution path

If you **only** distribute **source** (or your fork) and downstream teams compile and link PDFium themselves:

- Follow **`README.md`**, **ADR-0001** (engine / notices posture), and **ADR-0003** (optional split packaging).
- This repository does **not** become responsible for **their** compiled binary, installer, or store submission.
- **Host legal / release engineering** owns notices and compliance for **whatever they compile and ship**.

See **`README.md` → MVP → full ship** and **`docs/concept-pdf-document-view.md`** (source-first model).

---

## 3. What integrators must own for a **binary** product

When **you** (or CI) publish **prebuilt** libraries, installers, or app bundles that include **linked PDFium** (or a Poppler-linked build), **your** organization typically must plan for at least:

| Area | Integrator responsibility (summary) |
|------|--------------------------------------|
| **PDFium revision** | Match **your** pinned PDFium / prebuilt to **your** notices and verification story. For **from-source** Chromium/PDFium builds, walk **`DEPS`** (and feature flags) for **transitive** third-party text — not replaced by **`aggregate_pdfium_third_party_notices.py`** alone. |
| **Runtime layout** | Ship **matching** shared libraries (PDFium, Qt, libc++, etc.) and loader paths consistent with how **`pdf_document_view`** was built. |
| **Product notices** | Maintain a versioned **`third-party-notices-<product>-<version>.md`** (or equivalent) and host **About** / in-app attribution appropriate to **your** SKU. Extend **`docs/third-party-notices.template.md`** / **`docs/about/THIRD_PARTY_NOTICES.template.md`** patterns — do not treat repo indexes as exhaustive. |
| **Poppler (GPL-class) builds** | If **`PDFDOCUMENTVIEW_RENDER_BACKEND=POPPLER`** with **`PDFDOCUMENTVIEW_POPPLER_REAL=ON`**, **you** own Poppler **`COPYING`**, source-offer / copyleft obligations for **your** distribution shape (static vs dynamic, combined work), and counsel sign-off. |

---

## 4. Optional packaging knobs (CMake)

**`PDFDOCUMENTVIEW_PACKAGE_COMPONENT_SET`** (**`FULL`** default, or **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** on **PDFIUM** builds) controls whether the **shipped** **`libpdfium`** / **`pdfium.dll`** is part of **`cmake --install`** and CPack **TGZ** — see **`README.md`**, **`docs/adrs/0003-split-packages-and-poppler-opt-in.md`**, and **`cmake/PDFDocumentViewCPack.cmake`**. **Poppler** backends have **no** **`PdfiumRuntime`** component; the setting is documented as a no-op there.

When **`RUNTIME_WITHOUT_SHIPPED_PDFIUM`** is used on **PDFIUM**, the installed **`PDFDocumentViewConfig.cmake`** sets **`PDFDOCUMENTVIEW_HAVE_PDFIUM_RUNTIME=OFF`** and omits imported **`PDFDocumentView::PDFium`** — integrators supply PDFium for **both** link and runtime layout.

---

## 5. Honest limits

- No warranty that any **default prebuilt** tag satisfies **your** product’s license, export, or store policies.
- **Synthetic** or **fixture** PDFs in this repo do not grant rights to **your** users’ documents.
- **GPL / LGPL** questions for Poppler-linked **combined works** are **outside** this checklist; use counsel.

---

## See also

- **`README.md`** — MVP → full ship, CPack, aggregate script.
- **`docs/concept-pdf-document-view.md`** — product boundaries, source-first distribution.
- **`docs/third-party-notices.md`** — PDFium / prebuilt record (partial).
- **`docs/adrs/0001-pdf-engine-license-and-third-party-notices.md`**, **`docs/adrs/0003-split-packages-and-poppler-opt-in.md`**.

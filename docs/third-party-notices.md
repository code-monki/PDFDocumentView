# Third-party notices (working record)

**Status:** **Partial.** For **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`** on **macOS**, CMake links **`libpdfium.dylib`** through imported target **`PDFDocumentView::PDFium`** (`cmake/PdfiumPrebuilt.cmake`: fetch with pinned checksums, or **`PDFIUM_ROOT` / `PDFIUM_INCLUDE_DIR`+`PDFIUM_LIBRARY`**). The **minimum** upstream PDFium Git SHA for **in-tree legal text** remains **`third_party/pdfium/REVISION`** / **`PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA`**; it need **not** match the prebuilt Chromium milestone. Default **bblanchon** tarballs ship root **`LICENSE`** and **`licenses/`**; when install is on, CMake installs the vendored snapshot under **`…/third_party/pdfium/`**, **`LICENSE.pdfium-binary-drop`** when the resolved layout has tarball **`LICENSE`**, the repo-authored aggregate index **`…/third_party/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`** (source **`docs/notices/pdfium-prebuilt-NOTICES.md`**), and—if **`licenses/`** exists on the resolved prebuilt root at configure time—the **`licenses/`** tree under **`…/third_party/pdfium-prebuilt/licenses/`** (see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**). This file remains an engineering record; release engineering still extends **`docs/third-party-notices.template.md`** into **`third-party-notices-<version>.md`** for **binary** distribution lines, and host **About** copy remains the integrator’s product obligation (see **`docs/about/THIRD_PARTY_NOTICES.template.md`**). If you **only ship source** and downstream teams compile and link PDFium themselves, **their** build and ship-time notices process applies; this repository still documents what **CMake** installs when **`PDFDOCUMENTVIEW_INSTALL`** is on so integrators can mirror it (**README** → **MVP → full ship**).

**Not legal advice.** Engineering and governance record only.

---

## Linked PDFium binary (macOS MVP)

| Field | Value |
|-------|-------|
| Dylib | **`libpdfium.dylib`** — default source: [bblanchon/pdfium-binaries](https://github.com/bblanchon/pdfium-binaries) release tag **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`**, asset **`pdfium-mac-arm64.tgz`** or **`pdfium-mac-x64.tgz`**. |
| Runtime (basic demo) | **`BUILD_RPATH`** / **`INSTALL_RPATH`** resolve **`libpdf_document_view`** and **`libpdfium`** from the build tree / install **`lib/`** (see **README**, **PDFium prebuilt**); Qt is resolved separately (**`INSTALL_RPATH`** includes Qt’s **`lib/`** from **`Qt6_DIR`** when installing the demo). |
| License bundle for the **binary drop** | Tarball **`LICENSE`** (installed as **`LICENSE.pdfium-binary-drop`** when **`PDFIUM_PREBUILT_LICENSE_FILE`** is set); see also tarball **`licenses/`** for bundled components (ICU, zlib, etc.). |

### Linked macOS prebuilt (bblanchon)

| Topic | Location / policy |
|-------|-------------------|
| CMake resolution | **`cmake/PdfiumPrebuilt.cmake`**: **`PDFIUM_INCLUDE_DIR`+`PDFIUM_LIBRARY`**, else **`PDFIUM_ROOT`**, else macOS fetch → **`PDFIUM_FETCH_EXTRACT_ROOT`** (internal) + **`PDFDocumentView::PDFium`** imported shared library. |
| After **`cmake --install`** (install on, backend PDFIUM) | **`${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt/LICENSE.pdfium-binary-drop`** from tarball **`LICENSE`** when **`PDFIUM_PREBUILT_LICENSE_FILE`** is set. |
| Aggregate index | **`${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`** — table of **`licenses/`** filenames for the default **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`** (maintainer inventory; not a substitute for installed **`licenses/`** text). |
| Tarball **`licenses/`** | When present on **`PDFIUM_FETCH_EXTRACT_ROOT`** or **`PDFIUM_ROOT`** at configure: **`${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt/licenses/`** (per-file contents identical to the upstream tarball; not duplicated in `docs/`). |
| Minimal / custom **`PDFIUM_ROOT`** | If **`licenses/`** is absent at configure, CMake skips that install rule; copy the matching release’s **`licenses/`** at ship time — see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**. |

---

## PDFium (minimum pinned snapshot + in-tree legal text)

| Field | Value |
|-------|-------|
| Project | **PDFium** — open-source PDF library originated and maintained as part of the Chromium project. |
| Homepage | `https://pdfium.googlesource.com/pdfium/` (canonical) / `https://github.com/chromium/pdfium` (GitHub mirror used for documentation links in this repo). |
| Source of truth for license **text at this minimum snapshot** | **In-tree:** `third_party/pdfium/LICENSE` and `third_party/pdfium/AUTHORS` (verbatim upstream at the SHA in `third_party/pdfium/REVISION`). **Online mirror (same commit):** `https://github.com/chromium/pdfium/blob/<SHA>/LICENSE` with `<SHA>` from `REVISION`. Confirm any per-file headers in a future **vendored source or SDK** artifact separately from this documentation snapshot. |
| Minimum supported upstream revision for bundled notices | `a84323421e94f484faca52dd9d027934eba42ab8` (recorded in `third_party/pdfium/REVISION`; matches reference verification in `docs/engine-verification.md`; exposed by CMake as `PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA` via `cmake/PDFiumMinimumRevision.cmake`). **Semantics:** newer upstream revisions are **allowed** for builds that obtain PDFium sources elsewhere; this row records the **minimum** revision the bundled legal text has been verified against, not an equality requirement on a linked engine. |
| `NOTICE` / `COPYING` at this minimum revision | **`NOTICE`** and **`COPYING`** are **not** present at this SHA on the GitHub raw host (HTTP 404); see `third_party/pdfium/README.md`. Re-check on every revision bump. |
| SPDX identifiers (approximate; confirm at pin) | The composite upstream `LICENSE` combines a **`BSD-3-Clause`-style** PDFium-authors block with the full **`Apache-2.0`** text. Distributions commonly express PDFium licensing for that composite file as `BSD-3-Clause AND Apache-2.0`; do **not** rely on a single shorthand. |
| Obligations summary (at-a-glance, **not legal advice**) | The PDFium-authors BSD-style block requires preserving the copyright notice, the conditions list, and the disclaimer in source and binary redistributions, and forbids using contributor names (including Google) to endorse derived products without permission. The Apache-2.0 block requires giving recipients a copy of the License, marking modified files, retaining attribution / patent / trademark notices, and — **if upstream provides a `NOTICE` file at the pinned revision** — including its contents in a `NOTICE` file, in source documentation, or in a runtime third-party-notices display. The Apache-2.0 patent grant terminates on patent litigation against contributors. |
| What we must copy into shipping NOTICES **when linking or shipping PDFium** | (1) The verbatim `LICENSE` for the **revision the build actually links** (the in-tree `third_party/pdfium/LICENSE` discharges the **minimum** snapshot; refresh it if the build pin advances past the minimum and structure changes). (2) **`AUTHORS`** when present upstream (vendored as `third_party/pdfium/AUTHORS`). (3) Per-component license text for **every transitive third-party dependency** PDFium pulls in for **your** build (`DEPS`, optional features, prebuilt SDK contents). (4) An attribution line / About-box entry naming PDFium and pointing back to the upstream project. (5) For host applications: placement where end users normally find third-party notices. |
| Repository library SPDX | The widget library source itself uses the SPDX identifier in the root `LICENSE` file; bundling or linking PDFium does **not** change this repo's own SPDX, only the **combined** distribution's obligations. |

### Open when shipping linked PDFium broadly

| Item | Owner | Status |
|------|-------|--------|
| Align **build** revision (prebuilt SDK / system package) with policy for **minimum** in `third_party/pdfium/REVISION`, or bump the vendored snapshot when the binary drop advances materially | Build / Release | **Partial** — macOS prebuilt wired; revision may differ from `REVISION` |
| Append a **build-pin** verification row in `docs/engine-verification.md` and extend ADR-0001’s verification table | Maintainers | **Partial** — build pin + install behavior recorded; ADR table row exists; full legal NOTICES pass still open |
| Generate `third-party-notices-<version>.md` from `docs/third-party-notices.template.md` covering **transitive** deps and the **actual** linked stack | Release | Open |
| Walk transitive deps from PDFium's **`DEPS`** at a **from-source** Chromium/PDFium pin | Release / Legal | **Out of scope** for the **bblanchon prebuilt** path — use tarball **`licenses/`** for the binary drop; **`DEPS`** applies when you vendor/build PDFium source yourself (see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**) |
| Confirm root `LICENSE` SPDX alignment with the chosen distribution shape (single combined notice or split packages) | Maintainers | Open |

---

## Poppler (alternate, not verified here)

Documented in ADR-0001 as an alternate backend behind `PDFDOCUMENTVIEW_RENDER_BACKEND=POPPLER`. **No verification record exists yet** in `docs/engine-verification.md` for Poppler; add one when integration work begins or before any Poppler-linked artifact ships. Source of truth for license: the upstream `COPYING` file at the pinned revision (`https://gitlab.freedesktop.org/poppler/poppler/-/blob/master/COPYING`, confirm on the tag used).

---

## Qt and toolchain

Qt redistributables and toolchain runtimes are independent of the PDF engine and follow the Qt license agreement and platform rules in force for the integrator's product. Not tracked here beyond this pointer.

---

See also: `docs/adrs/0001-pdf-engine-license-and-third-party-notices.md`, `docs/engine-verification.md`, `docs/third-party-notices.template.md`, `docs/about/THIRD_PARTY_NOTICES.template.md`, `docs/notices/pdfium-prebuilt-NOTICES.md`, `docs/shipped-third-party-notices-pdfium-prebuilt.md`, `third_party/pdfium/README.md`, `docs/legal-integration-checklist.md` (repo vs integrator scope; not legal advice).

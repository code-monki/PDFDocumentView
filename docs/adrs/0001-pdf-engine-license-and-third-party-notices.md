# ADR-0001: PDF engine license posture and third-party notices

## Status

**Split status (as of 2026-05-13):**

- **Default render engine:** **Accepted** — **PDFium** is the **locked default** for implementation planning and spikes unless evaluation shows it inadequate (then revisit per `docs/concept-pdf-document-view.md`).
- **Vendored upstream legal text for the documented PDFium minimum snapshot (narrow):** **Accepted** — for commit `a84323421e94f484faca52dd9d027934eba42ab8`, verbatim upstream **`LICENSE`** and **`AUTHORS`** are stored under **`third_party/pdfium/`** (`REVISION` matches that SHA). The recorded SHA is the **minimum supported upstream revision** for the **bundled** legal text — not an exact-only forever pin; newer upstream revisions are allowed for builds that obtain PDFium sources elsewhere, and downstream tooling can consult the cache variable **`PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA`** (defaulted from `REVISION` by `cmake/PDFiumMinimumRevision.cmake`, overridable at configure time). At this revision, **`NOTICE`** and **`COPYING`** do not exist on the GitHub raw URLs (HTTP 404); nothing is claimed for those filenames. When **`PDFDOCUMENTVIEW_INSTALL`** is **ON** and **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`**, CMake **`install(FILES …)`** places those files under **`${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium`**. This acceptance **does not** assert that a **compiled** PDFium library matches that exact Git SHA — only that the **documented text bundle** for compliance traceability is present.
- **macOS linked engine (bblanchon prebuilt, MVP):** **Accepted (narrow)** — CMake defines imported **`PDFDocumentView::PDFium`** and links **`pdf_document_view`** to **`libpdfium.dylib`** from **`cmake/PdfiumPrebuilt.cmake`** (pinned **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`** + SHA-256 verified download on macOS default path, or integrator **`PDFIUM_ROOT` / `PDFIUM_*`**). When **`PDFDOCUMENTVIEW_INSTALL`** is **ON**, CMake always installs **`${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`** (repo **`docs/notices/pdfium-prebuilt-NOTICES.md`** — filename-level index for the default tag, **no** paraphrased license bodies). When **`PDFIUM_PREBUILT_LICENSE_FILE`** is set, CMake installs tarball **`LICENSE`** as **`LICENSE.pdfium-binary-drop`**; when a **`licenses/`** directory exists on **`PDFIUM_FETCH_EXTRACT_ROOT`** or **`PDFIUM_ROOT`** at configure time, **`install(DIRECTORY …)`** copies it under **`…/pdfium-prebuilt/licenses/`** (see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**). **Residual risk (explicit):** integrator **`PDFIUM_INCLUDE_DIR`+`PDFIUM_LIBRARY`** without a layout root does **not** auto-install upstream **`licenses/`** or root **`LICENSE`**; **Windows/Linux** default fetch is off; **`find_package`** may embed absolute paths to **`PDFDocumentView::PDFium`**; a **full Chromium `DEPS` source walk** is **out of scope** for this prebuilt path (policy: use tarball **`licenses/`** for the binary drop). Legal review of every upstream file, parity with **source** `DEPS`, and host **About** / product NOTICES completeness remain **Hypothesis** (see next bullet).
- **License and third-party notices posture for shipped binaries / linked engine (general):** **Hypothesis** for the **host product** until: (1) the integrator’s distribution includes all license texts required for **their** stack (Qt edition, other libs, static vs dynamic PDFium, custom PDFium paths without auto **`licenses/`**); (2) the project’s own SPDX / `LICENSE` for the widget library is aligned with the distribution shape; and (3) a maintainer or counsel has signed off on **About** / NOTICES for the **resulting** binary or SDK layout where needed. **Repo-codified slice (this pass):** aggregate **`NOTICES.pdfium-prebuilt.md`**, optional tarball **`licenses/`** + **`LICENSE.pdfium-binary-drop`** when configure-time layout allows, and documented **`DEPS`** vs **binary-drop** scope — see **`docs/third-party-notices.md`**. **No claim** that independent legal review of every downstream linked stack is complete.

**Promotion of the binary / linked-engine hypothesis toward fully Accepted** follows the gates in **Status** once transitive deps, install portability, and a NOTICES pass are verified for the **actual shipping artifact**.

### Steering complete (this phase)

The following completes the **“choice + licensing steering”** milestone for the current phase **without** claiming shipped **production** NOTICES are complete for every linked dependency:

- **Explicit build-time selection:** CMake cache string **`PDFDOCUMENTVIEW_RENDER_BACKEND`** with allowed values **`PDFIUM`** and **`POPPLER`**, default **`PDFIUM`**, validated at configure time and echoed once with `message(STATUS ...)`.
- **Consumer contract:** `find_package(PDFDocumentView CONFIG)` sets **`PDFDOCUMENTVIEW_RENDER_BACKEND`** to the value the install was built with; targets may also expose compile definitions **`PDFDOCUMENTVIEW_RENDER_BACKEND_PDFIUM`** or **`PDFDOCUMENTVIEW_RENDER_BACKEND_POPPLER`** (see `src/CMakeLists.txt`).
- **Repository license:** root **`LICENSE`** (library) documents distribution of **this repo’s source**; it does not replace upstream engine or Qt obligations.
- **Distribution placeholder:** **`docs/third-party-notices.template.md`** (installed with the package when install is on) is the maintainer checklist for pins and upstream links. **Verbatim upstream PDFium `LICENSE` / `AUTHORS`** for the documented SHA live under **`third_party/pdfium/`** (not in `docs/`) and install with the package under the conditions in **Status** above.
- **Reference-revision verification:** **`docs/engine-verification.md`** records a dated structural check of the **upstream** PDFium `LICENSE` at commit `a84323421e94f484faca52dd9d027934eba42ab8` **without** embedding that long text in `docs/`, and a second record for the **vendored minimum-revision snapshot** (verbatim files in `third_party/pdfium/`). **`docs/third-party-notices.md`** holds the working record (homepage, in-tree paths, approximate SPDX, obligations summary, and remaining open items for a **linked** engine). The verification table at the end of this ADR mirrors the same dated rows.
- **Prebuilt notices index:** **`docs/notices/pdfium-prebuilt-NOTICES.md`** records a **`tar -tf`** inventory (filenames only) for the default **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`**; CMake installs it as **`NOTICES.pdfium-prebuilt.md`** next to **`pdfium-prebuilt/licenses/`** when install is on (see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**).

**Still explicitly future work:** Windows/Linux automatic PDFium resolution; **`find_package(PDFDocumentView)`** install portability for the **IMPORTED** PDFium target (absolute paths); CI matrix; host **About** / product **`third-party-notices-<version>.md`** / counsel review for **combined** shipping artifacts. The repo now installs an aggregate **`NOTICES.pdfium-prebuilt.md`** and documents **`DEPS`** vs prebuilt scope; that discharges **only** the narrow **install + policy** claims in **Status**, not a complete downstream NOTICES pass.

---

## Context

This repository targets a **reusable Qt widget library** with **pluggable rendering backends** at the render and text-geometry layer, as described in `docs/concept-pdf-document-view.md` and `AGENTS.md`. The **minimal demo** under `examples/basic/` exists only to demonstrate integration; **host applications** that ship the widget to end users own distribution, activation, and legal packaging for their product.

- **Split from pdf-text-extractor:** This repo was separated to isolate **embeddable widget** functionality from the parent project.
- **Qt PDF vs direct PDFium:** Product experience in the parent stack indicated **Qt PDF / QTPDF** did not expose the full **PDFium-class capability surface** needed for this component’s goals; **direct PDFium** is therefore the accepted default engine path for breadth of features (rendering, geometry-style APIs), subject to “revisit if inadequate” in the concept doc.

Engine choice affects whether downstream can ship **permissively** or must honor **copyleft**; the concept document explicitly allows a **single default backend per release line** or **split packages** so consumers opt into obligations. Nothing in this ADR overrides that document; it operationalizes it for notices and verification.

---

## Options summary

| Option | Upstream license reference (verify at pin time) | Integration note |
|--------|---------------------------------------------------|------------------|
| **PDFium** (Chromium / `chromium/pdfium`) | Authoritative text lives in the upstream repository file [`LICENSE`](https://github.com/chromium/pdfium/blob/main/LICENSE). As of recent upstream, that file includes **PDFium-authors BSD-style redistribution terms** followed by the **Apache License, Version 2.0** text; treat the **whole file** as governing for the revision you ship, and read any **`AUTHORS` / `NOTICE`**-style files shipped with your chosen artifact. | Often favorable for **permissive** distribution of proprietary host apps **when** the combined work complies with those upstream terms and any transitive dependencies you bundle. |
| **Poppler** | Authoritative text is **`COPYING`** in the [Poppler repository](https://gitlab.freedesktop.org/poppler/poppler) (e.g. `master` branch `COPYING`). Poppler is widely described and packaged as **GNU GPL-2.0-or-later**; **confirm** the exact header in the revision you link. Project site: [poppler.freedesktop.org](https://poppler.freedesktop.org/). | Typical **Qt** integrations link **libpoppler** (often `libpoppler-cpp` / `libpoppler-qt`). **Linking mode and what you distribute** (static vs dynamic, LGPL vs GPL Qt, combined work vs separate process) affect **copyleft** scope; this is a **product and legal architecture** decision, not something this library can abstract away. |

This document is **not legal advice**; it is an engineering and governance record only.

---

## Decision

**Engine linked in CMake today:** **PDFium on macOS** when **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`** — see `cmake/PdfiumPrebuilt.cmake`, `src/pdfium_backend.cpp`, and `src/CMakeLists.txt`. **Poppler** is not linked (stub only). Other platforms require explicit **`PDFIUM_*`** cache paths until fetch support is extended.

**Accepted default render engine:** **PDFium** — primary implementation target; revisit only if capability or integration gaps prove it inadequate.

**Poppler** remains a **documented alternate** backend for integrators who **accept GPL obligations** (or who isolate Poppler in a way their counsel approves), matching the concept document’s **split package / explicit opt-in** direction.

**Tension handling:** The concept document describes PDFium licensing as often “BSD-style” and asks teams to **verify** the exact artifact. Upstream’s single `LICENSE` file currently combines BSD-style PDFium terms with Apache-2.0; teams must follow the **pinned revision’s** file, not a shorthand label.

---

## Consequences

- **Permissive vs copyleft:** A PDFium-backed build (compliant with upstream and transitive terms) generally supports **broader** proprietary redistribution than a classic in-process **GPL Poppler** link, subject to your exact stack and counsel.
- **Static vs dynamic linking:** Static linking tends to produce **one distributable** that must carry **all** applicable notices and license texts for linked code; dynamic linking may shift some obligation to **runtime packages** but does not automatically remove **GPL** concerns for in-process GPL libraries.
- **Split packages:** Shipping **separate** installables (e.g. core + `pdfdocumentview-pdfium` vs `pdfdocumentview-poppler`) can make consumer obligations explicit but **increases** packaging and CI matrix work.

---

## Third-party notices / compliance checklist

Use this checklist when producing **NOTICES** files, **About** boxes, SDK tarballs, or app store submissions:

- **Pin and record** exact engine versions (Git commit, package version, or prebuilt SDK id) in release notes or an internal bill of materials.
- **Collect upstream `LICENSE` / `COPYING` / `NOTICE` / `AUTHORS`** (whatever exists for that revision) for the engine and for **each** statically or vendored dependency you redistribute.
- **Apache-2.0-style obligations:** retain `NOTICE` attribution where upstream provides it; include **license text** where redistribution conditions require it; do not strip copyright headers from source you distribute.
- **GPL-style obligations (Poppler path):** plan for **corresponding source** or other GPL conditions **before** shipping; document whether the host uses **dynamic** linking to distro packages vs **bundling**; ensure **About** / distribution docs state the GPL dependency clearly.
- **Qt and toolchain:** include **Qt** and compiler/runtime attributions per your Qt license agreement and platform rules (separate from the PDF engine).
- **Build flags documentation:** record optional features (e.g. crypto, font backends) that pull in extra libraries, because they change the **notice set**.
- **SBOM:** optional but recommended for repeatable audits (SPDX, CycloneDX, or equivalent) when release cadence justifies it.

---

## Verification tasks

| Trigger | Owner | Action |
|---------|--------|--------|
| **First backend wire-up** | Maintainers / Arch | Re-read upstream `LICENSE` / `COPYING`, collect `NOTICE` when present at the **build** pin, refresh `third_party/pdfium/` if the **build** SHA diverges from the documentation pin, and move the **binary / linked-engine** portion of **Status** toward **Accepted** once linkage, repo `LICENSE` alignment, transitive deps, and a NOTICES pass on the shipped artifact are complete (the **narrow** vendored-text claim for `a843…` is already **Accepted**; see **Status**). |
| **Engine version bump** | Release owner | Diff upstream license files; refresh shipped notices; re-run checklist row for new transitive deps. |
| **Add second backend or split package** | Maintainers | Duplicate or extend checklist per artifact; ensure CMake options document which binary carries which obligations. |
| **Commercial proprietary host on Poppler** | Product + counsel | Treat as **mandatory** legal review; GPL interaction with Qt edition and linking is outside maintainer guarantees. |

---

## Verification record (this repo)

This table is **append-only**. Rows (1) **reference** upstream without vendoring long text into `docs/`, (2) record a **vendored minimum-revision snapshot** (verbatim `LICENSE` / `AUTHORS` under `third_party/pdfium/`), or (3) record the **bblanchon prebuilt build pin** used when CMake links **`libpdfium`**. Early rows do not assert a **linked** engine; the **third** and **fourth** rows document **minimum** legal snapshot vs. **prebuilt** pin respectively. Full method detail: `docs/engine-verification.md`. Working notices record: `docs/third-party-notices.md`. **Accepted** status for **all** shipped-binary obligations still requires the **hypothesis** gates in **Status** where applicable.

| Date | Artifact | Revision / pin | Method | Residual risk |
|------|----------|----------------|--------|---------------|
| 2026-05-13 | PDFium | `a84323421e94f484faca52dd9d027934eba42ab8` (upstream `main` HEAD at check, committed 2025-11-19 UTC) — **reference only** | HTTP fetch of `https://raw.githubusercontent.com/chromium/pdfium/main/LICENSE` and `https://api.github.com/repos/chromium/pdfium/commits/main`; **structural** inspection only (BSD-3-Clause-style PDFium-authors block + full Apache-2.0 text); **no** full upstream license text copied **into `docs/`**. | **Pin drift** on `main`; no claim of vendored engine source/binary. |
| 2026-05-13 | PDFium | Same SHA — **vendored minimum-revision snapshot** (canonical SHA in `third_party/pdfium/REVISION`; exposed by CMake cache `PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA`) | `curl -fsSL` of `LICENSE` and `AUTHORS` at `raw.githubusercontent.com/chromium/pdfium/<SHA>/…`; stored verbatim under `third_party/pdfium/`; `NOTICE`/`COPYING` 404 at this SHA (documented in `third_party/pdfium/README.md`). Installed to `${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium` when `PDFDOCUMENTVIEW_INSTALL` and `PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`. **Newer** upstream revisions are allowed for external sources; this row is the **minimum** snapshot for bundled notices. | **Build** revision may advance past this minimum (refresh together if upstream structure changes); transitive `DEPS` and feature flags not reviewed here. |
| 2026-05-13 | PDFium (bblanchon binary drop) | GitHub release tag **`chromium/7834`**; `pdfium-mac-arm64.tgz` / `pdfium-mac-x64.tgz` (SHA-256 enforced in `cmake/PdfiumPrebuilt.cmake`) | `file(DOWNLOAD)` + `cmake -E tar`; link **`libpdfium.dylib`** via **`PDFDocumentView::PDFium`**; demo copies dylib and runs **`install_name_tool`** for **`@loader_path`**. **`tar -tzf`** inventory of **`licenses/`** (filenames only) recorded in **`docs/notices/pdfium-prebuilt-NOTICES.md`**. When install is on, CMake installs **`NOTICES.pdfium-prebuilt.md`** always; when **`PDFIUM_PREBUILT_LICENSE_FILE`** is set, **`LICENSE.pdfium-binary-drop`**; when **`licenses/`** exists on the resolved prebuilt root at configure, **`…/pdfium-prebuilt/licenses/`**. | **MVP** macOS linkage; **Windows/Linux** not default-fetched; **`find_package`** may embed absolute paths; **`PDFIUM_*`** without layout root skips auto **`licenses/`**; Chromium **`DEPS`** source walk **out of scope** for prebuilt (use tarball **`licenses/`**); host **About** / full product NOTICES + counsel still **hypothesis** (see **Status**). |

---

## References

- PDFium upstream license file (moving `main` view): `https://github.com/chromium/pdfium/blob/main/LICENSE` — **pinned** verbatim copy: `third_party/pdfium/LICENSE` (see `third_party/pdfium/REVISION`).
- Poppler upstream COPYING: `https://gitlab.freedesktop.org/poppler/poppler/-/blob/master/COPYING` (confirm on the tag you use)
- Verification record log (this repo, append-only): `docs/engine-verification.md`
- Working third-party notices record (this repo): `docs/third-party-notices.md`
- Prebuilt install paths (maintainer): `docs/shipped-third-party-notices-pdfium-prebuilt.md`
- Vendored PDFium legal text directory: `third_party/pdfium/`
- Release-time notices template: `docs/third-party-notices.template.md`
- Host About / credits template: `docs/about/THIRD_PARTY_NOTICES.template.md`
- Prebuilt license filename index (maintainer): `docs/notices/pdfium-prebuilt-NOTICES.md`
- Project concept: `docs/concept-pdf-document-view.md`
- Agent contract: `AGENTS.md`

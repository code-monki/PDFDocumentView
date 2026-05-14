# PDF engine verification record

**Purpose:** capture **reference-revision** verifications, **vendored minimum-revision** legal snapshots (verbatim upstream files in **`third_party/pdfium/`**), and the **CMake build pin** for the **linked** macOS **bblanchon** prebuilt when that path is used. The in-tree **`third_party/pdfium/`** bundle is **documentation only** (no object code built from that tree). **Separately**, **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`** on **macOS** links **`libpdfium.dylib`** via **`PDFDocumentView::PDFium`** (`cmake/PdfiumPrebuilt.cmake`); see the **build-pin** row below for checksums and install-time notice paths. This file is **not** the install-time NOTICES bundle; see **`docs/third-party-notices.template.md`** and **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**.

**This is not legal advice.** It is an engineering / governance record.

---

## How to read this

A **verification record** says: at the date below, a maintainer fetched the upstream license artifact at the recorded revision and confirmed its **structure** — which licenses are present and how attribution is organized — matches what ADR-0001 documents. The record does **not** assert that ship-time NOTICES are complete for every transitive dependency of a **linked** binary.

When the project pins a **vendored source/binary artifact** and CMake wires the engine, add or refresh records for that **build** revision and complete the remaining **Open when shipping linked PDFium broadly** items in `docs/third-party-notices.md` (transitive deps, About text, and so on). A **minimum-revision documentation snapshot** (legal text capture only) is recorded separately below; newer upstream revisions remain permitted for external sources.

This document is **append-only** so the verification history stays auditable.

---

## Records

### 2026-05-13 — PDFium upstream `main` HEAD (reference revision, not a vendored pin)

| Field | Value |
|-------|-------|
| Artifact | **PDFium** (Chromium project mirror) |
| Reference revision | `a84323421e94f484faca52dd9d027934eba42ab8` (upstream `main` HEAD at verification time, committed 2025-11-19 UTC) |
| Upstream commit URL | `https://github.com/chromium/pdfium/commit/a84323421e94f484faca52dd9d027934eba42ab8` |
| License file URL fetched | `https://raw.githubusercontent.com/chromium/pdfium/main/LICENSE` |
| Latest-commit API URL inspected | `https://api.github.com/repos/chromium/pdfium/commits/main` |
| Method | HTTP fetch of the upstream `LICENSE` file and the latest-commit JSON; only **structural** inspection (which license blocks are present, how they are ordered). **No full upstream license text copied into this repository.** |
| Structure observed | The single `LICENSE` file begins with a **PDFium-authors BSD-3-Clause-style** redistribution block (Google Inc. named as the contributor entity, opening line `// Copyright 2014 The PDFium Authors`), followed by the **full Apache License, Version 2.0** text under the heading `Apache License / Version 2.0, January 2004`. Both blocks must be treated as governing at the pinned revision; per-file headers and any `AUTHORS` / `NOTICE`-style files shipped with the chosen artifact must also be honored at ship time. |
| Tag pin considered? | PDFium does not maintain a conventional stable-release tag stream on its GitHub mirror; commit SHA is the verifiable pin. The recorded SHA is a **reference** revision for the structural check above. A **separate** row below records the same SHA as a **vendored documentation pin** (`third_party/pdfium/`); that pin still does **not** mean the engine library is linked. |
| Residual risk | **Pin drift** on the `main`-branch fetch URL used for this structural check vs. future upstream commits. A **vendored documentation pin** for the same SHA exists below; refresh both rows together when changing pins. This row does **not** substitute for a full NOTICES pass on a **linked** `libpdfium` build (see build-pin row). |
| Verified by | Maintainer (this file is the source of truth; commit history records the change). |

### 2026-05-13 — PDFium vendored **minimum-revision** snapshot (`third_party/pdfium/`, same SHA as reference)

| Field | Value |
|-------|-------|
| Artifact | **PDFium** (Chromium project mirror) |
| Snapshot type | **Vendored minimum-revision snapshot** — verbatim upstream **`LICENSE`** and **`AUTHORS`** files for the SHA in `third_party/pdfium/REVISION`, plus maintainer `README.md`. The SHA records the **minimum supported upstream revision** for the **bundled** legal text; **newer** upstream revisions are allowed for builds that obtain sources elsewhere. **No** PDFium object code is **built from** this in-tree directory; it is a **legal-text bundle** only. **Linked** `libpdfium.dylib` (when enabled) comes from the **bblanchon** prebuilt or integrator-supplied paths in `cmake/PdfiumPrebuilt.cmake`, not from compiling this tree. |
| Minimum revision | `a84323421e94f484faca52dd9d027934eba42ab8` (same commit as the reference row above; not superseded). Exposed by CMake as `PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA` (defaulted from `REVISION` by `cmake/PDFiumMinimumRevision.cmake`; overridable at configure time). |
| Upstream commit URL | `https://github.com/chromium/pdfium/commit/a84323421e94f484faca52dd9d027934eba42ab8` |
| Files fetched (raw.githubusercontent.com) | `LICENSE` (HTTP 200), `AUTHORS` (HTTP 200). `NOTICE` and `COPYING` returned **HTTP 404** at this revision; nothing vendored for those names (see `third_party/pdfium/README.md`). |
| In-tree location | `third_party/pdfium/LICENSE`, `third_party/pdfium/AUTHORS`, `third_party/pdfium/REVISION`, `third_party/pdfium/README.md` |
| CMake / install | When `PDFDOCUMENTVIEW_INSTALL` is **ON** and `PDFDOCUMENTVIEW_RENDER_BACKEND` is **PDFIUM**, whatever files exist under `third_party/pdfium/` install to `${CMAKE_INSTALL_DOCDIR}/pdfdocumentview/third_party/pdfium` (see root `CMakeLists.txt`). Configure-time `message(STATUS …)` echoes the minimum SHA once. |
| Method | `curl -fsSL` of upstream raw URLs at the minimum SHA; files stored **verbatim** under `third_party/pdfium/`. |
| Residual risk | **Two pins:** the **minimum** Git SHA here vs. the **prebuilt** Chromium tag in `PdfiumPrebuilt.cmake` may differ. **Snapshot drift** if upstream structure changes — refresh `third_party/pdfium/` and this log together when the minimum advances. **Linked stack:** transitive `DEPS`, feature flags, Windows/Linux fetch defaults, and product-level NOTICES / About text are **not** fully reviewed in this row. |
| Verified by | Maintainer (this file is the source of truth; commit history records the change). |

### 2026-05-13 — PDFium **bblanchon** prebuilt (CMake build pin; linked `libpdfium` on macOS MVP)

| Field | Value |
|-------|-------|
| Artifact | [bblanchon/pdfium-binaries](https://github.com/bblanchon/pdfium-binaries) release assets (not Chromium source checkout). |
| Cache pin | **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`** default **`chromium/7834`** (`cmake/PdfiumPrebuilt.cmake`). |
| **`pdfium-mac-arm64.tgz`** SHA-256 | `2b733774416de02482281c0abc7589b08dc908896ecef2bfc31a85c5b5ffd572` |
| **`pdfium-mac-x64.tgz`** SHA-256 | `fcfed5eaf8fe9a761577d626dff651227600a52fc5f933c461447564361bb036` |
| CMake linkage | **`PDFDocumentView::PDFium`** — `SHARED IMPORTED` target; `pdf_document_view` links it when **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`** (`src/CMakeLists.txt`). |
| Layout root for install-time **`licenses/`** | **`PDFIUM_FETCH_EXTRACT_ROOT`** after successful fetch, else integrator **`PDFIUM_ROOT`** when that path is used (see `cmake/PdfiumPrebuilt.cmake`). |
| Install ( **`PDFDOCUMENTVIEW_INSTALL`** + PDFIUM) | **`…/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`** (always). **`LICENSE.pdfium-binary-drop`** and **`…/pdfium-prebuilt/licenses/`** when the corresponding configure-time sources exist — see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`**. |
| Method | Configure-time **`file(DOWNLOAD … EXPECTED_HASH=…)`** + extract; **`tar -tzf pdfium-mac-arm64.tgz | grep '^licenses/'`** (filenames only) cross-checked into **`docs/notices/pdfium-prebuilt-NOTICES.md`**; no substantive license text copied into `docs/`. |
| Residual risk | **Build pin ≠** `third_party/pdfium/REVISION` minimum SHA. **Windows/Linux:** no default fetch in this pass. **Integrator-only paths** may omit auto **`licenses/`**. **`find_package`** may embed absolute paths. **Chromium `DEPS`** for a **source** build is **not** claimed for this **prebuilt** artifact — tarball **`licenses/`** is the binary-drop notice set. Host **About** / full product NOTICES + counsel still **hypothesis** — see ADR-0001 and **`docs/third-party-notices.md`**. |
| Verified by | Maintainer (this file is the source of truth; commit history records the change). |

---

## Recheck triggers

Re-run a verification record and append a row above when **any** of these happen:

1. CMake or any build path begins consuming a **vendored** PDFium revision — the new record uses the **actual** vendored SHA / tarball checksum, not `main`, and notes whether the build revision sits at or above the recorded minimum.
2. The project rolls the vendored minimum-revision snapshot forward (typical) or back (only with explicit rationale and a fresh verification row).
3. Upstream restructures its `LICENSE` (e.g. adds a third block, changes Apache-2.0 component, splits into multiple files).
4. A second backend (e.g. Poppler) is wired; that backend needs its own record in this file.

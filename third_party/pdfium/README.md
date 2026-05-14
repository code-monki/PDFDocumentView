# PDFium upstream legal texts (vendored minimum-revision snapshot)

This directory holds **verbatim copies** of selected legal / attribution files from the [PDFium](https://github.com/chromium/pdfium) GitHub mirror at the **single-line commit SHA** in `REVISION`. They are the **in-tree source of truth** for the **bundled** legal text at that **minimum** revision and are consumed by this repository's CMake install rules; they are **not** a substitute for a full ship-time NOTICES pass (transitive dependencies, build flags, the **actual** linked artifact, etc.).

- **`LICENSE`** — upstream composite license (PDFium-authors BSD-style terms + Apache-2.0 text).
- **`AUTHORS`** — upstream contributor listing.
- **`REVISION`** — canonical SHA for the **minimum verified** upstream revision (see policy below).

## Minimum revision policy

The SHA in `REVISION` is the **minimum supported upstream PDFium revision** for the **bundled** legal text in this directory. It is **not** an "exact only forever" pin and it does **not** impose a runtime equality check on the engine source a downstream build links against.

Concretely:

- The repository **guarantees** that the verbatim legal files alongside this README correspond to the SHA in `REVISION`, and that a maintainer has recorded a verification row in `docs/engine-verification.md` for that SHA.
- Builds that obtain PDFium sources elsewhere (FetchContent, prebuilt SDK, system package) are **allowed** to use **newer** upstream revisions. Those builds are responsible for their own NOTICES pass against the **actual** revision they ship and should compare upstream `LICENSE` / `AUTHORS` / any `NOTICE` files to the snapshot here.
- Downstream tooling that wants to detect older or unverified revisions should consult the CMake cache variable **`PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA`** (see `cmake/PDFiumMinimumRevision.cmake`), which defaults to the SHA in `REVISION` and can be overridden at configure time.

### How the minimum is exposed

- **CMake (single variable, documentation-grade):** `cmake/PDFiumMinimumRevision.cmake` reads `REVISION` and exposes the value as a cache string `PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA`. The root `CMakeLists.txt` echoes it once at configure time (`-- PDFDocumentView: PDFium minimum (bundled notices): <sha>`). Maintainers bump in **one place** (this file's `REVISION`) or override on the command line via `-DPDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA=<sha>`.
- **No runtime enforcement today:** the library does not yet link a concrete PDFium artifact, so the minimum is a **documentation / governance** signal, not a compile- or run-time check. When the engine is wired, the same variable can drive a guard in `find_package` / `FetchContent` logic.

## Files not present at this revision

At the SHA currently recorded in `REVISION`, `NOTICE` and `COPYING` return **HTTP 404** on `https://raw.githubusercontent.com/chromium/pdfium/<SHA>/…` (verified for `a84323421e94f484faca52dd9d027934eba42ab8`; re-check on every pin bump in case upstream restores them). There is nothing to vendor for those paths at that revision.

## How to bump the minimum revision

There is a **single canonical edit point** (`REVISION`). The cache variable picks up the new value automatically on the next configure.

1. Edit `REVISION` to the new upstream commit SHA (single line, no extra whitespace).
2. Re-fetch the corresponding legal files from `https://raw.githubusercontent.com/chromium/pdfium/<SHA>/LICENSE` and `…/AUTHORS`. Also probe `…/NOTICE` and `…/COPYING` in case they reappear; commit them if present. Suggested offline-friendly invocation:

   ```sh
   SHA="<new-sha>"
   for f in LICENSE AUTHORS NOTICE COPYING; do
       curl -fsSL "https://raw.githubusercontent.com/chromium/pdfium/${SHA}/${f}" \
           -o "third_party/pdfium/${f}" || rm -f "third_party/pdfium/${f}"
   done
   ```

3. Update `docs/engine-verification.md` (append a new row; do not rewrite history), and align language in `docs/third-party-notices.md` and `docs/adrs/0001-pdf-engine-license-and-third-party-notices.md` if structure (license blocks, attribution, transitive expectations) has changed.
4. Reconfigure CMake (`cmake -B build …`) and verify the status line reads the new SHA. If you need to override without editing the file (e.g. for a temporary spike), pass `-DPDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA=<sha>` and document the deviation.
5. Re-run `cmake --install` against a scratch prefix to confirm the doc-install rules still pick up whichever files exist under `third_party/pdfium/`.

Upstream canonical Git: `https://pdfium.googlesource.com/pdfium/` (this repo documents the GitHub mirror for raw URLs).

# PDFiumMinimumRevision.cmake
#
# Single source of truth for the **minimum** upstream PDFium Git revision that
# this repository's **vendored legal snapshot** under `third_party/pdfium/`
# has been verified against.
#
# Canonical SHA file: `third_party/pdfium/REVISION` (one-line Git commit SHA).
# This module reads that file and exposes the value as a **cache variable**
# `PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA` so maintainers can bump it in one
# place (the REVISION file) **or** override it at configure time, e.g.:
#
#     cmake -S . -B build -DPDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA=<new-sha>
#
# Semantics (documentation only — no runtime check):
#   - This SHA is the **minimum supported upstream revision** for the
#     **bundled** legal text (LICENSE / AUTHORS) under `third_party/pdfium/`.
#   - **Newer** upstream PDFium revisions are **allowed** for builds that
#     obtain PDFium sources elsewhere (FetchContent, prebuilt SDK, system
#     package). Such builds remain responsible for their own NOTICES pass
#     against the **actual** revision they ship (see ADR-0001).
#   - This is **not** an "exact only" pin; it documents that the in-tree
#     legal snapshot is verified **at least** through this SHA until the
#     maintainer refreshes it.
#
# How to bump (single place):
#   1. Update `third_party/pdfium/REVISION` to the new commit SHA.
#   2. Refresh the vendored legal texts (`LICENSE`, `AUTHORS`, and check for
#      `NOTICE` / `COPYING` reappearance) from
#      `https://raw.githubusercontent.com/chromium/pdfium/<SHA>/...`.
#   3. Append a row to `docs/engine-verification.md` and update
#      `docs/third-party-notices.md` / ADR-0001 as the policy doc requires.
#   4. Re-configure; the cache default will pick up the new SHA. Existing
#      build trees may need `-DPDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA=<sha>`
#      or a fresh cache to refresh the variable.

set(_pdfdocumentview_pdfium_revision_file
    "${CMAKE_CURRENT_LIST_DIR}/../third_party/pdfium/REVISION")

if(NOT EXISTS "${_pdfdocumentview_pdfium_revision_file}")
    message(FATAL_ERROR
        "PDFDocumentView: cannot find vendored PDFium REVISION file at "
        "'${_pdfdocumentview_pdfium_revision_file}'. This file is the canonical "
        "source for PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA; create it (single "
        "line containing a Git commit SHA) before configuring.")
endif()

file(STRINGS "${_pdfdocumentview_pdfium_revision_file}"
    _pdfdocumentview_pdfium_revision_lines
    LIMIT_COUNT 1
    REGEX "[0-9a-fA-F]")

if(NOT _pdfdocumentview_pdfium_revision_lines)
    message(FATAL_ERROR
        "PDFDocumentView: '${_pdfdocumentview_pdfium_revision_file}' is empty or "
        "does not contain a hexadecimal Git commit SHA on the first matching line.")
endif()

list(GET _pdfdocumentview_pdfium_revision_lines 0
    _pdfdocumentview_pdfium_revision_default)
string(STRIP "${_pdfdocumentview_pdfium_revision_default}"
    _pdfdocumentview_pdfium_revision_default)

set(PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA
    "${_pdfdocumentview_pdfium_revision_default}"
    CACHE STRING
    "Minimum verified upstream PDFium Git commit SHA covered by the vendored \
legal snapshot in third_party/pdfium/. Default is read from \
third_party/pdfium/REVISION. Newer upstream revisions are allowed for \
external sources; this records the bundled-notices floor.")

if(NOT PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA MATCHES "^[0-9a-fA-F]+$")
    message(FATAL_ERROR
        "PDFDocumentView: PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA must be a "
        "hexadecimal Git commit SHA (got '${PDFDOCUMENTVIEW_PDFIUM_MINIMUM_GIT_SHA}').")
endif()

unset(_pdfdocumentview_pdfium_revision_file)
unset(_pdfdocumentview_pdfium_revision_lines)
unset(_pdfdocumentview_pdfium_revision_default)

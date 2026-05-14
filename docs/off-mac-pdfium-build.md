# PDFium off macOS

This project resolves **PDFium** for `PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM` via `cmake/PdfiumPrebuilt.cmake`.

## Automatic fetch (no secrets)

On **Linux (glibc) x86_64 and arm64**, CMake defaults **`PDFDOCUMENTVIEW_FETCH_PDFIUM=ON`** and downloads the same pinned [bblanchon/pdfium-binaries](https://github.com/bblanchon/pdfium-binaries) tag as macOS, with SHA-256 verification.

- **musl** (typical Alpine): set **`PDFDOCUMENTVIEW_PDFIUM_LINUX_ABI=MUSL`** or unpack a musl tarball yourself and point **`PDFIUM_ROOT`** at the extracted tree (`include/` + `lib/libpdfium.so`).
- **Linux armv7** and other ABIs: there is no pinned fetch in-tree yet; supply **`PDFIUM_ROOT`** / **`PDFIUM_INCLUDE_DIR`** + **`PDFIUM_LIBRARY`** or extend `PdfiumPrebuilt.cmake` with the matching release asset and digest from the upstream release JSON.

## Windows

**`PDFDOCUMENTVIEW_FETCH_PDFIUM`** defaults **ON** (same policy as macOS / Linux glibc): pinned `pdfium-win-*.tgz` with SHA-256 verification. Set **`PDFDOCUMENTVIEW_FETCH_PDFIUM=OFF`** and **`PDFIUM_ROOT`** (or **`PDFIUM_INCLUDE_DIR`** + **`PDFIUM_LIBRARY`**) for air-gapped or policy-restricted configures.

## Integrator-only layout

If fetch is disabled or unsupported, set **`PDFIUM_ROOT`** to an extracted bblanchon layout (`include/fpdfview.h`, `lib/libpdfium.so` or macOS dylib, or `bin/pdfium.dll` on Windows), or set **`PDFIUM_INCLUDE_DIR`** and **`PDFIUM_LIBRARY`** explicitly.

## Licensing

Binary drops are governed by upstream and transitive notices; this repo installs **`NOTICES.pdfium-prebuilt.md`** and optional tarball **`licenses/`** when install is enabled. Do not commit prebuilt blobs into git.

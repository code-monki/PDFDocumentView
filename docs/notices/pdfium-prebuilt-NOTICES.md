# PDFium prebuilt — third-party license index (bblanchon)

**Not legal advice.** Maintainer-only index for the **default** CMake pin **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`** = **`chromium/7834`** (`cmake/PdfiumPrebuilt.cmake`). Verbatim license text lives only in the upstream tarball’s **`licenses/`** tree (installed under **`…/pdfium-prebuilt/licenses/`** when install rules apply); **do not** treat this table as a substitute for those files.

**Inventory (filenames only):** release asset **`pdfium-mac-arm64.tgz`**, SHA-256 `2b733774416de02482281c0abc7589b08dc908896ecef2bfc31a85c5b5ffd572`, command `tar -tzf <asset> | grep '^licenses/'` on **2026-05-13**. The sibling **`pdfium-mac-x64.tgz`** at the same tag uses the same checksum policy in `cmake/PdfiumPrebuilt.cmake`; re-run the listing when changing tags or if upstream rearranges the archive.

| Component (short) | Upstream hint | License file (relative to tarball/install `licenses/`) |
|-------------------|----------------|---------------------------------------------------------|
| ICU | ICU project | `icu.txt` |
| zlib | zlib | `zlib.txt` |
| libjpeg-turbo | libjpeg-turbo / IJG portions | `libjpeg_turbo.ijg`, `libjpeg_turbo.md` |
| LLVM libc++ | LLVM project | `llvm-libc.txt` |
| PDFium | Chromium PDFium | `pdfium.txt` |
| Little CMS | Little CMS (lcms2) | `lcms.txt` |
| libpng | libpng | `libpng.txt` |
| Anti-Grain Geometry | AGG 2.3 | `agg23.txt` |
| OpenJPEG | OpenJPEG | `libopenjpeg.txt` |
| FreeType | FreeType | `freetype.txt` |
| libTIFF | libtiff | `libtiff.txt` |
| fast_float | fast_float | `fast_float.txt` |
| Abseil | Abseil (Google) | `abseil.txt` |
| simdutf | simdutf | `simdutf.txt` |

**Scope:** This list reflects **only** files present under **`licenses/`** in the **prebuilt binary drop** for the pin above. It does **not** enumerate Chromium **PDFium `DEPS`** for a **full source** checkout; see **`docs/shipped-third-party-notices-pdfium-prebuilt.md`** and ADR-0001.

# Shipped third-party notices — bblanchon PDFium prebuilt (CMake)

**Not legal advice.** Maintainer pointer only; filenames under `licenses/` come from the upstream tarball — do not paraphrase them here.

## Upstream

- Prebuilt assets: [bblanchon/pdfium-binaries](https://github.com/bblanchon/pdfium-binaries) (release tag matches cache variable **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`**; digests in `cmake/PdfiumPrebuilt.cmake`).

## What this repository installs (when install is enabled)

When **`PDFDOCUMENTVIEW_INSTALL`** is **ON** and **`PDFDOCUMENTVIEW_RENDER_BACKEND=PDFIUM`**:

| Installed path (under install prefix) | Source / condition |
|----------------------------------------|---------------------|
| **`…/pdfdocumentview/third_party/pdfium-prebuilt/NOTICES.pdfium-prebuilt.md`** | Always (repo **`docs/notices/pdfium-prebuilt-NOTICES.md`**): maintainer **index** of tarball **`licenses/`** filenames for the default **`PDFDOCUMENTVIEW_PDFIUM_PREBUILT_TAG`**; **no** full license bodies. |
| **`…/pdfdocumentview/third_party/pdfium-prebuilt/LICENSE.pdfium-binary-drop`** | Tarball root **`LICENSE`**, when CMake set **`PDFIUM_PREBUILT_LICENSE_FILE`** (e.g. macOS fetch or **`PDFIUM_ROOT`** containing **`LICENSE`**). |
| **`…/pdfdocumentview/third_party/pdfium-prebuilt/licenses/`** *(each file as in the tarball)* | Tarball **`licenses/`** subtree, **only when** that directory exists on the resolved prebuilt layout root at **configure** time (**`PDFIUM_FETCH_EXTRACT_ROOT`** after fetch, or integrator **`PDFIUM_ROOT`** — see `cmake/PdfiumPrebuilt.cmake`). |

\*Integrators who pass **`PDFIUM_INCLUDE_DIR`+`PDFIUM_LIBRARY`** only (no layout root) still receive **`NOTICES.pdfium-prebuilt.md`** but must supply **`LICENSE`** and **`licenses/`** from the matching [pdfium-binaries](https://github.com/bblanchon/pdfium-binaries/releases) asset themselves if they ship the dylib.

`CMAKE_INSTALL_DOCDIR` follows **GNUInstallDirs** (e.g. `share/doc/<project>/…` on many Unix layouts). The segment after that is **`pdfdocumentview/third_party/pdfium-prebuilt/`**.

## Binary drop vs Chromium `DEPS` (scope)

Chromium’s PDFium **`DEPS`** file describes how to fetch and pin **dependencies for building PDFium from source** inside the Chromium ecosystem. **This distribution does not ship a PDFium source tree or run that `DEPS` resolver** when you use the default **bblanchon** prebuilt: it ships the **compiled** shared library plus whatever the upstream **binary tarball** already bundles. For **third-party license text tied to that binary drop**, follow the tarball’s **`licenses/`** directory (and root **`LICENSE`**) for **components included in the prebuilt** you actually link or redistribute. A **full walk of upstream `DEPS`** for a from-source Chromium/PDFium build is **out of scope** for this repo’s install rules unless you **vendor PDFium source** and own that build — then your release process must collect notices for **your** dependency closure separately.

## If `licenses/` is missing at configure time

Some integrator trees (minimal **`PDFIUM_ROOT`**) may omit **`licenses/`**. CMake then **does not** emit `install(DIRECTORY …)` for that subtree. For a release that ships **`libpdfium`**, copy the **`licenses/`** directory from the **matching** [pdfium-binaries release asset](https://github.com/bblanchon/pdfium-binaries/releases) next to your other shipped notices, and extend your release **`third-party-notices-*.md`** using **`docs/third-party-notices.template.md`**.

## Obligations (engineering summary)

Treat each file under **`licenses/`** and **`LICENSE.pdfium-binary-drop`** as third-party attribution / license text required by the corresponding upstream projects (see those files). A full distribution review (feature flags, static vs dynamic deps, product packaging) remains the integrator’s responsibility; see **`docs/third-party-notices.md`** and ADR-0001.

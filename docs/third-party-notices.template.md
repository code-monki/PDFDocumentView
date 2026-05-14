# Third-party notices (template)

**Purpose:** Release engineering fills this in (or generates a sibling `NOTICES` / `ThirdPartyNotices` file) **before** shipping binaries or SDKs that bundle or statically link third-party code (PDF engine, Qt redistributables, etc.).

**Instructions**

1. Copy this file to a release-specific name (for example `third-party-notices-<version>.md`) or merge sections into your product’s consolidated notices file.
2. **Pin** every vendored or redistributed component to an exact revision (Git commit, package version, or SDK build id). Record the pin in your bill of materials or release notes.
3. For **PDFium** (if used): paste **copyright / attribution lines** required by the **pinned** upstream tree only—do not treat this template as authoritative legal text. The upstream license file is here: https://github.com/chromium/pdfium/blob/main/LICENSE  
   - **Pinned revision (fill at release):** `<PDFIUM_COMMIT_OR_TAG>`  
   - **Copyright / NOTICE excerpts (fill at release):** `<PASTE_MINIMAL_REQUIRED_LINES_FROM_PINNED_UPSTREAM_ONLY>`
4. For **Poppler** (if used): follow the **pinned** upstream `COPYING` and any `AUTHORS` / notice files. Project: https://poppler.freedesktop.org/  
   - **Pinned revision (fill at release):** `<POPPLER_COMMIT_OR_TAG>`
5. For **Qt** and other runtime dependencies: follow your Qt license agreement and platform rules (separate from the PDF engine).
6. Remove unused backend sections before publication if you ship only one engine.

**Repository library license**

The **PDFDocumentView** widget library source in this repository is under the terms in the root `LICENSE` file (see that file for the SPDX identifier and full text). **Linking or bundling a PDF engine** may impose **additional** obligations; this template is only a checklist placeholder.

---

## Open until release

| Item | Owner | Status |
|------|--------|--------|
| PDF engine revision pinned and recorded | Release | `<TODO>` |
| Upstream license / NOTICE files collected for that pin | Release | `<TODO>` |
| Binary or SDK layout reviewed for all redistributed libs | Release / Legal | `<TODO>` |

When the above are complete, align with `docs/adrs/0001-pdf-engine-license-and-third-party-notices.md` and archive the filled notices with the shipped artifact.

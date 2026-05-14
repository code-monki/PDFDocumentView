# Third-party notices — About box / credits (host template)

**Not legal advice.** Host applications that ship **PDFDocumentView** with the **PDFIUM** backend should surface third-party attribution where end users expect it (About dialog, credits, or a “Legal” screen). Replace placeholders below with your product facts; keep links accurate for your install layout.

---

**`<PRODUCT_NAME>`** (version **`<PRODUCT_VERSION>`**) includes third-party software. PDF rendering may use **PDFium** and bundled components; license texts shipped with this library’s install tree include, when present:

- **`<NOTICES_PDFIUM_PREBUILT_URI>`** — e.g. path or URL to the installed file **`NOTICES.pdfium-prebuilt.md`** under **`…/pdfdocumentview/third_party/pdfium-prebuilt/`** (see **`CMAKE_INSTALL_DOCDIR`** / GNUInstallDirs on your platform).
- The **`licenses/`** directory alongside that path, and **`LICENSE.pdfium-binary-drop`**, when your build/install included them.

For the authoritative file list and upstream pin, see the installed **`NOTICES.pdfium-prebuilt.md`** and **`docs/shipped-third-party-notices-pdfium-prebuilt.md`** in the **PDFDocumentView** source distribution. Your legal and release teams remain responsible for a complete notices set for **your** product (Qt, other engines, static vs dynamic linking, counsel review where needed).

# Test fixtures (PDFs)

**`minimal-one-page.pdf`** — synthetic, repo-authored for tests (minimal PDF 1.1, one blank page).

**`demo-synthetic-3page.pdf`** — synthetic three-page PDF (short on-page labels) used as the **basic demo** default document; regenerate with **`tools/generate_demo_synthetic_3page_pdf.sh`** (requires **`gs`**).

This directory is reserved for **small, approved** PDF binaries used in automated tests or documentation examples.

- **`pdf_document_fixture_smoke`** (PDFium builds only) opens **`minimal-one-page.pdf`** via **`PdfDocumentModel::openFile`**; **`pdf_document_model_smoke`** still covers empty/invalid buffers without a fixture.
- Prefer **synthetic** minimal PDFs or files with **clear, documented** licenses.
- Do **not** commit large manuals or copyrighted corpora here without explicit project approval.

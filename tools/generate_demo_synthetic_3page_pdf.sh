#!/usr/bin/env sh
# Regenerate tests/fixtures/demo-synthetic-3page.pdf (synthetic text for basic demo / manual checks).
# Requires Ghostscript (gs) on PATH. Committed output keeps CI free of this dependency.

set -eu
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${ROOT}/tests/fixtures/demo-synthetic-3page.pdf"
TMP="${OUT}.tmp.$$"

cleanup() { rm -f "${TMP}"; }
trap cleanup EXIT

command -v gs >/dev/null 2>&1 || {
    echo "error: gs (Ghostscript) not found; install it or copy a suitable 3-page PDF to ${OUT}" >&2
    exit 1
}

gs -q -o "${TMP}" -sDEVICE=pdfwrite -dNOPAUSE -dBATCH \
    -c "/Times-Roman findfont 24 scalefont setfont \
        100 700 moveto (PDFDocumentView demo page 1) show showpage \
        100 700 moveto (PDFDocumentView demo page 2) show showpage \
        100 700 moveto (PDFDocumentView demo page 3) show showpage"

mv "${TMP}" "${OUT}"
echo "wrote ${OUT}"

#!/usr/bin/env sh
# Regenerate examples/basic/demo-synthetic-3page.pdf (synthetic placeholder text only).
# Requires Ghostscript (gs) on PATH. Committed output keeps CI free of this dependency.

set -eu
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${ROOT}/examples/basic/demo-synthetic-3page.pdf"
TMP="${OUT}.tmp.$$"

cleanup() { rm -f "${TMP}"; }
trap cleanup EXIT

command -v gs >/dev/null 2>&1 || {
    echo "error: gs (Ghostscript) not found; install it or copy a suitable 3-page PDF to ${OUT}" >&2
    exit 1
}

gs -q -o "${TMP}" -sDEVICE=pdfwrite -dNOPAUSE -dBATCH \
    -dDEVICEWIDTHPOINTS=612 -dDEVICEHEIGHTPOINTS=792 \
    -c "/Helvetica findfont 14 scalefont setfont \
        72 720 moveto (Synthetic PDFDocumentView demo placeholder text page 1) show showpage \
        72 720 moveto (Synthetic PDFDocumentView demo placeholder text page 2) show showpage \
        72 720 moveto (Synthetic PDFDocumentView demo placeholder text page 3) show showpage"

mv "${TMP}" "${OUT}"
echo "wrote ${OUT}"

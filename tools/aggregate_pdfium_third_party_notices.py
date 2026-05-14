#!/usr/bin/env python3
"""
Aggregate PDFium prebuilt ``licenses/`` text under a ``PDFIUM_ROOT`` (or fetch extract
layout) into a single markdown file, prefixed by ``docs/about/THIRD_PARTY_NOTICES.template.md``.

Not legal advice — integrators still own complete product notices.
"""

from __future__ import annotations

import argparse
import datetime as _dt
import sys
from pathlib import Path


def _repo_root() -> Path:
    return Path(__file__).resolve().parent.parent


def _read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def _gather_license_files(licenses_dir: Path) -> list[Path]:
    if not licenses_dir.is_dir():
        return []
    files = [p for p in licenses_dir.iterdir() if p.is_file()]
    return sorted(files, key=lambda p: p.name.lower())


def _format_file_section(rel_name: str, body: str) -> str:
    body = body.rstrip()
    return f"### `{rel_name}`\n\n```\n{body}\n```\n"


def build_markdown(
    *,
    template_text: str,
    pdfium_root: Path,
    include_root_license: bool,
) -> str:
    licenses_dir = pdfium_root / "licenses"
    parts: list[str] = [template_text.rstrip()]

    now = _dt.datetime.now(tz=_dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    parts.append("")
    parts.append("---")
    parts.append("")
    parts.append("## PDFium prebuilt — aggregated `licenses/` (machine-generated)")
    parts.append("")
    parts.append(f"- **PDFIUM_ROOT (input):** `{pdfium_root.resolve()}`")
    parts.append(f"- **Generated at (UTC):** `{now}`")
    parts.append("")
    parts.append(
        "Verbatim text from files under `licenses/` next to `include/fpdfview.h` in the "
        "bblanchon-style prebuilt tree (or your integrator mirror). "
        "Do not treat this file as counsel-reviewed; ship upstream files when required."
    )
    parts.append("")

    if include_root_license:
        lic = pdfium_root / "LICENSE"
        if lic.is_file():
            parts.append("### Top-level `LICENSE` (binary drop)\n")
            parts.append("```")
            parts.append(_read_text(lic).rstrip())
            parts.append("```")
            parts.append("")

    files = _gather_license_files(licenses_dir)
    if not files:
        parts.append(
            "*No `licenses/` directory or no files found under this PDFIUM_ROOT — "
            "nothing aggregated.*"
        )
        parts.append("")
    else:
        parts.append(f"**Files ({len(files)}):**")
        parts.append("")
        for p in files:
            rel = f"licenses/{p.name}"
            parts.append(_format_file_section(rel, _read_text(p)))

    return "\n".join(parts).rstrip() + "\n"


def main(argv: list[str]) -> int:
    root = _repo_root()
    default_template = root / "docs" / "about" / "THIRD_PARTY_NOTICES.template.md"
    default_out = root / "docs" / "about" / "THIRD_PARTY_NOTICES.pdfium.md"

    ap = argparse.ArgumentParser(
        description="Merge THIRD_PARTY_NOTICES.template.md with PDFium licenses/ tree.",
        epilog=(
            "This tool aggregates files present under PDFIUM_ROOT; it does not replace a full "
            "Chromium DEPS / from-source license review for your pinned PDFium revision. "
            "Not legal advice."
        ),
    )
    ap.add_argument(
        "--pdfium-root",
        type=Path,
        required=True,
        help="Root of extracted prebuilt (contains include/ and usually licenses/).",
    )
    ap.add_argument(
        "--template",
        type=Path,
        default=default_template,
        help=f"Host template markdown (default: {default_template})",
    )
    ap.add_argument(
        "--out",
        type=Path,
        default=default_out,
        help=f"Output markdown (default: {default_out})",
    )
    ap.add_argument(
        "--include-root-license",
        action="store_true",
        help="Also append top-level LICENSE next to licenses/ if present.",
    )
    args = ap.parse_args(argv)

    pdfium_root = args.pdfium_root.expanduser().resolve()
    if not (pdfium_root / "include" / "fpdfview.h").is_file():
        print(
            f"error: PDFIUM_ROOT must contain include/fpdfview.h (got {pdfium_root})",
            file=sys.stderr,
        )
        return 2

    template_path = args.template.expanduser().resolve()
    if not template_path.is_file():
        print(f"error: template not found: {template_path}", file=sys.stderr)
        return 2

    out_path = args.out.expanduser().resolve()
    out_path.parent.mkdir(parents=True, exist_ok=True)

    md = build_markdown(
        template_text=_read_text(template_path),
        pdfium_root=pdfium_root,
        include_root_license=bool(args.include_root_license),
    )
    out_path.write_text(md, encoding="utf-8", newline="\n")
    print(f"Wrote {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

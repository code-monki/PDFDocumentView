# AGENTS.md

## 1. Purpose

This file defines the **operational contract** for any AI agent interacting with this repository.

It enforces:

- clear separation between **embeddable widget library** and **host applications**
- disciplined handling of **copyrighted or restricted PDFs** and derived text
- risk-driven exploration before formalizing APIs
- use of repository **governance** under `ai-toolkit/` when producing stable artifacts
- no confusion between a **demo `main.cpp`** and a **product viewer application**

This file is **authoritative** for agent behavior in this repository.

---

## 2. Toolkit Location (MANDATORY)

All governance, guardrails, prompts, and templates are located under:

```
ai-toolkit/
```

Agents MUST use explicit paths when referencing toolkit files.

DO NOT assume files exist at repository root except what this repository documents.

---

## 3. Project Identity

This repository implements a **reusable, widget-centric Qt component** for displaying PDF content and related behaviors (navigation, optional search, highlight overlays with geometry).

The project IS:

- an **independent** C++/Qt library intended for use by **multiple** applications
- structured around a **`QWidget`-class** (or closely related) **public API**
- designed for **pluggable rendering backends** (e.g. PDFium vs Poppler) behind narrow interfaces
- accompanied by a **minimal example binary** that demonstrates **integration only**

The project is NOT:

- a goal to land this code **inside** the Qt Project upstream distribution (non-goal unless explicitly revised)
- a commitment to match **every** feature of either backend—expose **capabilities** honestly
- a full **document viewer application** product (the demo is **not** the product)
- a channel for committing **copyrighted PDFs** or **full extracted text** without explicit approval
- a commitment to ship **signed, store-ready application binaries** from this repository by default—**source-first** distribution is the baseline scope (see **README**, **MVP → full ship**); integrators own signing and notices for **their** shipped apps

**Licensing note:** the **choice of PDF engine** may constrain how this library is **released** (e.g. permissive vs copyleft). Record decisions in `docs/` artifacts per governance when stabilizing.

---

## 4. Demo vs Product

- **`examples/`** (or the documented demo target) exists to show **how to wire** the widget: construct, open a document, call documented APIs, quit.
- Do **not** expand the demo into a general-purpose PDF viewer unless requirements explicitly say so.
- **`PdfDocumentViewWidget` + PDFium** is the **intended substitute** for **Qt PDF** in hosts that need **deeper engine capability** than Qt’s PDF module exposes; the **library** does **not** take a dependency on Qt Pdf.
- Historical: we do not use Qt Pdf in the demo. `PdfDocumentViewWidget` is the only PDF surface; menus (and any toolbar) drive the public widget API.
- Host applications own menus, persistence, file dialogs, activation quirks, and deployment.

---

## 5. Copyrighted or Restricted PDF Handling

PDF inputs may include copyrighted material.

Agents MUST:

- treat PDFs as **local inputs** unless a fixture is **explicitly approved** for the repo
- avoid quoting or embedding **substantive** document text in commits, issues, or docs
- use **synthetic** or **clearly licensed** tiny PDFs for automated tests when possible
- keep `.gitignore` and review practices aligned with **no accidental corpus commits**

---

## 6. Architectural Decisions (ADR)

When the project records ADRs, use:

```
docs/adrs/
```

**Accepted** ADRs govern structure; hypothesis ADRs are for exploration only—see toolkit traceability guidance.

---

## 7. Development Model

Follow the **Spiral** emphasis from `ai-toolkit/02-governance/00-lifecycle-bootstrap.md` where applicable: **prototype backend and API boundaries** before freezing a public widget surface.

---

## 8. Guardrail Routing

Apply the **implementation** and **documentation** guardrails from the toolkit when producing code and user-facing docs. Scale **traceability** to project needs (widget library may be lighter than a regulated system, but **public API** and **license** choices still deserve explicit records).

Key files:

- `ai-toolkit/02-governance/05-implementation-guardrail.md`
- `ai-toolkit/02-governance/07-system-documentation-guardrail.md` (when writing system docs)
- `ai-toolkit/02-governance/09-traceability-guardrail.md` (when requirements exist)

---

## 9. Bootstrap Directive (session start)

For non-trivial work:

1. Read `docs/concept-pdf-document-view.md` for current intent.
2. Read `ai-toolkit/02-governance/00-lifecycle-bootstrap.md` and `ai-toolkit/02-governance/guardrails-index.md` if formal artifacts or phase boundaries apply.
3. State assumptions about **backend**, **Qt version**, and **public API** before large changes.

### WBS hygiene (mandatory)

- When an agent completes work that corresponds to a task in `tools/wbs-dashboard/public/wbs-data.json` (or discovers a new tracking need), **update that JSON in the same change series**: set `status`, refresh `notes`, and extend `meta.sourceDocs` when new docs or ADRs apply. Before finishing, run `python3 -m json.tool tools/wbs-dashboard/public/wbs-data.json` so the file is valid JSON (fix any parse errors the command reports).

---

## 10. Prohibited Actions

- treating the **demo app** as the **only** integration contract (the **library API** is the contract)
- hard-coding **one application’s** chrome or file paths into the reusable widget layer
- committing **corpus PDFs** or **extracted full text** without explicit approval
- silently **crossing license boundaries** when adding or swapping backends (document and review)

---

## 11. Final Principle

Prefer **clear embeddable boundaries**, **honest capability reporting**, and **reproducible builds** over speed.

---

END OF FILE

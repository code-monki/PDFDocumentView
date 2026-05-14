# WBS dashboard (local)

Offline-first **work breakdown** and **RTM summary** view for the **PDFDocumentView** repository (embeddable **pdf-document-view** Qt widget library). Data is **checked-in JSON** under `public/` — no network or PDF corpus required.

## When to edit data

- After **`docs/concept-pdf-document-view.md`** changes (goals, layers, backend notes, demo policy), update **`public/wbs-data.json`** so the tree and `meta.criticalPathHint` stay aligned with steering intent.
- When **`docs/adrs/`** gains or changes accepted ADRs that affect scope, refresh **`public/wbs-data.json`** tasks and owners as needed.
- After **`docs/requirements-traceability-matrix.md`** exists and its requirement rows change, update **`public/rtm-summary.json`** (counts and ID ranges). Until that file exists, the RTM tab shows **zero** counts with a stub note.

## Commands

```bash
cd tools/wbs-dashboard
npm install
npm run dev
```

Build (typecheck + production bundle):

```bash
npm run build
```

Preview the production build:

```bash
npm run preview
```

## Files

| Path | Role |
|------|------|
| `public/wbs-data.json` | WBS tree, meta, critical-path hint |
| `public/rtm-summary.json` | FR/NFR counts and ID ranges (trimmed); stub until an RTM doc exists |
| `src/` | React UI |

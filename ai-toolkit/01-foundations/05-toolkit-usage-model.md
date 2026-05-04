<!--
File: 01-foundations/05-toolkit-usage-model.md

Purpose:
  Define how to operationalize the Hybrid Systems Governance Toolkit
  within AI-assisted development workflows.

This document explains:
  - What artifacts are loaded into the AI agent
  - When they are loaded
  - How enforcement changes by lifecycle phase
  - Minimal vs enterprise operating modes
-->

# Toolkit Usage Model

This toolkit is modular.

Not all files are loaded simultaneously.

The lifecycle is phase-gated.
AI configuration must reflect the current phase.

Improper loading leads to:

- Premature convergence
- Scope drift
- Over-constrained ideation
- Unnecessary verbosity
- Structural enforcement gaps

This document prevents that.

---

# 1. Core Bootstrap Set (Always Loaded)

At minimum, load:

- 02-governance/00-lifecycle-bootstrap.md
- toolkit/platforms/<agent-config>.md
- toolkit/platforms/enterprise-system-prompt.md

This establishes:

- Lifecycle sequencing authority
- AI behavioral enforcement
- Refusal protocol
- Rollback rules

No templates are required during ideation.

---

# 2. Phase-Specific Loading Model

## Phase 1 — Ideation

Load:

- Lifecycle Bootstrap
- Enterprise System Prompt
- Platform configuration

Do NOT load:

- SRS template
- Architecture template
- RTM template
- Test plan template
- Packaging template

Purpose:
Allow structured exploration without premature formalization.

---

## Phase 2 — Requirements (SRS)

Load:

- Lifecycle Bootstrap
- SRS template
- Traceability template (optional scaffold)
- Enterprise System Prompt
- Platform config

Do NOT load:

- Architecture template (until approved)
- Implementation artifacts
- Packaging template

Purpose:
Define "what" without embedding structure.

---

## Phase 3 — High-Level Architecture

Load:

- Lifecycle Bootstrap
- Approved SRS
- Architecture template
- RTM template
- Enterprise System Prompt
- Platform config

Purpose:
Map structure to requirements.

---

## Phase 4 — Detailed Design

Load:

- Lifecycle Bootstrap
- Approved SRS
- Approved HLA
- Detailed Design template
- RTM template
- Enterprise System Prompt
- Platform config

Purpose:
Refine structure without altering architecture.

---

## Phase 5 — Traceability Consolidation

Load:

- RTM template
- Approved SRS
- HLA
- Detailed Design
- Lifecycle Bootstrap

Purpose:
Confirm structural completeness before implementation.

---

## Phase 6 — Test Planning

Load:

- Test Plan template
- Approved SRS
- RTM
- Lifecycle Bootstrap
- Enterprise System Prompt

Purpose:
Define validation strategy prior to implementation.

---

## Phase 7 — Implementation

Load:

- Lifecycle Bootstrap
- Approved SRS
- HLA
- Detailed Design
- Test Plan
- RTM
- Platform configuration
- Cursor rules (if using Cursor)

Purpose:
Controlled execution with traceability preservation.

---

## Phase 8 — Packaging and Orchestration

Load:

- Packaging template
- Test Plan
- RTM
- Lifecycle Bootstrap

Purpose:
Enforce reproducibility and release gating.

---

## Phase 9 — Documentation Closure

Load:

- System Documentation template
- RTM release snapshot
- Packaging plan
- Lifecycle Bootstrap

Purpose:
Align delivered system to governance artifacts.

---

# 3. Minimal Mode vs Enterprise Mode

## Minimal Mode

Load:

- Lifecycle Bootstrap
- Platform config
- Current phase template only

Use for:
- Smaller teams
- Internal projects
- Low regulatory pressure

---

## Enterprise Mode

Load:

- Lifecycle Bootstrap
- All relevant guardrails
- Enterprise System Prompt
- Platform configuration
- Active phase template
- RTM (from Architecture onward)

Use for:
- Regulated environments
- External-facing products
- AI-integrated systems
- High audit exposure

---

# 4. Cursor-Specific Rules

Cursor is loaded only during Implementation.

Cursor must NOT be used during:

- Ideation
- Requirements
- Architecture
- Traceability consolidation

Cursor is enforcement for code generation — not lifecycle design.

---

# 5. Deterministic–Probabilistic Systems

If probabilistic components exist:

Beginning at Architecture phase, always load:

- RTM template
- Guardrails governing probabilistic boundaries
- Enterprise System Prompt

Do NOT defer boundary governance to implementation.

Containment is structural.

---

# 6. What Not to Do

Do not:

- Load all templates simultaneously during ideation
- Generate implementation before Detailed Design approval
- Skip RTM alignment prior to implementation
- Package without clean build validation
- Allow AI to redefine scope silently
- Suppress refusal protocol for speed

---

# 7. Operational Summary

The toolkit is:

- Phase-gated
- Artifact-driven
- Traceability-enforced
- Reproducibility-focused
- AI-aware
- Release-gated

It is not a prompt library.

It is a lifecycle control system.

Correct loading is part of governance.

---

End of Toolkit Usage Model
<!--
File: 02-governance/12-phase-gate-checklist.md

Purpose:
  Provide enforceable lifecycle validation gates for hybrid
  deterministic–probabilistic systems.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Phase gates are structural risk controls.
Advancement is prohibited unless criteria are satisfied.
-->

# 12 — Phase Gate Checklist

## 1. Governance Principle

Phase gates are enforceable lifecycle controls.

They SHALL:

- Prevent premature convergence
- Prevent scope drift
- Preserve deterministic–probabilistic boundaries
- Preserve traceability continuity
- Enforce validation discipline
- Preserve reproducibility posture

Skipping gates transfers risk downstream.

Advancement requires explicit human approval.

---

## 2. Lifecycle Sequence (Authoritative Order)

1. Ideation  
2. Requirements (SRS)  
3. High-Level Architecture (HLA)  
4. Detailed Design (DD)  
5. Traceability Consolidation  
6. Test Planning  
7. Implementation  
8. Packaging & Orchestration  
9. Documentation Closure  

No phase skipping is permitted.

---

# Gate 1 → 2  
## Ideation → Requirements

### Mandatory Exit Criteria

- Problem statement formally defined  
- Stakeholders identified  
- Constraints documented  
- Risks identified  
- Success criteria defined  
- Alternative solution paths explored  
- No unresolved ambiguity in problem framing  

If ambiguity persists, advancement is prohibited.

Human approval required.

---

# Gate 2 → 3  
## Requirements → Architecture

### Mandatory Exit Criteria

- Functional requirements defined and uniquely identified  
- Non-functional requirements defined and measurable  
- Constraints documented  
- Assumptions explicitly stated  
- Scope boundaries declared  
- Waiting room items captured  
- Requirement IDs stable  
- Initial RTM scaffold created  

Requirements instability SHALL block architectural work.

Human approval required.

---

# Gate 3 → 4  
## Architecture → Detailed Design

### Mandatory Exit Criteria

- System boundaries declared  
- Deterministic–probabilistic boundaries declared (if applicable)  
- NFR-driven structural rationale documented  
- Architectural components defined  
- Integration points identified  
- Failure posture summarized  
- Deployment model declared  
- Requirement-to-Architecture mapping complete  

If boundary modeling is incomplete, advancement is prohibited.

Human approval required.

---

# Gate 4 → 5  
## Detailed Design → Traceability Consolidation

### Mandatory Exit Criteria

- All architectural components refined  
- Interface contracts defined  
- Data models defined  
- Failure semantics defined  
- Deterministic–probabilistic containment detailed  
- Security posture refined  
- Performance assumptions documented  
- Requirement-to-Design mapping complete  

Design without traceability linkage SHALL block advancement.

Human approval required.

---

# Gate 5 → 6  
## Traceability Consolidation → Test Planning

### Mandatory Exit Criteria

- RTM fully populated (Requirement → Architecture → Design)  
- No orphaned requirements  
- No orphaned architectural components  
- No orphaned design artifacts  
- Version identifier assigned  
- Impact analysis completed for recent changes  

Traceability gaps SHALL block advancement.

Human approval required.

---

# Gate 6 → 7  
## Test Planning → Implementation

### Mandatory Exit Criteria

- Test Strategy defined  
- Test Plan defined  
- Requirement-to-Test mapping complete  
- NFR validation strategy defined  
- Failure scenario coverage defined  
- Deterministic–probabilistic containment validation defined  
- RTM updated with Test Case IDs  

Implementation without defined validation SHALL NOT begin.

Human approval required.

---

# Gate 7 → 8  
## Implementation → Packaging & Orchestration

### Mandatory Exit Criteria

- All approved Requirement IDs implemented  
- No unauthorized scope present  
- Code documentation complete  
- Tests implemented and passing  
- Deterministic–probabilistic containment verified  
- RTM fully updated  
- Clean build succeeds  

Unvalidated implementation SHALL NOT proceed.

Human approval required.

---

# Gate 8 → 9  
## Packaging & Orchestration → Documentation Closure

### Mandatory Exit Criteria

- Packaging automated  
- Orchestration validated  
- Clean environment deployment verified  
- Artifact reproducibility confirmed  
- Version metadata embedded  
- Deterministic–probabilistic boundaries intact in packaged form  
- Observability verified  
- RTM reflects release state  

Packaging without validation SHALL block release.

Human approval required.

---

# Final Gate  
## Documentation Closure → Release Approval

### Mandatory Exit Criteria

- System documentation current  
- Architecture documentation current  
- Deployment documentation current  
- Known limitations documented  
- Change log updated  
- Version identifiers aligned across artifacts  
- RTM version state finalized  
- Audit review completed  

Release SHALL NOT proceed with outdated documentation.

Explicit release approval required.

---

## 3. Orphan Detection Rule

At any gate, the following SHALL block advancement:

- Requirement without downstream mapping  
- Architecture without requirement mapping  
- Design without architecture parent  
- Implementation without requirement reference  
- Test without requirement mapping  
- Packaging configuration undocumented  
- Undeclared probabilistic boundary  

Orphans indicate structural governance failure.

---

## 4. Refusal Protocol

AI SHALL refuse to:

- Advance lifecycle phase without gate validation  
- Ignore traceability gaps  
- Ignore containment boundary gaps  
- Suppress required documentation  
- Declare readiness without satisfying exit criteria  

Refusal preserves lifecycle integrity.

---

## 5. Enforcement Rule

If any exit criterion is unmet:

- Remain in current phase  
- Identify blocking conditions  
- Resolve deficiencies  
- Re-evaluate gate  

Phase progression is a controlled authorization event.

Not a convenience step.

---

End of Checklist
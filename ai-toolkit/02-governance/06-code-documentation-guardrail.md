<!--
File: 02-governance/06-code-documentation-guardrail.md

Purpose:
  Enforce disciplined, governance-aligned in-file documentation
  during Implementation.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

This guardrail governs documentation completeness, traceability continuity,
boundary transparency, and auditability.

Documentation is a structural control artifact.
-->

# 06 — Code Documentation Guardrail

## 1. Purpose

Code documentation is a governance requirement.

Documentation SHALL:

- Preserve architectural intent
- Preserve design rationale
- Maintain traceability continuity
- Clarify deterministic–probabilistic boundaries
- Support auditability
- Support long-term maintainability
- Prevent knowledge concentration risk

Code without aligned documentation is incomplete.

---

## 2. Lifecycle Authority

This guardrail operates within the Implementation phase and is governed by:

`02-governance/00-lifecycle-bootstrap.md`

Implementation SHALL NOT be considered complete unless documentation requirements are satisfied.

---

## 3. Documentation Mandate

Documentation SHALL communicate:

- Intent
- Constraints
- Assumptions
- Invariants
- Failure semantics
- Architectural alignment
- Requirement mapping

Documentation SHALL NOT:

- Restate syntax
- Describe obvious language constructs
- Contain speculative commentary
- Drift from implemented behavior

Documentation is explanatory, not decorative.

---

## 4. File-Level Context (Mandatory)

Every source file SHALL include:

- File purpose statement
- Architectural Component ID reference
- Requirement ID references
- High-level responsibility description
- Notable design constraints
- External dependencies (if non-obvious)
- Environmental assumptions (if applicable)

This preserves architectural continuity and traceability integrity.

Files without contextual documentation SHALL block advancement.

---

## 5. Public Interface Documentation (Mandatory)

All public classes, modules, functions, or APIs SHALL document:

- Purpose
- Inputs
- Outputs
- Side effects
- Error behavior
- Preconditions
- Postconditions
- Determinism expectations (if applicable)
- Security considerations (if relevant)

Public interfaces represent contractual boundaries and SHALL be unambiguous.

Undocumented interfaces are prohibited.

---

## 6. Deterministic–Probabilistic Transparency

If a file interacts with probabilistic components, documentation MUST:

- Identify the probabilistic boundary
- Describe validation harness usage
- Clarify fallback behavior
- Define logging and observability posture
- Document reproducibility constraints (if applicable)

Probabilistic invocation without documentation is prohibited.

Boundary opacity is a governance failure.

---

## 7. Complex Logic Clarification

Where logic is non-trivial, documentation SHALL explain:

- Why the logic exists
- Trade-offs considered
- Performance constraints
- Edge-case handling
- Failure-handling rationale
- Deterministic guarantees (if applicable)

Complex logic without explanation SHALL block advancement.

---

## 8. Invariants and Assumptions

Documentation MUST explicitly state:

- Data invariants
- Ordering expectations
- State assumptions
- Concurrency guarantees
- Idempotency behavior (if applicable)
- External dependency assumptions

Implicit assumptions are prohibited.

Operational risk increases when invariants are undocumented.

---

## 9. Error Handling Rationale

If errors are:

- Transformed
- Suppressed
- Retried
- Wrapped
- Logged without propagation

Documentation SHALL explain:

- Why this behavior exists
- Risk implications
- Recovery expectations
- Observability implications

Error-handling opacity is prohibited.

---

## 10. Documentation Drift Prevention

When code changes:

- Documentation SHALL be reviewed
- Obsolete commentary SHALL be removed
- Modified behavior SHALL be updated
- Traceability references SHALL be validated

Documentation retrofitting is prohibited.

Documentation lag SHALL block phase advancement.

---

## 11. Traceability Embedding

Code documentation MUST reference:

- Requirement ID(s)
- Architectural Component ID
- Detailed Design reference
- Test Case ID(s) (if applicable)

Traceability SHALL be visible at implementation level.

Hidden traceability is unacceptable.

---

## 12. Auditability and Reproducibility

Documentation SHALL support:

- Audit review
- Behavioral reasoning
- Reproducibility analysis
- Failure analysis
- Security review

Undocumented behavior undermines enterprise viability.

---

## 13. Refusal Protocol

AI SHALL refuse to:

- Generate code without documentation scaffolding
- Remove required documentation sections
- Introduce undocumented logic
- Advance phase without documentation alignment
- Suppress traceability references
- Obscure deterministic–probabilistic boundaries

Refusal preserves governance integrity.

---

## 14. Completion Criteria

Implementation documentation is complete only when:

- All files contain required contextual documentation
- Public interfaces are fully documented
- Deterministic–probabilistic boundaries are documented
- Complex logic is explained
- Invariants and assumptions are explicit
- Error-handling rationale is documented
- Traceability references are embedded
- Documentation accurately reflects current behavior
- Human approval is granted

If any condition is unmet, Implementation remains incomplete.

Transition to next lifecycle phase is prohibited without explicit approval.

---

End of Guardrail
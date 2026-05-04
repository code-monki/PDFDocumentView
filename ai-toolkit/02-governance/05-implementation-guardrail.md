<!--
File: 02-governance/05-implementation-guardrail.md

Purpose:
  Enforce disciplined implementation strictly derived from approved
  Requirements, Architecture, and Detailed Design artifacts.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

This guardrail governs execution discipline, traceability preservation,
hybrid boundary enforcement, and scope containment.

Implementation is execution — not redesign.

No phase skipping is permitted.
-->

# 05 — Implementation Guardrail

## 1. Purpose

The Implementation phase translates approved design artifacts into executable software.

Implementation SHALL:

- Execute approved scope only
- Preserve architectural boundaries
- Preserve deterministic–probabilistic containment
- Maintain traceability continuity
- Enforce documentation discipline
- Prepare for verification and packaging alignment

Implementation SHALL NOT:

- Redesign architecture
- Introduce new requirements
- Expand scope
- Collapse modality boundaries
- Bypass traceability controls

Implementation is structural execution under governance control.

---

## 2. Lifecycle Authority

This phase is governed by:

`02-governance/00-lifecycle-bootstrap.md`

Implementation SHALL NOT begin unless:

- SRS is approved
- HLA is approved
- Detailed Design is approved
- Traceability Matrix (RTM) is current
- Test Plan is defined
- Advancement from Detailed Design is authorized

If any artifact is incomplete, implementation is prohibited.

---

## 3. Execution Discipline

Implementation MUST:

- Conform exactly to Detailed Design specifications
- Respect Architectural boundaries
- Implement only approved Requirement IDs
- Maintain structural modularity
- Avoid speculative enhancement
- Avoid premature optimization

If implementation reveals:

- Requirement ambiguity
- Architectural weakness
- Design inconsistency

Work SHALL pause and return to the appropriate lifecycle phase.

Silent correction is prohibited.

---

## 4. Scope Containment

Implementation SHALL NOT:

- Introduce undocumented features
- Add “convenience” behavior
- Perform structural refactoring beyond design scope
- Insert new frameworks or dependencies without requirement constraint
- Expand probabilistic surface area

Scope drift is a governance violation.

All deviations MUST be documented and approved.

---

## 5. Deterministic–Probabilistic Boundary Preservation

If probabilistic components exist, implementation MUST:

- Preserve defined invocation boundaries
- Preserve validation harness logic
- Enforce acceptance/rejection criteria
- Maintain deterministic state protection
- Log probabilistic outputs
- Preserve observability hooks
- Enforce fallback mechanisms

Probabilistic components SHALL NOT:

- Write directly to persistent state without validation
- Bypass deterministic verification layers
- Operate without instrumentation
- Modify system control flow beyond defined contracts

Boundary collapse during implementation is prohibited.

---

## 6. Coding Discipline

Code MUST:

- Reflect design intent precisely
- Maintain modular isolation
- Avoid cross-layer leakage
- Respect interface contracts
- Enforce input validation
- Implement defined error semantics

All code SHALL:

- Follow documented coding standards
- Maintain clarity over cleverness
- Preserve reproducibility posture
- Avoid hidden side effects

Unreviewable code is unacceptable.

### 6.1 Defensive Checks Contract (Mandatory)

Implementation MUST treat **trust boundaries** (module entry points, callbacks, FFI, OS APIs, operator-supplied paths, and any value not established by local invariant) as **unsafe until validated**.

At those boundaries, code SHALL:

- **Pointers and handles:** Verify non-null / valid before dereference or use when the origin is external or not guaranteed by the immediate caller contract. When null or invalid is a legitimate outcome, branch explicitly; do not assume invariants that are not enforced by the type system.
- **Optionals and fallible results:** Do not read `optional` / `expected` / similar values without proving presence or success; do not ignore error channels.
- **Preconditions:** Public and cross-module APIs SHALL validate arguments (emptiness, ranges, encoding, path semantics) or state explicit preconditions and enforce them with checks that fail closed (safe error return, safe message, or defined abort policy as approved by design).
- **External and operator input:** Filesystem paths, environment variables, configuration fields, subprocess output, and sized buffers SHALL be validated or bounded before use where misuse could cause crashes, UB, or security defects.
- **Failure mode:** On violation, stop the affected operation with **controlled, observable behavior** (typed errors, safe operator messages without leaking sensitive content, logging within defined boundaries). **Silent continuation** past broken invariants is prohibited.

Relying on informal “this should never be null” without an explicit check at the boundary is a governance violation unless the Detailed Design documents a stronger, machine-enforced guarantee (e.g., type-level non-null) and implementation conforms to it.

---

## 7. In-File Documentation Requirements

Each file MUST include:

- File-level purpose declaration
- Architectural context reference
- Requirement ID references
- Public interface documentation
- Description of non-obvious logic
- Assumption statements
- Constraint declarations

Documentation SHALL support auditability and long-term maintainability.

Undocumented behavior is prohibited.

---

## 8. Traceability Enforcement

Every implemented unit MUST map to:

- Requirement ID
- Architectural Component ID
- Detailed Design reference
- Test Case ID

Traceability SHALL be updated continuously.

RTM gaps SHALL block phase advancement.

Traceability retrofitting after implementation is prohibited.

---

## 9. Test-Aware Development

Implementation MUST align with defined Test Plan.

Code SHALL:

- Be testable
- Support defined unit/integration tests
- Avoid introducing untestable constructs
- Avoid bypassing validation harnesses

Test evasion is prohibited.

Test alignment is mandatory before advancement.

---

## 10. Observability and Auditability

Implementation MUST preserve:

- Logging boundaries
- Error reporting behavior
- Monitoring hooks
- Deterministic replay capability (where applicable)
- Reproducibility posture

Hidden execution paths are prohibited.

---

## 11. Orchestration Alignment

Implementation SHALL:

- Integrate with defined build system
- Respect dependency declarations
- Preserve configuration discipline
- Support defined deployment model
- Avoid environment-specific assumptions unless specified

Build fragility is a governance failure.

---

## 12. Packaging Awareness

Implementation MUST consider:

- Deployment targets
- Runtime dependencies
- Platform constraints
- Configuration management posture
- Distribution reproducibility

Packaging SHALL NOT be deferred.

---

## 13. Refusal Protocol

AI SHALL refuse to:

- Generate code without approved artifacts
- Introduce new scope
- Modify architecture during implementation
- Skip traceability mapping
- Bypass validation layers
- Collapse deterministic–probabilistic containment
- Advance phase without explicit human authorization

Refusal preserves lifecycle integrity.

---

## 14. Completion Criteria

Implementation phase is complete only when:

- All approved Requirement IDs are implemented
- No unauthorized scope exists
- Deterministic–probabilistic boundaries remain intact
- RTM is fully updated
- Test alignment is verified
- Documentation is complete
- Build system executes successfully
- Packaging build succeeds
- Human approval is granted

If any condition is unmet, remain in Implementation.

No transition to Packaging or Validation is authorized without explicit approval.

---

End of Guardrail
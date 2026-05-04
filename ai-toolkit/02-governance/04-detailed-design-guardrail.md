<!--
File: 02-governance/04-detailed-design-guardrail.md

Purpose:
  Enforce disciplined Detailed Design (DD) derived strictly from
  approved High-Level Architecture.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

This guardrail governs component-level behavioral precision,
contract definition, hybrid boundary enforcement, and traceability continuity.

No phase skipping is permitted.
-->

# 04 — Detailed Design Guardrail (DD)

## 1. Purpose

The Detailed Design phase translates approved High-Level Architecture
into precise, reviewable behavioral specifications suitable for implementation.

Detailed Design SHALL:

- Refine architectural components into executable-level specifications
- Eliminate behavioral ambiguity
- Formalize interface contracts
- Define data models and invariants
- Specify control and failure flows
- Preserve deterministic–probabilistic boundary discipline
- Maintain traceability integrity

Detailed Design is a control artifact.

Implementation is prohibited in this phase.

---

## 2. Lifecycle Authority

This phase is governed by:

`02-governance/00-lifecycle-bootstrap.md`

Detailed Design SHALL NOT begin unless:

- Architecture phase has been approved
- Architectural components are stable
- NFR-driven structural rationale is documented
- Requirement-to-Architecture mapping is complete
- Advancement from Architecture has been authorized

If these conditions are unmet, design work is prohibited.

---

## 3. Design Mandate

Detailed Design MUST:

1. Refine each architectural component
2. Preserve structural constraints
3. Maintain requirement traceability
4. Prevent architectural drift
5. Enforce hybrid boundary containment
6. Prepare for test planning alignment

Design SHALL NOT:

- Introduce new architectural components
- Modify system boundaries
- Insert frameworks without requirement constraint
- Optimize prematurely
- Collapse modality boundaries

---

## 4. Component-Level Refinement

For each architectural component, Detailed Design SHALL define:

- Internal responsibilities
- Public interfaces
- Data ownership boundaries
- Collaboration contracts
- Behavioral invariants
- Error semantics

Each component MUST reference:

- Its parent Architectural Component ID
- The Requirement IDs it implements

Unmapped design elements SHALL block advancement.

---

## 5. Interface Contracts

All interfaces MUST specify:

- Conceptual method/function signatures
- Input structures and validation expectations
- Output structures and guarantees
- Error behavior
- Preconditions
- Postconditions
- Side effects
- Determinism expectations (if applicable)

Implicit contracts are prohibited.

Contract ambiguity is a governance failure.

---

## 6. Data Model Specification

Detailed Design SHALL define:

- Data schemas
- Data lifecycle
- Validation rules
- Transformation logic
- Persistence model (if applicable)
- Serialization expectations
- Audit logging posture (if required)

Data invariants MUST be explicit.

Uncontrolled schema evolution is prohibited.

---

## 7. Interaction and Control Flow

Design SHALL document:

- Nominal flows
- Exception flows
- Cross-component interaction paths
- Retry behavior (if applicable)
- Degradation strategy
- Escalation behavior
- Rollback posture

Hidden control logic is prohibited.

Behavioral ambiguity SHALL block advancement.

---

## 8. State and Concurrency Discipline

If state exists, design MUST define:

- State lifecycle
- State transitions
- Invariants
- Concurrency expectations
- Synchronization requirements
- Isolation boundaries
- Idempotency behavior (if relevant)

Implicit state is prohibited.

Concurrency assumptions MUST be documented.

---

## 9. Performance Derivation

Performance behavior SHALL be derived from NFRs.

Design MUST document:

- Expected complexity characteristics
- Resource utilization assumptions
- Identified bottlenecks
- Caching posture (if required)
- Scaling assumptions

Performance speculation without NFR basis is prohibited.

---

## 10. Security Derivation

Security behavior SHALL be explicit.

Design MUST define:

- Authentication boundary
- Authorization enforcement points
- Input validation strategy
- Data protection posture
- Error exposure policy
- Audit logging boundaries

Security SHALL NOT be deferred to implementation.

---

## 11. Hybrid Deterministic–Probabilistic Enforcement

If probabilistic subsystems exist, Detailed Design MUST:

- Preserve architectural boundary definitions
- Specify invocation contract
- Define validation harness logic
- Define acceptance criteria
- Define rejection and fallback behavior
- Define monitoring and logging strategy
- Define reproducibility posture (where applicable)

Probabilistic components SHALL NOT:

- Bypass validation layers
- Self-authorize outputs
- Modify deterministic state without validation
- Operate without observability

Boundary collapse is prohibited.

---

## 12. Failure Semantics

Design MUST refine architectural failure modeling into:

- Specific failure triggers
- Error propagation paths
- Retry limits
- Circuit-breaker logic (if applicable)
- Degradation behavior
- Recovery orchestration hooks

Failure handling SHALL be deterministic wherever possible.

Undefined failure behavior SHALL block advancement.

---

## 13. Traceability Continuity

Detailed Design MUST:

- Reference Requirement IDs
- Reference Architectural Component IDs
- Establish forward linkage for Test Case IDs
- Maintain RTM continuity

Traceability gaps SHALL prohibit advancement.

---

## 14. Refusal Protocol

AI SHALL refuse to:

- Generate implementation code in this phase
- Introduce new architecture
- Collapse deterministic–probabilistic boundaries
- Proceed without traceability mapping
- Skip interface specification
- Defer error handling
- Advance phase without explicit human approval

Refusal preserves lifecycle discipline.

---

## 15. Advancement Criteria

Detailed Design is complete only when:

- All architectural components are refined
- Interfaces are fully specified
- Data models are explicit
- Nominal and failure flows are documented
- State and concurrency behavior is defined (if applicable)
- Performance derivation is documented
- Security behavior is specified
- Hybrid boundaries are preserved and detailed
- Traceability mapping is updated
- Human approval is granted

If any condition is unmet, remain in Detailed Design.

Transition to Test Planning requires explicit authorization.

---

End of Guardrail
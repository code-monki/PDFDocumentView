<!--
File: 02-governance/03-architecture-guardrail.md

Purpose:
  Enforce disciplined, requirement-derived High-Level Architecture (HLA)
  for hybrid deterministic–probabilistic systems.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

This guardrail governs architectural rigor, NFR-driven structure,
modality boundary discipline, and traceability enforcement.

No phase skipping is permitted.
-->

# 03 — Architecture Guardrail (High-Level Architecture)

## 1. Purpose

The Architecture phase defines the structural model of the system.

Architecture SHALL:

- Translate approved requirements into structural form
- Encode non-functional constraints into topology
- Define deterministic–probabilistic boundaries
- Establish failure containment strategy
- Create forward traceability anchors

Architecture is a control artifact — not a design sketch.

No implementation detail, behavioral logic, or framework selection is authorized in this phase.

---

## 2. Lifecycle Authority

This phase is governed by:

`02-governance/00-lifecycle-bootstrap.md`

Architecture SHALL NOT begin unless:

- Requirements have been formally approved
- Requirement IDs are stable
- NFRs are explicitly enumerated
- Advancement from Requirements phase has been authorized

If these conditions are not met, architectural work is prohibited.

---

## 3. Architectural Mandate

The High-Level Architecture SHALL define:

1. System boundaries
2. Major structural components
3. Responsibility allocation
4. Integration surfaces
5. Data movement topology
6. Dependency model
7. Failure containment strategy
8. Modality boundaries (deterministic vs probabilistic)

Architecture MUST be defensible, reviewable, and traceable.

---

## 4. System Boundary Declaration

The architecture SHALL explicitly define:

- What is inside system control
- What is outside system control
- External actors
- External systems
- Trust boundaries
- Security boundaries
- Operational boundaries

Unclear boundaries introduce integration and governance risk and SHALL block advancement.

---

## 5. NFR-Driven Structural Derivation

Architecture MUST be derived from Non-Functional Requirements.

The structure SHALL explicitly demonstrate how it enforces:

- Performance constraints
- Reliability targets
- Security posture
- Auditability
- Observability
- Reproducibility
- Scalability constraints
- Availability targets

NFRs drive structure. Structure does not invent NFRs.

If an architectural element cannot be traced to a functional or non-functional requirement, it SHALL be removed or justified.

---

## 6. Hybrid Deterministic–Probabilistic Boundary Discipline

If probabilistic components exist, the architecture MUST:

- Identify each probabilistic subsystem
- Declare its boundary explicitly
- Define its input/output contract
- Define validation harness strategy
- Define fallback behavior
- Define containment strategy
- Define observability mechanism
- Define drift detection posture (if applicable)

Probabilistic subsystems SHALL NOT be embedded implicitly.

There SHALL be no silent modality blending.

If probabilistic insertion is proposed without boundary modeling, advancement is prohibited.

---

## 7. Component Responsibility Model

For each major component, architecture SHALL define:

- Structural role
- Responsibility scope
- Data ownership
- Interface surface
- Dependency constraints

Components SHALL exist to satisfy one or more Requirement IDs.

Unjustified structural elements SHALL block advancement.

---

## 8. Data Flow Topology

Architecture SHALL describe:

- Input ingress points
- Output egress points
- Internal data movement paths
- Cross-boundary exchanges
- Trust transitions
- Validation checkpoints

Data flow SHALL be structurally coherent and reviewable.

Hidden data movement is prohibited.

---

## 9. Dependency Model

Architecture MUST identify:

- Internal dependencies
- External systems
- Third-party services
- Operational assumptions
- Critical-path elements
- Failure-amplifying dependencies

Dependency blindness is a governance failure.

---

## 10. Failure Modeling Requirements

Architecture SHALL include high-level modeling of:

- Deterministic failure modes
- Probabilistic failure modes
- Integration failures
- Data corruption risks
- Capacity constraints
- Degradation behavior
- Recovery posture
- Rollback strategy

Failure modeling at architecture level is mandatory.

If failure posture is undefined, architecture is incomplete.

---

## 11. Packaging and Orchestration Awareness

Architecture SHALL consider:

- Deployment model (monolith, modular, service, library, hybrid)
- Target execution environments
- Build and packaging constraints
- Runtime orchestration posture
- Configuration boundaries

Framework or vendor selection is NOT authorized unless required by explicit requirement constraints.

Tool-first architecture is prohibited.

---

## 12. Traceability Enforcement

Each major architectural element MUST:

- Reference supporting Requirement IDs
- Establish forward linkage placeholder for Detailed Design
- Anchor to Traceability Matrix (RTM)

Architecture without traceability is invalid.

If mapping between requirements and structural elements is incomplete, advancement SHALL be blocked.

---

## 13. Refusal Protocol

AI SHALL refuse to:

- Generate architecture without approved requirements
- Propose frameworks before structural model exists
- Insert probabilistic systems without boundary definition
- Optimize performance before structure is defined
- Skip failure modeling
- Proceed without explicit NFR articulation
- Advance phase without human authorization

Refusal is required to preserve lifecycle discipline.

---

## 14. Advancement Criteria

The Architecture phase is complete only when:

- System boundaries are declared
- Major components are defined
- NFR-driven structural rationale is explicit
- Deterministic–probabilistic boundaries are modeled
- Data flows are documented
- Dependencies are identified
- Failure modeling is recorded
- Requirement mapping is explicit
- Trade-offs are documented
- Human approval is granted

If any condition is unmet, remain in Architecture phase.

No transition to Detailed Design is authorized without explicit approval.

---

End of Guardrail
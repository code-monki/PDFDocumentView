<!--
File: 02-governance/02-requirements-guardrail.md

Purpose:
  Ensure disciplined, complete, and traceable requirements definition
  prior to architectural design or implementation.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md
-->

# Requirements Phase Guardrail

You are currently in the **Requirements Phase**.

The purpose of this phase is to define what must be built — clearly, completely, and without prematurely defining how it will be built.

Requirements define intent.  
Architecture and design define realization.

Architecture must not begin until Requirements receive explicit human approval.

---

# Phase Objectives

The Requirements Phase must:

- Define functional requirements.
- Define non-functional requirements (NFRs).
- Identify explicit constraints.
- Document assumptions.
- Define scope boundaries.
- Establish traceability foundations.

Requirements must be sufficiently complete to guide architecture without reinterpretation.

Ambiguity deferred to later phases becomes rework.

---

# Functional Requirements

Each functional requirement must:

- Be clearly and unambiguously stated.
- Describe externally observable behavior.
- Be testable.
- Include acceptance criteria.
- Avoid implementation detail.
- Be assigned a unique, stable identifier.

Functional requirements must not:

- Contain architectural decisions.
- Contain technology selections (unless mandated by constraint).
- Embed design-level structure.

If implementation detail appears, refactor the requirement.

---

# Non-Functional Requirements (NFRs)

Non-functional requirements must be explicitly defined.

These may include:

- Performance targets
- Scalability expectations
- Reliability and availability requirements
- Security and compliance constraints
- Auditability requirements
- Maintainability constraints
- Observability and monitoring requirements
- Usability expectations
- Deployment environment assumptions

NFRs frequently drive architecture.

Failure to define them early increases architectural instability.

---

# Constraints

Constraints must be documented explicitly.

Examples include:

- Mandated technology stack
- Regulatory requirements
- Integration dependencies
- Timeline constraints
- Budgetary limitations
- Infrastructure boundaries
- Organizational policy constraints

Constraints shape feasible architecture and must be visible before design begins.

---

# Assumptions

All assumptions must be:

- Explicitly stated
- Testable or verifiable where possible
- Flagged if uncertain

Unstated assumptions become hidden risks.

---

# Scope Control

The Requirements Phase must define:

- Explicit in-scope functionality
- Explicit out-of-scope functionality
- Deferred items

Scope boundaries must be clear and enforceable.

Scope creep during later phases indicates requirements weakness.

---

# Waiting Room Discipline

Not all ideas belong in the current cycle.

A clearly defined “Waiting Room” section must capture:

- Deferred features
- Future enhancements
- Exploratory ideas
- Non-validated suggestions

This preserves focus without discarding insight.

---

# Traceability Foundations

Each requirement must:

- Have a unique identifier.
- Be structured to support mapping to:
  - Architecture components
  - Design artifacts
  - Test cases
  - Implementation units

Traceability begins here.

If traceability cannot be established, the requirement is incomplete.

---

# Prohibited Activities

During this phase, the AI agent must not:

- Define architectural decomposition.
- Select specific technologies (unless constrained).
- Produce design artifacts.
- Produce implementation-level artifacts.
- Collapse Requirements into Architecture.

If architectural reasoning begins to dominate, surface the phase violation.

Progression requires approval.

---

# Advancement Conditions

The Requirements Phase is complete only when:

- Functional requirements are defined and testable.
- Non-functional requirements are defined.
- Constraints are documented.
- Assumptions are captured.
- Scope boundaries are explicit.
- Waiting room items are recorded.
- Unique requirement IDs are assigned.
- Traceability structure is established.
- Explicit human approval is granted.

Architecture must not begin until Requirements are formally approved.
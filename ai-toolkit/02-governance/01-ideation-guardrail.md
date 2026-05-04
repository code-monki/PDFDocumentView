<!--
File: 02-governance/01-ideation-guardrail.md

Purpose:
  Enforce disciplined ideation prior to formal requirements and architecture.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

This guardrail governs structured problem exploration.
-->

# Ideation Phase Guardrail

You are currently in the **Ideation Phase**.

The purpose of this phase is to deeply understand the problem space before formalizing requirements or defining architecture.

This phase focuses on clarity, alignment, constraint visibility, and risk identification.

No architectural commitment or implementation planning is permitted in this phase.

Progression requires explicit human approval.

---

# Ideation Objectives

The Ideation Phase must establish:

- A clearly articulated problem statement
- Strategic relevance of the problem
- Defined conceptual success criteria
- Stakeholder visibility
- Constraint visibility
- Identified risks
- Multiple viable conceptual solution directions

Ideation is complete when the problem is understood — not when a solution is selected.

---

# Required Exploration Areas

## 1. Problem Framing

Define:

- Clear and bounded problem statement
- Desired outcomes
- Conceptual success criteria
- Current state versus desired state
- Known failure patterns (if applicable)

Ambiguity must be surfaced, not resolved prematurely.

---

## 2. Context Analysis

Explore:

- Business context
- Technical environment
- Organizational considerations
- Regulatory or compliance considerations (if applicable)
- Existing systems or processes
- Known integration dependencies

Context visibility reduces downstream rework.

---

## 3. Stakeholder Identification

Identify:

- Primary stakeholders
- Secondary stakeholders
- Decision authorities
- Potentially conflicting priorities
- Operational impact

Stakeholder visibility reduces misalignment risk.

---

## 4. Constraint Identification

Surface:

- Technical constraints
- Resource constraints
- Timeline expectations
- Integration boundaries
- Governance requirements
- Budgetary constraints (if applicable)

Constraints must be explicit before Requirements begin.

---

## 5. Preliminary Non-Functional Considerations

At a conceptual level, consider:

- Performance sensitivity
- Security implications
- Reliability expectations
- Scalability implications
- Maintainability concerns
- Auditability expectations

These are conceptual signals, not formal non-functional requirements.

---

## 6. Alternative Solution Directions

Explore at least two viable conceptual approaches (where applicable).

For each:

- Identify advantages
- Identify risks
- Identify unknowns
- Identify structural trade-offs

Premature convergence is prohibited.

Selection occurs in later phases.

---

# Risk Identification

Document:

- Technical risks
- Organizational risks
- Integration risks
- Governance risks
- Uncertainty domains
- External dependencies

Risk surfacing is mandatory.

---

# Prohibited Activities

During Ideation, the AI agent must not:

- Select final architecture
- Define detailed component structure
- Select technologies unless explicitly constrained
- Produce implementation-level artifacts
- Collapse Ideation into Requirements

If such actions are requested, the AI agent must surface the phase violation.

---

# Advancement Conditions

The Ideation Phase is complete only when:

- The problem is clearly articulated.
- Stakeholders are identified.
- Constraints are documented.
- Risks are surfaced.
- Alternative solution directions are explored.
- Conceptual success criteria are defined.

If significant ambiguity remains, remain in Ideation.

Explicit human approval is required before entering the Requirements Phase.
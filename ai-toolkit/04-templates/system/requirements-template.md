<!--
File: 04-templates/system/requirements-template.md

Purpose:
  Provide enforceable Software Requirements Specification (SRS)
  aligned with lifecycle governance and traceability discipline
  for hybrid deterministic–probabilistic systems.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Requirements define intent.
They SHALL NOT encode architecture or implementation.
-->

# Software Requirements Specification (SRS)

Project Name:  
Version:  
Date (YYYY-MM-DD):  
Author(s):  
Status: Draft / Approved  
Project Primer Version Reference:  
RTM Scaffold Version:  

---

# 1. Requirements Authority Declaration

Confirm:

- Ideation phase approved? (Yes / No)  
- Project Primer stable? (Yes / No)  
- Scope boundaries defined? (Yes / No)  
- Advancement to Requirements authorized? (Yes / No)  

If any answer is “No,” requirements definition is prohibited.

---

# 2. Introduction

## 2.1 Purpose

Define:

- Purpose of this document  
- System being specified  
- Intended audience  
- Lifecycle context  

---

## 2.2 Scope

Define explicitly:

- High-level system boundaries  
- Major capabilities  
- Explicit out-of-scope items  
- Ownership boundaries  

Scope ambiguity SHALL block advancement.

---

# 3. System Overview

## 3.1 Problem Summary

Reference approved Project Primer.

Summarize:

- Problem being addressed  
- Business or operational context  
- Stakeholder impact  

No solution description permitted.

---

## 3.2 Definitions and Terminology

Define:

- Domain-specific terminology  
- Acronyms  
- Key concepts  

Ambiguous terminology SHALL be resolved before advancement.

---

# 4. Functional Requirements

Each Functional Requirement SHALL:

- Be uniquely identified (FR-001, FR-002, etc.)  
- Be atomic  
- Be testable  
- Avoid implementation detail  
- Avoid architectural commitments  
- Include measurable acceptance criteria  
- Avoid ambiguous language (“fast,” “robust,” “efficient,” etc.)  

---

## Functional Requirement Template

**Requirement ID:** FR-XXX  
**Title:**  
**Description:**  
**Acceptance Criteria:**  
**Priority:** High / Medium / Low  
**Dependencies:**  
**Constraints:**  
**Notes:**  

Acceptance criteria SHALL be testable without architectural knowledge.

---

# 5. Non-Functional Requirements (NFRs)

Each NFR SHALL:

- Be uniquely identified (NFR-001, etc.)  
- Be measurable or objectively evaluable  
- Avoid vague qualifiers  
- Influence architectural direction without prescribing structure  

---

## Non-Functional Requirement Template

**Requirement ID:** NFR-XXX  
**Category:** Performance / Security / Reliability / Scalability / Maintainability / Usability / Observability / Compliance / Reproducibility / Other  
**Description:**  
**Measurement Criteria:**  
**Constraints:**  
**Dependencies:**  

NFRs frequently drive structural decisions.

Assumed NFR compliance is prohibited.

---

# 6. Deterministic–Probabilistic Requirements (If Applicable)

If probabilistic components are anticipated, define:

- Probabilistic behavior expectations  
- Acceptable output variability boundaries  
- Validation expectations  
- Containment expectations  
- Fallback requirements  
- Observability requirements  
- Reproducibility expectations  

Probabilistic behavior SHALL be explicitly bounded at requirement level.

Unbounded probabilistic scope is prohibited.

---

# 7. Constraints

Document explicitly:

- Technology constraints  
- Regulatory requirements  
- Infrastructure limitations  
- Integration requirements  
- Organizational constraints  
- Budget or timeline constraints  

Constraints SHALL be visible before architecture.

---

# 8. Assumptions

Document:

- Environmental assumptions  
- Usage assumptions  
- Dependency assumptions  
- Operational assumptions  

Unvalidated assumptions SHALL be tracked as risks.

---

# 9. Interfaces and External Dependencies

Define:

- External systems  
- APIs  
- Data exchanges  
- User interfaces  
- Hardware interfaces (if applicable)  

Architectural structure SHALL NOT be defined here.

---

# 10. Data Requirements

Describe:

- Data types  
- Persistence requirements  
- Retention expectations  
- Compliance constraints  
- Privacy requirements  
- Audit requirements  

Data handling expectations SHALL be explicit.

---

# 11. Error Handling and Edge Conditions

Identify:

- Expected failure conditions  
- Boundary conditions  
- Invalid input handling expectations  
- Degradation behavior expectations  

Failure expectations SHALL be testable.

---

# 12. Security and Compliance Requirements

Document:

- Authentication expectations  
- Authorization expectations  
- Data protection requirements  
- Audit logging requirements  
- Regulatory obligations  

Security SHALL be defined at requirement level.

---

# 13. Waiting Room (Deferred Scope)

List explicitly:

- Deferred features  
- Enhancements outside current release  
- Experimental ideas  

Waiting room items SHALL NOT appear in architecture or design.

---

# 14. Risk Assessment

Identify:

- Requirement ambiguity risk  
- Scope drift risk  
- Regulatory risk  
- Probabilistic integration risk (if applicable)  
- Dependency risk  

Classify each risk:

- Low  
- Moderate  
- High  

High-risk requirements SHALL be addressed prior to architectural modeling.

---

# 15. Traceability Readiness Declaration

Confirm:

- All requirements uniquely identified  
- Acceptance criteria defined  
- No architectural decisions embedded  
- No implementation bias present  
- Waiting room separated  
- Probabilistic requirements bounded (if applicable)  
- RTM scaffold updated  

Traceability gaps SHALL block advancement.

---

# 16. Phase Gate Declaration

Confirm readiness to proceed to Architecture:

- Requirements stable? (Yes / No)  
- NFRs measurable? (Yes / No)  
- Scope boundaries explicit? (Yes / No)  
- Traceability scaffold prepared? (Yes / No)  
- Human approval granted? (Yes / No)  

If any answer is “No,” remain in Requirements phase.

---

# Approval

Approved By:  
Role:  
Date:  
Version Incremented: Yes / No  

Advancement to High-Level Architecture requires explicit authorization per lifecycle governance.

---

End of Requirements Template
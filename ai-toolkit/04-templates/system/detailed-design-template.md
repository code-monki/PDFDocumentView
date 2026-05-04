<!--
File: 04-templates/system/detailed-design-template.md

Purpose:
  Provide enforceable Detailed Design (DD) structure aligned with
  approved High-Level Architecture and Requirements for hybrid
  deterministic–probabilistic systems.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Detailed Design refines structure.
It does not authorize architectural modification or implementation.
-->

# Detailed Design (DD)

Project Name:  
Version:  
Date (YYYY-MM-DD):  
Author(s):  
Status: Draft / Approved  
Architecture Version Reference:  
Requirement Version Reference:  
RTM Version Reference:  

---

# 1. Design Authority Declaration

Confirm:

- Architecture phase approved? (Yes / No)  
- Architectural Component IDs stable? (Yes / No)  
- NFR structural intent documented? (Yes / No)  
- Advancement to Detailed Design authorized? (Yes / No)  

If any answer is “No,” detailed design work is prohibited.

---

# 2. Scope of This Design

Define explicitly:

- Architectural Component ID(s) being refined  
- Associated Requirement ID(s)  
- Boundaries of this document  
- Explicit exclusions  

This document SHALL NOT introduce new architectural components.

Architectural change requires rollback to Architecture phase.

---

# 3. Architectural Conformance

Reference:

- Approved HLA version  
- Relevant structural constraints  
- Dependency rules  
- Deterministic–probabilistic boundaries  

Confirm:

- No architectural boundaries altered  
- No new external dependencies introduced  

If alteration is required, halt and escalate.

---

# 4. Component Decomposition

For each Architectural Component:

---

## 4.1 Component Identifier

Component ID:  
Associated Requirement ID(s):  

---

## 4.2 Responsibilities

Define precisely:

- What the component SHALL do  
- What the component SHALL NOT do  
- Invariants maintained  

Responsibility overlap is prohibited.

---

## 4.3 Interface Contracts

Define explicitly:

- Public interfaces  
- Input contracts  
- Output contracts  
- Data structures  
- Validation logic  
- Error semantics  
- Preconditions  
- Postconditions  
- Determinism expectations  

Implicit contracts are prohibited.

---

## 4.4 Deterministic–Probabilistic Refinement (If Applicable)

If component interacts with probabilistic subsystem:

- Invocation contract  
- Validation harness logic  
- Acceptance criteria  
- Rejection criteria  
- Fallback behavior  
- Logging and observability  
- Reproducibility constraints  

Containment refinement SHALL preserve architectural boundaries.

Boundary collapse is prohibited.

---

## 4.5 Internal Structure

Describe:

- Internal modules or classes  
- Control flow patterns  
- Concurrency strategy  
- Synchronization model  
- Isolation boundaries  
- Idempotency behavior  

Implementation SHALL conform to this structure.

---

# 5. Data Design

Define:

- Data models  
- Schemas  
- Validation rules  
- Transformation logic  
- Persistence behavior  
- Serialization formats  
- Data lifecycle  
- Data ownership  

Hidden data movement is prohibited.

---

# 6. Failure Semantics

Document explicitly:

- Failure triggers  
- Error categories  
- Retry logic  
- Circuit-breaker behavior (if applicable)  
- Degradation posture  
- Recovery strategy  
- Escalation paths  

Undefined failure behavior SHALL block advancement.

---

# 7. Non-Functional Derivation

For this component, demonstrate structural enforcement of:

- Performance constraints  
- Scalability posture  
- Reliability expectations  
- Security controls  
- Audit logging  
- Observability hooks  
- Configuration discipline  
- Reproducibility controls  

Each NFR SHALL map to structural design elements.

Assumed compliance is prohibited.

---

# 8. Testing Alignment

Define:

- Unit Test Case ID(s)  
- Integration Test Case ID(s)  
- Failure scenario tests  
- NFR validation approach  
- Probabilistic containment tests (if applicable)  

Testing intent SHALL be explicit before implementation.

---

# 9. Packaging and Orchestration Impact

Identify:

- Runtime dependencies introduced  
- Configuration changes  
- Build process impact  
- Clean build implications  
- Packaging configuration impact  
- Pipeline adjustments required  

Design SHALL support reproducible packaging.

---

# 10. Traceability Summary

Confirm mapping between:

- Requirement ID(s)  
- Architectural Component ID(s)  
- Detailed Design section(s)  
- Planned Implementation units  
- Planned Test Case ID(s)  

No orphaned scope SHALL exist.

Traceability gaps block advancement.

---

# 11. Risk Assessment

Identify:

- Complexity risk  
- Performance risk  
- Security risk  
- Integration risk  
- Containment risk (if probabilistic components exist)  
- Operational risk  

Classify:

- Low  
- Moderate  
- High  

High-risk items SHALL require mitigation before implementation.

---

# 12. Open Questions

List:

- Design uncertainties  
- Validation dependencies  
- External constraints  
- Revisit triggers  

Unresolved high-impact issues SHALL block advancement.

---

# 13. Phase Gate Declaration

Confirm readiness to proceed to Implementation:

- All components decomposed? (Yes / No)  
- Interface contracts complete? (Yes / No)  
- NFR derivation explicit? (Yes / No)  
- Deterministic–probabilistic refinement complete? (Yes / No)  
- Traceability mapping complete? (Yes / No)  
- Human approval granted? (Yes / No)  

If any answer is “No,” remain in Detailed Design.

---

# Approval

Approved By:  
Role:  
Date:  
Version Incremented: Yes / No  

---

End of Detailed Design Template
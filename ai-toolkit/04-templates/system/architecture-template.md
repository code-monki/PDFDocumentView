<!--
File: 04-templates/system/architecture-template.md

Purpose:
  Provide enforceable High-Level Architecture (HLA) structure
  aligned with approved Requirements and lifecycle governance
  for hybrid deterministic–probabilistic systems.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Architecture encodes structural constraints.
It does not authorize implementation.
-->

# High-Level Architecture (HLA)

Project Name:  
Version:  
Date (YYYY-MM-DD):  
Author(s):  
Status: Draft / Approved  
Requirement Version Reference:  
RTM Version Reference:  

---

# 1. Architectural Authority Declaration

Confirm:

- Requirements phase approved? (Yes / No)  
- Requirement IDs stable? (Yes / No)  
- NFRs defined and measurable? (Yes / No)  
- Advancement to Architecture authorized? (Yes / No)  

If any answer is “No,” architectural definition is prohibited.

---

# 2. Architecture Overview

## 2.1 System Purpose

Summarize:

- System function  
- Problem addressed  
- Alignment with Requirement IDs  
- Scope boundaries  

This section SHALL reference Requirement Version.

---

## 2.2 Architectural Drivers

Identify:

- Key functional Requirement IDs  
- Key non-functional Requirement IDs  
- Regulatory constraints  
- Integration constraints  
- Deployment constraints  

Each architectural decision SHALL trace to one or more drivers.

Unjustified structural elements are prohibited.

---

# 3. System Context and Boundaries

## 3.1 External Systems and Interfaces

Document:

- External integrations  
- Data exchange mechanisms  
- API boundaries  
- User interaction boundaries  
- Trust boundaries  

Include diagram where appropriate.

---

## 3.2 System Scope Boundaries

Define explicitly:

- In-scope responsibilities  
- Out-of-scope responsibilities  
- Ownership boundaries  
- Responsibility demarcation  

Boundary ambiguity SHALL block advancement.

---

# 4. Deterministic–Probabilistic Boundary Model (If Applicable)

If probabilistic subsystems are anticipated or present, document:

- Identified probabilistic components  
- Explicit boundary declaration  
- Invocation contracts  
- Validation harness strategy  
- Containment logic  
- Fallback mechanisms  
- Observability strategy  
- Reproducibility posture  

Probabilistic subsystems SHALL NOT be embedded implicitly.

Silent modality blending is prohibited.

---

# 5. Architectural Style and Structural Model

Describe:

- Selected architectural pattern  
- Structural layering or modularization  
- Dependency direction rules  
- Cross-cutting concern handling  

Include:

- Rationale  
- Trade-offs  
- Alternatives considered  
- Rejection rationale  

Framework-first design is prohibited.

Structure SHALL derive from Requirements and NFRs.

---

# 6. Major Components

For each component, define:

- Component ID  
- Responsibility  
- Related Requirement ID(s)  
- Key interfaces  
- Dependency constraints  
- Data ownership  

Each component MUST map to at least one Requirement ID.

Orphan components are prohibited.

---

# 7. Data Architecture

Document:

- Persistence model  
- Data ownership boundaries  
- Data lifecycle  
- Consistency guarantees  
- Cross-boundary data flows  
- Validation checkpoints  

Data movement SHALL be explicit.

Hidden data flows are prohibited.

---

# 8. Non-Functional Architecture

Explicitly demonstrate structural enforcement of:

- Performance constraints  
- Scalability posture  
- Reliability mechanisms  
- Fault tolerance strategy  
- Security controls  
- Audit logging  
- Observability mechanisms  
- Configuration management  
- Reproducibility controls  

NFRs SHALL be visible in structural decisions.

Assumed NFR compliance is prohibited.

---

# 9. Failure Posture

Document:

- Deterministic failure modes  
- Probabilistic failure modes (if applicable)  
- Integration failure scenarios  
- Degradation strategy  
- Recovery strategy  
- Rollback posture  
- Risk concentration points  

Failure modeling at architecture level is mandatory.

---

# 10. Deployment and Packaging Alignment

Define:

- Target runtime environments  
- Infrastructure assumptions  
- Containerization (if applicable)  
- Networking model  
- Artifact boundaries  
- Configuration boundaries  

Architecture SHALL support reproducible packaging.

---

# 11. Orchestration Alignment

Document:

- Build entry point  
- Dependency management strategy  
- Clean build expectations  
- CI/CD alignment  
- Validation harness execution within pipeline  

Architecture SHALL not conflict with orchestration requirements.

---

# 12. Risk Assessment

Identify:

- Structural risks  
- Scalability risks  
- Integration risks  
- Regulatory risks  
- Probabilistic containment risks  
- Operational risks  

Classify each risk:

- Low  
- Moderate  
- High  

High-risk items SHALL require mitigation strategy before Detailed Design.

---

# 13. Traceability Summary

Provide reference to RTM.

Confirm:

- All Architectural Components map to Requirement IDs  
- No orphan requirements exist  
- No untraceable structural elements exist  

Traceability gaps SHALL block advancement.

---

# 14. Open Questions

List:

- Architectural uncertainties  
- Pending decisions  
- Validation dependencies  
- External verification needs  

Unresolved high-impact questions SHALL block progression.

---

# 15. Phase Gate Declaration

Confirm readiness to proceed to Detailed Design:

- All mandatory sections completed? (Yes / No)  
- Deterministic–probabilistic boundaries defined? (Yes / No)  
- NFR-driven structure demonstrated? (Yes / No)  
- RTM updated? (Yes / No)  
- Human approval granted? (Yes / No)  

If any answer is “No,” remain in Architecture phase.

---

# Approval

Approved By:  
Role:  
Date:  
Version Incremented: Yes / No  

---

End of High-Level Architecture Template
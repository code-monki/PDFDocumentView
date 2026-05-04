<!--
File: 04-templates/system/traceability-matrix-template.md

Purpose:
  Provide enforceable bidirectional lifecycle traceability
  across hybrid deterministic–probabilistic systems.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Traceability is a structural control mechanism.
Release without validated traceability is prohibited.
-->

# Requirements Traceability Matrix (RTM)

Project Name:  
Version:  
Date (YYYY-MM-DD):  
Maintained By:  
Status: Draft / Approved  
Requirement Version Reference:  
Architecture Version Reference:  
Design Version Reference:  
Test Plan Version Reference:  

---

# 1. RTM Authority Declaration

Confirm:

- Requirements approved? (Yes / No)  
- Architecture approved? (Yes / No)  
- Detailed Design approved? (Yes / No)  
- Test Plan aligned? (Yes / No)  
- Advancement to next phase authorized? (Yes / No)  

If any answer is “No,” traceability validation is incomplete.

---

# 2. Purpose

The RTM SHALL ensure:

- Every requirement maps to architecture  
- Every architectural element maps to requirements  
- Every design artifact maps to architecture  
- Every implementation artifact maps to design  
- Every test case maps to requirements  
- Packaging and orchestration artifacts map to release state  
- Deterministic–probabilistic boundaries are traceable  

Traceability SHALL be bidirectional and complete.

---

# 3. Core Traceability Matrix

| Req ID | Req Type | HLA Component ID | DD Artifact | Implementation Unit | Test Case ID | Packaging Ref | Orchestration Ref | Validation Status | Evidence Ref |
|--------|----------|------------------|------------|---------------------|--------------|---------------|-------------------|-------------------|--------------|

Example:

| FR-001 | FR | HLA-Auth | DD-Login | auth/login.py | TC-001 | PKG-v1.0 | ORCH-Build-01 | Verified | TEST-REPORT-001 |

---

## Field Definitions

**Req ID**  
FR-XXX or NFR-XXX identifier from SRS.

**Req Type**  
FR / NFR.

**HLA Component ID**  
Approved architectural component.

**DD Artifact**  
Design-level module, interface, or artifact identifier.

**Implementation Unit**  
Code module, package, service, or deployment unit.

**Test Case ID**  
Validation case identifier.

**Packaging Ref**  
Reference to packaging plan artifact or release identifier.

**Orchestration Ref**  
Reference to build or pipeline identifier.

**Validation Status**  
Planned / Implemented / Tested / Verified / Blocked.

**Evidence Ref**  
Reference to test report, validation artifact, or audit evidence.

---

# 4. Deterministic–Probabilistic Boundary Traceability (If Applicable)

For probabilistic subsystems, include:

| Req ID | Boundary ID | Validation Harness | Containment Logic | Fallback Ref | Observability Ref | Drift Validation | Status |

Probabilistic behavior SHALL NOT exist outside RTM coverage.

Untracked boundaries are governance violations.

---

# 5. Non-Functional Traceability

Each NFR SHALL explicitly map to:

- Architectural mechanism  
- Design enforcement  
- Test validation  
- Packaging consideration  
- Orchestration consideration  

Example:

| NFR ID | Architectural Mechanism | Design Artifact | Test Case | Packaging Impact | Status |

Assumed NFR compliance is prohibited.

---

# 6. Bidirectional Verification Rules

Traceability MUST support:

Forward tracing:
Requirement → Architecture → Design → Implementation → Test → Packaging

Backward tracing:
Test → Implementation → Design → Architecture → Requirement

If any chain breaks, advancement is prohibited.

---

# 7. Orphan Detection

The following SHALL block progression:

- Requirement without architectural mapping  
- Architectural component without requirement  
- Design artifact without architecture parent  
- Implementation unit without design reference  
- Test case without requirement reference  
- Packaging artifact without RTM linkage  
- Orchestration artifact without RTM linkage  

No orphan artifacts permitted.

---

# 8. Change Control and Lineage

When any of the following change:

- Requirement  
- Architecture  
- Design  
- Implementation  
- Test case  
- Packaging configuration  
- Orchestration pipeline  

The RTM SHALL be updated immediately.

Each update SHALL record:

- Date  
- Change summary  
- Impacted IDs  
- Phase rollback requirement (if any)  

Untracked change invalidates lifecycle integrity.

---

# 9. Coverage Validation Checklist

Before phase advancement, confirm:

- 100% FR coverage to design  
- 100% NFR coverage to architecture  
- 100% implementation traceability  
- 100% requirement-to-test coverage  
- Deterministic–probabilistic boundaries mapped (if applicable)  
- Packaging traceability complete  
- Orchestration traceability complete  
- No orphan artifacts  

Failure blocks advancement.

---

# 10. Release-State Snapshot

Before release authorization:

- RTM Version incremented  
- Validation status updated to release state  
- Evidence references finalized  
- Packaging reference aligned to artifact  
- Orchestration reference aligned to build  
- Documentation version aligned  

Release without finalized RTM snapshot is prohibited.

---

# 11. Approval

Approved By:  
Role:  
Date:  
Version Incremented: Yes / No  

RTM validation required before:

- Implementation completion  
- Test phase closure  
- Packaging approval  
- Release authorization  

---

End of Requirements Traceability Matrix Template
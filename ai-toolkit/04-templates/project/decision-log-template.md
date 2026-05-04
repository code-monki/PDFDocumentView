<!--
File: 04-templates/project/decision-log-template.md

Purpose:
  Provide enforceable recording of significant architectural,
  structural, and risk-bearing decisions.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Decision logging is a structural governance control.
Unlogged structural decisions are prohibited.
-->

# Decision Log

Project Name:  
Version / Branch:  
Maintainer:  
RTM Version Reference:  

---

# Entry ID: DL-XXX  
Date (YYYY-MM-DD):  
Lifecycle Phase:  
Author:  

Status:
- Proposed
- Approved
- Implemented
- Deprecated
- Superseded

If superseded, reference replacement Entry ID.

---

## 1. Decision Summary

Provide a concise, structural description of the decision.

Examples:

- Selected layered architecture over distributed microservices.
- Introduced probabilistic subsystem with defined containment boundary.
- Changed persistence model to enforce auditability.
- Modified packaging strategy to ensure reproducibility.

Decision summary must describe structural impact.

---

## 2. Context

Document:

- Requirement ID(s) involved
- Architectural Component ID(s) involved
- Design artifact references
- Non-functional drivers
- Constraints influencing the decision
- Risk factors identified
- Phase-gate context

Context must explain why the decision was necessary at this lifecycle stage.

---

## 3. Deterministic–Probabilistic Boundary Impact (If Applicable)

If decision affects probabilistic components, document:

- Boundary changes
- Validation harness implications
- Containment logic modifications
- Fallback behavior impact
- Observability changes
- Drift detection implications
- Reproducibility impact

Undocumented boundary impact is prohibited.

---

## 4. Alternatives Considered

Document:

- Options evaluated
- Trade-offs identified
- Deterministic impact comparison
- Risk comparison
- Long-term maintenance implications
- Reasons for rejection

Rejected alternatives must be documented.

Silent narrowing is prohibited.

---

## 5. Decision Rationale

Explain:

- Why the chosen option was selected
- Alignment with Requirement IDs
- Alignment with NFRs
- Architectural consistency
- Risk mitigation posture
- Reproducibility impact
- Long-term maintainability considerations

Rationale must be defensible under audit.

---

## 6. Impact Analysis

Identify impacts across lifecycle artifacts:

- Requirements (IDs)
- Architecture sections
- Detailed Design artifacts
- Implementation units
- Test Case IDs
- Packaging configuration
- Orchestration pipeline
- System documentation

Indicate whether RTM update is required.

If impact spans phases, identify required gate re-evaluation.

---

## 7. Traceability References

Mandatory references:

- Requirement ID(s)
- Architectural Component ID(s)
- Detailed Design reference(s)
- Test Case ID(s) (if applicable)
- Packaging reference (if applicable)
- Orchestration reference (if applicable)

Decision without traceability mapping is invalid.

---

## 8. Risk Assessment

Document:

- Residual risks
- Assumptions
- Monitoring requirements
- Revisit triggers
- Compliance implications
- Operational exposure

If risk level is Moderate or High, indicate mitigation plan.

---

## 9. Versioning & Lineage

Record:

- RTM version at time of decision
- System documentation version
- Artifact version (if applicable)
- Related prior Decision Log entries

Decision lineage must be reconstructable.

---

## 10. Phase Impact Declaration

Indicate:

- Does this decision require rollback to prior phase? (Yes / No)
- Does this decision require revalidation of tests? (Yes / No)
- Does this decision require packaging revalidation? (Yes / No)
- Does this decision alter NFR posture? (Yes / No)

Structural decisions may invalidate downstream artifacts.

Failure to declare impact is a governance violation.

---

# Governance Notes

Decision log entries SHALL be created when decisions:

- Affect architecture
- Modify deterministic–probabilistic boundaries
- Introduce or remove technologies
- Change non-functional posture
- Modify system boundaries
- Alter packaging or orchestration
- Resolve high-risk defects
- Modify compliance posture
- Introduce risk trade-offs

Routine implementation details SHALL NOT require entries.

Decision logs preserve:

- Architectural integrity
- Change impact clarity
- Audit readiness
- Institutional memory
- Structural governance continuity

Unlogged structural decisions are prohibited.

---

End of Decision Log Template
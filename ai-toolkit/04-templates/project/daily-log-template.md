<!--
File: 04-templates/project/daily-log-template.md

Purpose:
  Provide enforceable daily logging structure to preserve lifecycle
  discipline, traceability continuity, boundary transparency,
  and audit readiness.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md

Daily logging is a structural governance control.
-->

# Daily Log

Project Name:  
Version / Branch:  
Date (YYYY-MM-DD):  
Lifecycle Phase:  
Author:  
RTM Version Reference:  

---

# 1. Phase Confirmation

Current lifecycle phase (select one):

- Ideation  
- Requirements  
- High-Level Architecture  
- Detailed Design  
- Traceability Consolidation  
- Test Planning  
- Implementation  
- Packaging & Orchestration  
- Documentation Closure  

Confirm:

- Phase advancement authorized? (Yes / No)  
- Gate criteria previously satisfied? (Yes / No)

If gate criteria are unmet, advancement is prohibited.

---

# 2. Objectives for the Day

Explicitly state:

- Target artifacts to advance
- Requirement IDs impacted
- Architectural Component IDs impacted
- Detailed Design references impacted
- Test Case IDs impacted
- RTM sections impacted

Objectives must align with current lifecycle phase.

Scope expansion beyond approved artifacts is prohibited.

---

# 3. Work Performed

Document with specificity:

- Artifacts created or modified
- Requirement IDs addressed
- Architecture updates
- Design refinements
- Implementation units modified
- Tests created or updated
- Documentation updates
- Orchestration or packaging updates

Avoid summary language.

Record structural changes only.

---

# 4. Decisions Made

Record all non-trivial decisions.

For each decision include:

- Decision description
- Related Requirement ID(s)
- Related Architectural Component ID(s)
- Related Design reference(s)
- Risk assessment
- Impacted RTM entries

If decision alters scope or structure, identify required phase rollback.

Unrecorded decisions are governance violations.

---

# 5. Deterministic–Probabilistic Boundary Status (If Applicable)

If probabilistic components exist, document:

- Boundary changes
- Validation harness modifications
- Containment logic changes
- Fallback adjustments
- Observability changes
- Configuration adjustments

Boundary drift must be explicitly recorded.

---

# 6. Issues Identified

Document:

- Requirement ambiguities
- Architectural risks
- Design inconsistencies
- Traceability gaps
- Orphaned artifacts
- Packaging failures
- Orchestration failures
- Test failures

For each issue, indicate:

- Phase escalation required? (Yes / No)
- Risk classification (Low / Moderate / High)

Unresolved high-risk items block advancement.

---

# 7. Tests and Validation

Record:

- Test Case IDs created or modified
- Test coverage impact
- Functional validation status
- NFR validation status
- Probabilistic containment validation status (if applicable)
- Clean build result
- CI/CD result (if applicable)

Validation gaps must be visible daily.

---

# 8. Traceability Updates

Indicate explicitly:

- RTM updated? (Yes / No)
- New mappings added?
- Orphaned artifacts detected?
- Version identifier incremented?

If RTM is not updated during structural change, advancement is prohibited.

---

# 9. Packaging & Orchestration Status (If Applicable)

Record:

- Clean build result
- Packaging reproducibility status
- Artifact version identifier
- Environment parity validation
- Deterministic–probabilistic containment integrity post-build

Reproducibility failures must be escalated immediately.

---

# 10. Audit & Reproducibility Check

Confirm:

- All structural changes traceable
- Version identifiers aligned
- Documentation aligned with changes
- No undocumented scope introduced

If any answer is negative, remain in current phase.

---

# 11. Open Questions

List:

- Pending clarifications
- Design uncertainties
- External dependencies
- Compliance considerations
- Unresolved risks

Unresolved items must be visible.

---

# 12. Next Steps

Define:

- Immediate next structural actions
- Phase progression intent (if applicable)
- Required approvals
- Required RTM updates

Advancement requires explicit human authorization per lifecycle governance.

---

End of Daily Log Template
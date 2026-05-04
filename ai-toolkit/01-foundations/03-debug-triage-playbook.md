<!--
File: 01-foundations/03-debug-triage-playbook.md

Purpose:
  Provide a structured debugging protocol that preserves
  architectural integrity, traceability, and lifecycle discipline.

Lifecycle authority resides in:
  02-governance/00-lifecycle-bootstrap.md
-->

# Debug & Triage Playbook

Debugging must be structured, disciplined, and traceable.

Reactive fixes without structured analysis introduce architectural drift,
traceability gaps, and long-term systemic instability.

This playbook governs how issues are analyzed and resolved.

---

# 1. Confirm Lifecycle Context

Before debugging begins:

- Confirm the current lifecycle phase.
- Determine whether the issue represents:
  - Implementation defect
  - Design flaw
  - Architectural inconsistency
  - Requirement gap
  - Configuration error
  - Packaging or environment issue

Not all defects originate in code.

If root cause lies in Requirements, Architecture, or Detailed Design,
return to the appropriate phase before implementing changes.

No lifecycle shortcuts are permitted.

---

# 2. Reproduce the Issue

The issue must be reproducible before corrective action.

Define explicitly:

- Expected behavior
- Observed behavior
- Environment conditions
- Inputs
- Relevant logs or outputs
- Requirement ID(s) implicated

If the issue cannot be reproduced, do not implement speculative fixes.

Speculation is not engineering.

---

# 3. Apply Deterministic Debug Hierarchy

Investigate in the following order:

1. Configuration files
2. Build/orchestration scripts
3. Source implementation
4. Generated artifacts
5. Runtime environment assumptions

Do not begin by modifying source code unless configuration and orchestration have been validated.

This ordering reduces unnecessary code churn.

---

# 4. Isolate the Scope

Determine:

- Affected component(s)
- Related requirement ID(s)
- Related test case(s)
- Architectural boundaries involved

Avoid broad or unfocused changes.

Changes must be minimal, targeted, and traceable.

---

# 5. Perform Root Cause Analysis

Root cause analysis must:

- Identify the underlying failure mechanism.
- Distinguish symptom from cause.
- Confirm whether failure arises from:
  - Logic defect
  - Integration mismatch
  - Configuration issue
  - Data inconsistency
  - Concurrency or timing issue
  - Resource constraint
  - Environmental drift

Avoid patching symptoms.

Corrective action must address root cause.

---

# 6. Assess Architectural Impact

Before implementing a fix:

- Confirm alignment with High-Level Architecture.
- Confirm alignment with Detailed Design.
- Evaluate cross-cutting implications.
- Evaluate performance, security, and maintainability impact.

If architectural changes are required, return to Architecture or Detailed Design phase.

Debugging must not silently introduce architectural modification.

---

# 7. Define Fix Strategy

The fix must:

- Align with requirement intent.
- Preserve modular boundaries.
- Avoid introducing new scope.
- Include updated test coverage.
- Maintain traceability mappings.

Temporary or compensating fixes must be explicitly documented and time-bounded.

---

# 8. Update Tests

For every defect fix:

- Add or update test cases.
- Map tests to requirement IDs.
- Validate regression coverage.
- Confirm tests pass in a clean build environment.

A fix without a test is incomplete.

---

# 9. Update Documentation

If the fix affects:

- System behavior
- Configuration
- Deployment assumptions
- Architecture documentation
- Operational constraints

Documentation must be updated before phase closure.

Documentation drift is systemic risk.

---

# 10. Validate Clean Build

After implementing a fix:

- Perform a clean build.
- Execute full test suite.
- Validate packaging artifacts (if applicable).
- Confirm reproducibility.

Debugging must not degrade build integrity.

---

# 11. Record Decision

All non-trivial fixes must:

- Be logged in the Decision Log.
- Reference requirement ID(s).
- Reference affected components.
- Document rationale.
- Document risk assessment.

Institutional memory is mandatory.

---

# Governance Principle

Debugging is a structured engineering activity.

It must:

- Preserve architectural integrity.
- Maintain lifecycle discipline.
- Protect traceability.
- Reduce recurrence risk.
- Improve systemic stability over time.

Reactive patching without structured triage increases systemic risk.

No corrective action is complete until traceability, tests, documentation,
and reproducibility are preserved.
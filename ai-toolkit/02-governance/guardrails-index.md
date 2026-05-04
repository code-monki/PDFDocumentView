# Guardrails Index

Purpose:
  Provide structured reference to all lifecycle enforcement guardrails
  governing disciplined hybrid deterministic–probabilistic system development.

Lifecycle sequencing and authority are defined in:

- [00-lifecycle-bootstrap.md](./00-lifecycle-bootstrap.md)

Guardrails enforce discipline within phases.  
They must not redefine lifecycle order.

If conflict appears between guardrails, lifecycle authority resides in
[00-lifecycle-bootstrap.md](./00-lifecycle-bootstrap.md).

---

# Lifecycle Guardrails (Ordered by Phase)

## 01. Ideation

- [01-ideation-guardrail.md](./01-ideation-guardrail.md)

Enforces disciplined problem exploration prior to formal specification.

---

## 02. Requirements (SRS)

- [02-requirements-guardrail.md](./02-requirements-guardrail.md)

Enforces:

- Functional completeness  
- Non-functional requirements  
- Constraints and assumptions  
- Waiting room discipline  
- Requirement identification and traceability foundations  
- Testability as a design constraint  

---

## 03. High-Level Architecture (HLA)

- [03-architecture-guardrail.md](./03-architecture-guardrail.md)

Enforces:

- System boundaries  
- Component definitions  
- Data flows  
- Dependency modeling  
- Trade-off analysis  
- Failure mode consideration  

---

## 04. Detailed Design (DD)

- [04-detailed-design-guardrail.md](./04-detailed-design-guardrail.md)

Enforces:

- Interface specifications  
- Data models  
- Interaction flows  
- State modeling  
- Error strategy  
- Performance and security considerations  
- Traceability mapping  

---

## 05. Implementation

- [05-implementation-guardrail.md](./05-implementation-guardrail.md)

Enforces:

- Design conformance  
- Prevention of architectural drift  
- Prevention of requirement drift  
- Traceability preservation  
- Controlled refactoring  
- Scope discipline  

---

## 06. Code Documentation

- [06-code-documentation-guardrail.md](./06-code-documentation-guardrail.md)

Enforces:

- File-level headers  
- Interface documentation  
- Invariant documentation  
- Error-handling rationale  
- Alignment between code and documentation  

---

## 07. System Documentation

- [07-system-documentation-guardrail.md](./07-system-documentation-guardrail.md)

Enforces:

- Continuous documentation updates  
- Architecture alignment  
- Version alignment  
- Operational clarity  

---

## 08. Test Planning and Validation

- [08-test-guardrail.md](./08-test-guardrail.md)

Enforces:

- Requirement-to-test mapping  
- Functional coverage  
- Non-functional validation  
- Automation discipline  
- Regression protection  

---

## 09. Traceability

- [09-traceability-guardrail.md](./09-traceability-guardrail.md)

Enforces:

- End-to-end requirement mapping  
- Change impact analysis  
- Prevention of orphaned artifacts  
- Requirement → Architecture → Design → Implementation → Test integrity  

---

## 10. Packaging

- [10-packaging-guardrail.md](./10-packaging-guardrail.md)

Enforces:

- Version discipline  
- Automated artifact creation  
- Clean environment validation  
- Dependency transparency  
- Cross-platform awareness (if applicable)  

---

## 11. Orchestration

- [11-orchestration-guardrail.md](./11-orchestration-guardrail.md)

Enforces:

- Automated builds  
- Reproducibility  
- Environment isolation awareness  
- CI compatibility  
- Deployment readiness  

---

## 12. Phase Gates

- [12-phase-gate-checklist.md](./12-phase-gate-checklist.md)

Enforces:

- Human approval at phase transitions  
- Explicit readiness confirmation  
- Prevention of lifecycle skipping  

---

# Enforcement Principles

- Guardrails enforce discipline within phases.
- Lifecycle order is defined in [00-lifecycle-bootstrap.md](./00-lifecycle-bootstrap.md).
- No phase may be skipped.
- Human approval is required for phase transitions.
- Traceability must remain intact throughout the lifecycle.
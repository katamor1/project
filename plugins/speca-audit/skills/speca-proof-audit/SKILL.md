---
name: speca-proof-audit
description: Use when auditing implementation code against SPECA properties with map-prove-stress-test proof attempts and evidence-grounded vulnerability claims.
---

# SPECA Property-Grounded Audit

Phase `03` checks whether code satisfies each property through proof-attempt reasoning.

## Core rule

Do not start from “find bugs.” Start from “try to prove this property holds.” Report a finding only when the proof attempt exposes a concrete gap with code evidence and provenance.

## Procedure

1. Map property assumptions and code locations.
2. Prove the property using source evidence.
3. Stress-test the proof against adversarial inputs, missing validation, bad state transitions, race boundaries, and trust-boundary violations.
4. Emit concise findings only when the proof fails.
5. Include code location, property ID, spec provenance, exploit preconditions, and severity rationale.

## Command

```bash
uv run python3 scripts/run_phase.py --phase 03 --workers 4 --max-concurrent 16
```

## References

- Worker prompt: `references/prompts/03_auditmap_worker_inline.md`.
- Schemas: `schemas/AuditMapItem.schema.json`, `schemas/ReviewedItem.schema.json`.

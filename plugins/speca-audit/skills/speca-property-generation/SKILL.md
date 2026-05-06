---
name: speca-property-generation
description: Use when deriving typed security properties, trust assumptions, STRIDE/CWE checks, invariants, preconditions, or postconditions from SPECA subgraphs.
---

# SPECA Property Generation

Phase `01e` turns subgraphs and scope data into security properties.

## Required input

`outputs/BUG_BOUNTY_SCOPE.json` must exist. Do not fabricate scope. Ask the user to provide it or create a clearly marked draft for their review.

## Procedure

1. Load Phase 01b partials and scope.
2. Identify trust boundaries, actors, assets, assumptions, and attacker-controlled inputs.
3. Use domain-agnostic STRIDE and CWE Top 25 thinking.
4. Generate typed properties: Invariant, Precondition, Postcondition, Assumption.
5. Preserve provenance from property back to spec/subgraph IDs.
6. Keep `covers` slim: primary element ID as a string.

## Command

```bash
uv run python3 scripts/run_phase.py --phase 01e --workers 4 --max-concurrent 16
```

## References

- Worker prompt: `references/prompts/01e_prop_worker.md`.
- Schemas: `schemas/Property.schema.json`, `schemas/TrustModel.schema.json`, `schemas/PropertyReachability.schema.json`.

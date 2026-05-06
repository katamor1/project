---
name: speca-subgraph-extraction
description: Use when converting a technical specification section into program graphs, Mermaid state diagrams, invariants, or subgraph JSON artifacts.
---

# SPECA Subgraph Extraction

Phase `01b` decomposes specification text into formal program graphs and Mermaid `.mmd` diagrams.

## Output expectations

For each functional unit, preserve:

- `Q`: nodes / program points.
- `q_init` and final states.
- `Act`: assignments, guards, calls, protocol actions.
- `E`: transitions.
- Invariants as `INV-*` notes in Mermaid.

## Mermaid rules

- Use `=` instead of `:=`.
- Keep node identifiers alphanumeric plus `_`.
- Avoid raw `<`, `>`, `{`, `}` in labels where possible.
- Emit `.mmd` files and JSON references to them.

## Command

```bash
uv run python3 scripts/run_phase.py --phase 01b --workers 4 --max-concurrent 16
```

## References

- Main worker prompt: `references/prompts/01b_extract_worker.md`.
- Original Claude Code skill: `references/original-claude/skills/subgraph-extractor/SKILL.md`.
- Output schemas: `schemas/SpecSubGraphs.schema.json`, `schemas/SubGraph.schema.json`.

---
name: speca-code-preresolution
description: Use when mapping SPECA properties to source-code locations, symbols, call sites, entry points, or target repository files before audit.
---

# SPECA Code Pre-resolution

Phase `02c` maps generated properties to code locations before proof-attempt auditing.

## Required input

`outputs/TARGET_INFO.json` must exist and identify the target repository, commit, and local checkout information. Keep Phase 02c and Phase 03 on the same target commit.

## Procedure

1. Load generated properties and target info.
2. Prefer symbol- and tree-sitter-based search when MCP is available.
3. Use grep/glob/read fallbacks when MCP is unavailable.
4. Resolve likely files, symbols, entry points, and attacker-controlled paths.
5. Drop informational properties if the configured severity gate requires it.

## Command

```bash
uv run python3 scripts/run_phase.py --phase 02c --workers 4 --max-concurrent 16
```

## References

- Worker prompt: `references/prompts/02c_codelocation_worker.md`.
- Schemas: `schemas/CodeLocation.schema.json`, `schemas/CodeScope.schema.json`, `schemas/PropertyWithCode.schema.json`.

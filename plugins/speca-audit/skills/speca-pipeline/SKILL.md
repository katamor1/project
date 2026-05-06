---
name: speca-pipeline
description: Use when planning, running, resuming, or troubleshooting the SPECA specification-anchored security audit pipeline in Codex.
---

# SPECA Pipeline

Use this skill to operate the full SPECA phase chain from specification discovery through reviewed findings.

## First checks

1. Confirm the user has authorization to audit the target repository or protocol.
2. Inspect `outputs/` for existing `*_PARTIAL_*.json` files and resume rather than deleting work.
3. Check required inputs before each phase:
   - `SPEC_URLS`, `KEYWORDS` for `01a`.
   - `outputs/BUG_BOUNTY_SCOPE.json` before `01e`.
   - `outputs/TARGET_INFO.json` before `02c` and `03`.

## Codex-native runner

Prefer the OpenAI-compatible runner when using Codex:

```bash
export ORCHESTRATOR_RUNNER=api
export API_RUNNER_BASE_URL="${API_RUNNER_BASE_URL:-https://api.openai.com/v1}"
export API_RUNNER_API_KEY="${API_RUNNER_API_KEY:-$OPENAI_API_KEY}"
# Set API_RUNNER_MODEL to the approved model for the environment.
uv run python3 scripts/run_phase.py --target 04 --workers 4 --max-concurrent 16
```

Use the original Claude runner only when the environment intentionally has `claude` configured.

## Common commands

```bash
uv sync
uv run python3 scripts/run_phase.py --phase 01a --workers 1
uv run python3 scripts/run_phase.py --phase 01b --workers 4 --max-concurrent 16
uv run python3 scripts/run_phase.py --phase 01e --workers 4 --max-concurrent 16
uv run python3 scripts/run_phase.py --phase 02c --workers 4 --max-concurrent 16
uv run python3 scripts/run_phase.py --phase 03 --workers 4 --max-concurrent 16
uv run python3 scripts/run_phase.py --phase 04 --workers 2 --max-concurrent 8
uv run python3 scripts/run_phase.py --phase 03 --cleanup-dry-run
```

## References

- Pipeline details: `README.md`, `references/docs/V3_NOTES.md`.
- Worker prompts: `references/prompts/`.
- Schemas: `schemas/` and `references/prompts/*`.

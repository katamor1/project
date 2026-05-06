# SPECA Audit Codex Plugin

This is a Codex plugin conversion of the uploaded SPECA agent folder.

It packages the SPECA specification-anchored security audit workflow as Codex Agent Skills, while preserving the original Python orchestrator, schemas, prompts, and Claude Code references.

## Contents

```text
.codex-plugin/plugin.json   # Codex plugin manifest
skills/                     # Codex skill entry points
.mcp.json                   # Optional MCP server configuration
AGENTS.md                   # Repo-level Codex instructions
references/prompts/         # Original worker prompts
references/docs/            # Project design notes
references/original-claude/ # Original Claude Code config and skills
scripts/                    # SPECA orchestrator/runtime scripts
schemas/                    # JSON schemas for phase artifacts
csv/                        # Audit knowledge CSV resources
```

## Quick local use

```bash
uv sync
export ORCHESTRATOR_RUNNER=api
export API_RUNNER_API_KEY="$OPENAI_API_KEY"
export API_RUNNER_MODEL="<approved-model-for-your-environment>"
uv run python3 scripts/run_phase.py --phase 01a --workers 1
```

The original Claude runner is still present for backwards compatibility, but Codex users should prefer `ORCHESTRATOR_RUNNER=api` unless they intentionally have Claude Code installed.

## Required audit inputs

- `SPEC_URLS` / `KEYWORDS`: Phase 01a seed URLs and discovery terms.
- `outputs/BUG_BOUNTY_SCOPE.json`: required before Phase 01e.
- `outputs/TARGET_INFO.json`: required before Phase 02c and Phase 03.

## MCP setup

The plugin includes `.mcp.json` for `fetch`, `filesystem`, `tree_sitter`, `semgrep`, and `github`. Install `uv`, `uvx`, Node.js, and `npx` before using those MCP servers. GitHub access may require `GITHUB_PERSONAL_ACCESS_TOKEN`, `GH_TOKEN`, or `GITHUB_TOKEN`.

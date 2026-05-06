# SPECA Audit Codex Plugin Instructions

This repository is packaged as a Codex plugin. Use the bundled skills when the user asks for specification-anchored security auditing, SPECA pipeline execution, or conversion/maintenance of this package.

## Core operating rules

- Treat this as an authorized security-audit workflow. Do not help exploit, deploy, or weaponize findings outside the user's owned or authorized scope.
- Start from specifications, not only source-code patterns. Preserve provenance chains: `spec section -> property/invariant -> code location -> proof attempt -> finding/review verdict`.
- Prefer structured JSON artifacts that validate against `schemas/*.schema.json`; do not silently invent missing required inputs.
- Keep all generated audit artifacts under `outputs/` unless the user requests another path.
- For Codex-native runs, prefer the OpenAI-compatible API runner by setting `ORCHESTRATOR_RUNNER=api`. The original Claude runner is preserved for backwards compatibility only.
- Before claiming a phase is complete, verify that the expected `outputs/*PARTIAL*.json` or report files exist and are parseable JSON.

## Phase chain

`01a` specification discovery -> `01b` subgraph extraction -> `01e` property generation -> `02c` code pre-resolution -> `03` property-grounded audit -> `04` severity-preserving review. Manual report phases are `05`, `06`, and `06b`.

## Important inputs

- `SPEC_URLS` and `KEYWORDS` drive Phase 01a.
- `outputs/BUG_BOUNTY_SCOPE.json` is required for Phase 01e.
- `outputs/TARGET_INFO.json` is required for Phase 02c and Phase 03.
- `GITHUB_PERSONAL_ACCESS_TOKEN`, `GH_TOKEN`, or `GITHUB_TOKEN` may be needed for GitHub-backed repository access.

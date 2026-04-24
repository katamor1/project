<!-- ibm-bob/mode-pack/samples/diff-dry-run.md -->
<!-- Shows a Bob dry run for legacy diff work through the entry and runtime modes. -->
<!-- This exists so the team can validate the change, carry-over, and unknown path without entering SDLC authoring. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing-map.md, .bob/custom_modes.yaml, docs/external-runtime/legacy-kb/query-diff-flow.md -->
# Diff Dry Run

## User Intent

「通知設定の新要求が、現行仕様から何を変えるか整理したい」

## Suggested Mode Path

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> k5-retrieval-planner -> k6-legacy-diff-checker -> k8-evidence-reviewer -> c4-entry-response-shaper`

## What To Check

- `k6` が `change`, `carry_over`, `unknown` を分けること
- `k8` が conflict や low confidence を止めること
- `c4` が `abstain + next action` に落とせること

## Expected Bob Reply Shape

1. 変わる点
2. そのまま踏襲できる点
3. 根拠不足で保留の点

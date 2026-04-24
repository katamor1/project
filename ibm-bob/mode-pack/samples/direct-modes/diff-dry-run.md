<!-- ibm-bob/mode-pack/samples/direct-modes/diff-dry-run.md -->
<!-- Shows a dry run for new direct-mode diff work through entry and runtime. -->
<!-- This exists so the team can validate the direct diff route without entering the old fixed-stage family. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/pilot-runbook.md, docs/external-runtime/legacy-kb/query-diff-flow.md -->
# Diff Dry Run

## User Intent

「通知設定の新要求が、現行仕様から何を変えるか整理したい」

## Suggested Mode Path

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-legacy-diff-checker -> ibmbob-evidence-reviewer -> ibmbob-entry-response-shaper`

## What To Check

- `change`, `carry_over`, `unknown` の 3 区分が崩れないこと
- conflict や low confidence が direct authoring に流れないこと
- 最終応答が `abstain + next action` に落とせること


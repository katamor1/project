<!-- ibm-bob/mode-pack/samples/direct-modes/review-dry-run.md -->
<!-- Shows a dry run for the new Bob direct review-record path. -->
<!-- This exists so the team can validate review-only work without entering the old copied-workspace run. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/sdlc-guide.md, ibm-bob/mode-pack/pilot-runbook.md, .copilot/prompts/sdlc/ibmbob-sdlc-review-record.md -->
# Review Dry Run

## User Intent

「この基本設計レビューの記録だけをまとめたい」

## Suggested Mode Path

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-retrieval-planner -> ibmbob-artifact-context-packer -> ibmbob-evidence-reviewer -> ibmbob-sdlc-review-record`

## What To Check

- review record が unresolved risk を残せること
- source-backed review でも handoff が欠けないこと
- 余計な authoring lane に入らないこと



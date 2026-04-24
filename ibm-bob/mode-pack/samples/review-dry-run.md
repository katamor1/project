<!-- ibm-bob/mode-pack/samples/review-dry-run.md -->
<!-- Shows a Bob dry run for review recording after a source-backed or non-source-backed artifact review. -->
<!-- This exists so the team can validate the review path without running the full implementation lanes. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/sdlc-guide.md, .bob/custom_modes.yaml, .copilot/prompts/P8_review_record.md -->
# Review Dry Run

## User Intent

「この基本設計レビューの記録だけをまとめたい」

## Suggested Mode Path

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> k5-retrieval-planner -> k7-artifact-context-packer -> k8-evidence-reviewer -> p8-review-record -> c4-entry-response-shaper`

## What To Check

- `p8` が review history と unresolved risk をまとめること
- `c4` が user-facing に短く返すこと
- `p8` へ直接行く時も handoff が欠けないこと

<!-- ibm-bob/mode-pack/samples/direct-modes/generation-dry-run.md -->
<!-- Shows a dry run for new direct-mode source-backed design generation. -->
<!-- This exists so the team can validate the new Bob direct family without touching the old copied-workspace family. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/pilot-runbook.md, docs/sdlc/README.md -->
# Generation Dry Run

## User Intent

「通知設定変更の基本設計を、旧仕様の根拠付きで作りたい」

## Suggested Mode Path

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-retrieval-planner -> ibmbob-artifact-context-packer -> ibmbob-evidence-reviewer -> ibmbob-sdlc-basic-design-author`

## What To Check

- `artifact_context_packet` が authoring 前にそろうこと
- `ibmbob-evidence-reviewer` が weak evidence を止めること
- `ibmbob-sdlc-basic-design-author` が source-backed 下書きを始められること


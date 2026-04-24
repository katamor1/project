<!-- ibm-bob/mode-pack/samples/direct-modes/eval-dry-run.md -->
<!-- Shows a dry run for the new Bob direct eval-only path. -->
<!-- This exists so the team can validate evaluation work without mixing it with the old fixed-stage family. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/sdlc-guide.md, ibm-bob/mode-pack/pilot-runbook.md, .copilot/prompts/sdlc/ibmbob-sdlc-eval-monitor.md -->
# Eval Dry Run

## User Intent

「今ある review record を見て、次に足りない評価観点を整理したい」

## Suggested Mode Path

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-retrieval-planner -> ibmbob-artifact-context-packer -> ibmbob-evidence-reviewer -> ibmbob-sdlc-eval-monitor`

## What To Check

- `ibmbob-sdlc-eval-monitor` が coverage gap と next action を返すこと
- 余計な authoring lane に入らないこと
- source-backed 評価でも evidence gate を通ること



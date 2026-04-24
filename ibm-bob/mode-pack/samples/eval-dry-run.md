<!-- ibm-bob/mode-pack/samples/eval-dry-run.md -->
<!-- Shows a Bob dry run for evaluation-only work after review history already exists. -->
<!-- This exists so the team can validate that eval requests route to P9 without extra authoring work. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/sdlc-guide.md, .bob/custom_modes.yaml, .copilot/prompts/P9_eval_monitor.md -->
# Eval Dry Run

## User Intent

「今ある review record を見て、次に足りない評価観点を整理したい」

## Suggested Mode Path

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> k5-retrieval-planner -> k7-artifact-context-packer -> k8-evidence-reviewer -> p9-eval-monitor -> c4-entry-response-shaper`

## What To Check

- `p9` が coverage gap と next action を返すこと
- 余計な authoring lane に入らないこと
- `c4` が短い日本語で返すこと

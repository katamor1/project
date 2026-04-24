<!-- ibm-bob/mode-pack/pilot-runbook.md -->
<!-- Defines regression checks for the old Bob family and manual pilot checks for the new direct-mode family. -->
<!-- This exists so the team can verify coexistence without turning generated Bob output into another source of truth. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/scripts/validate_mode_pack.py, ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/samples/direct-modes/generation-dry-run.md -->
# Pilot Runbook

## Goal

old family を壊さずに、new direct-mode family を同じ generated `.bob/custom_modes.yaml` に共存させることを確認します。

## Pre-Check

1. `py -3 ibm-bob/mode-pack/scripts/validate_mode_pack.py` を通す
2. generated `.bob/custom_modes.yaml` に old + new family の両方が入ることを確認する
3. generated `.bob/rules-{slug}/` が作られることを確認する
4. `.bob/ibm-bob/references/` が curated bundle だけで、source mirror ではないことを確認する
5. `.bob/` は generated output であり、source of truth ではないことを確認する

## Old Family Regression

- old 10 slug が同じ順序で残っている
- `routing/stage-flow.json` が未変更
- `evaluate_mode_pack.py --execution-mode simulate` が従来どおり動く

## New Family Manual Pilot

### Source-Backed Design

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-retrieval-planner -> ibmbob-artifact-context-packer -> ibmbob-evidence-reviewer -> ibmbob-sdlc-basic-design-author`

### Diff

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-legacy-diff-checker -> ibmbob-evidence-reviewer -> ibmbob-entry-response-shaper`

### Legacy Search

`ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-grounded-answerer`

### Review / Eval

`ibmbob-entry-router -> ... -> ibmbob-sdlc-review-record`

`ibmbob-entry-router -> ... -> ibmbob-sdlc-eval-monitor`

## Expected Safety

- `low confidence`, `ACL mismatch`, `authoritative conflict` は `ibmbob-entry-response-shaper` の abstain に落ちる
- destructive command は human checkpoint なしで進まない
- `artifact_context_packet` が無い source-backed generation は始まらない

## Dry-Run References

- `samples/direct-modes/generation-dry-run.md`
- `samples/direct-modes/diff-dry-run.md`
- `samples/direct-modes/review-dry-run.md`
- `samples/direct-modes/eval-dry-run.md`
- `samples/direct-modes/legacy-search-dry-run.md`

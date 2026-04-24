<!-- ibm-bob/mode-pack/pilot-runbook.md -->
<!-- Defines the manual pilot procedure for validating the IBM Bob custom mode pack against the existing design assets. -->
<!-- This exists so the team can check routing, abstains, and handoff discipline before relying on the Bob pack in daily work. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, ibm-bob/mode-pack/routing-map.md, ibm-bob/mode-pack/samples/generation-dry-run.md -->
# Pilot Runbook

## Goal

Bob custom mode pack が、既存の `C / K / P / LS` 設計と同じ流れで動くかを手動で確認します。

## Pre-Check

1. `.bob/custom_modes.yaml` を Bob が読める状態にする
2. `.bob/rules/` が project-level rule として有効なことを確認する
3. built-in mode を上書きしていないことを確認する
4. `edit` 制限が `.bob/`, `ibm-bob/`, `docs/`, `.copilot/` だけに向いていることを確認する

## Manual Pilot Flows

### Legacy Search

`ls-01-intake-router -> ls-02-glossary-normalizer -> ls-03-document-locator -> ls-04-evidence-verifier -> ls-05-grounded-answerer`

### Diff

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> k6-legacy-diff-checker -> k8-evidence-reviewer -> c4-entry-response-shaper`

### Source-Backed Design

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> k5-retrieval-planner -> k7-artifact-context-packer -> k8-evidence-reviewer -> p2-basic-design-author`

### Review

`c0-entry-router -> ... -> p8-review-record`

### Eval

`c0-entry-router -> ... -> p9-eval-monitor`

## Expected Safety Behavior

- `low confidence`, `ACL mismatch`, `authoritative conflict` は `c4` の `abstain + next action` に落ちる
- destructive command は human checkpoint なしで進まない
- `artifact_context_packet` がない source-backed generation は始まらない

## Record What Happened

各 pilot で次を残します。

- 使った mode
- 実際の route
- expected route と一致したか
- abstain が適切だったか
- handoff が欠けなかったか
- 次に直すべき rule または mode

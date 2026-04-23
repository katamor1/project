<!-- ibm-bob/mode-pack/rules/ibmbob-orchestrator/01-core.md -->
<!-- Defines the stage orchestration rules for the dedicated IBM-Bob Orchestrator mode. -->
<!-- This exists so Bob routes the IBM-Bob workflow through the fixed child-mode sequence and review gates. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/stage-flow.json, ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Orchestrator Rules

## Start
- 最初に `.bob/ibm-bob/current_run.json` を読む。
- 次に `.bob/ibm-bob/stage-flow.json` を読む。
- run artifact は `artifacts/ibm-bob/runs/<run-id>/` のみを正本として扱う。

## Fixed Flow
- `ibmbob-intake`
- `ibmbob-context-scout`
- `ibmbob-detail-design`
- `ibmbob-review-gate`
- `ibmbob-code-author`
- `ibmbob-review-gate`
- `ibmbob-unit-test-author`
- `ibmbob-review-gate`
- `ibmbob-unit-test-executor`
- `ibmbob-review-gate`
- `ibmbob-functional-test-author`
- `ibmbob-review-gate`
- `ibmbob-run-summary`

## Routing Rules
- `pass` は次の author mode へ進める。
- `revise` は review packet の `revise_to` へ戻す。
- `block` は human checkpoint に進める。
- `unit_test_run` の `revise` は `code`, `unit_test`, `intake` の 3 通りだけを許可する。

## Safety
- 原本の `ibm-bob/samples/base/` と `ibm-bob/samples/評価用基本設計書/` を編集しない。
- copied workspace の `source/` と run artifact 以外は編集しない。
- review packet のない stage jump をしない。

## Output
- 最後は `run-summary-packet` を更新して終える。
- block の場合でも `run-summary-packet` は必ず残す。

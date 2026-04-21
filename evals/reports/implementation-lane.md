<!-- evals/reports/implementation-lane.md -->
<!-- Defines the manual report format for implementation lane evaluation results. -->
<!-- This exists so implementation prep quality can be tracked without test automation yet. -->
<!-- RELEVANT FILES: evals/datasets/implementation-lane.md, docs/sdlc/templates/eval-report.md, .copilot/prompts/P9_eval_monitor.md -->
# Implementation Lane Report Notes

## 見ること
- full-stack slice の境界が妥当か
- API contract と DB change の参照があるか
- test pack の coverage が十分か
- repo bridge がそのまま handoff に使えるか

## 合格の目安
- missing surface が 0
- route ambiguity が 0
- repo bridge に必須 path がある
- review 後の next step が明確

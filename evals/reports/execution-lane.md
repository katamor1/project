<!-- evals/reports/execution-lane.md -->
<!-- Defines the manual report format for execution lane evaluation results. -->
<!-- This exists so near-code pack quality can be tracked before real repository integration starts. -->
<!-- RELEVANT FILES: evals/datasets/execution-lane.md, docs/sdlc/templates/eval-report.md, .copilot/prompts/P9_eval_monitor.md -->
# Execution Lane Report Notes

## 見ること
- target repo map が single app layout と一致しているか
- frontend, backend, sqlite, test の 4 pack が欠けずにあるか
- revise の戻り先が一意か
- near-code skeleton が implementer の追加判断なしで読めるか

## 合格の目安
- missing pack が 0
- route ambiguity が 0
- repo mapping ready が yes
- review 後の next step が明確

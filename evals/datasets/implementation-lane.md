<!-- evals/datasets/implementation-lane.md -->
<!-- Lists the manual evaluation targets for the implementation SDLC lane. -->
<!-- This exists so implementation prep can be checked consistently before touching a real app repo. -->
<!-- RELEVANT FILES: .copilot/routing/implementation-matrix.yaml, docs/sdlc/templates/test-spec.md, evals/reports/implementation-lane.md -->
# Implementation Lane Dataset Notes

## 評価対象
- slice boundary が 1 機能に閉じているか
- UI / API / DB の 3 点が欠けずに出ているか
- test pack に validation, permission, regression が入っているか
- repo bridge に target path と handoff order があるか

## 最低記録項目
- request_id
- slice_id
- review_decision
- revise_to
- missing_surface
- bridge_ready

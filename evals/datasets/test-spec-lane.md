<!-- evals/datasets/test-spec-lane.md -->
<!-- Lists the dataset notes for checking functional and integration specification coverage. -->
<!-- This exists so manual evaluation focuses on the minimum fields for the test spec lane. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-eval-monitor.md, docs/sdlc/templates/functional-spec.md, docs/sdlc/templates/integration-spec.md -->
# Test Spec Lane Dataset Notes

## Focus
- functional spec に主要シナリオと期待結果があるか
- integration spec に境界、整合、異常系があるか
- `P3_spec_reviewer` の差戻し先が一意か
- 受入条件や詳細設計との対応が見えるか

## Minimum Record
- `request_id`
- `artifact_type`
- `review_decision`
- `missing_scenarios`
- `missing_boundaries`
- `traceability_ready`

<!-- evals/reports/test-spec-lane.md -->
<!-- Lists the reporting notes for evaluating functional and integration spec outputs. -->
<!-- This exists so manual review can judge whether test intent is ready before implementation or execution prep. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-eval-monitor.md, evals/datasets/test-spec-lane.md, docs/sdlc/templates/review-summary.md -->
# Test Spec Lane Report Notes

## Checkpoints
- functional spec に正常系と異常系がある
- integration spec に API と DB の境界がある
- revise 先が functional か integration で一意である
- review record に渡せる粒度になっている

## Pass Threshold
- `missing_scenarios = 0`
- `missing_boundaries = 0`
- `route_ambiguity = 0`
- `traceability_ready = yes`

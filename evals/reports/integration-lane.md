<!-- evals/reports/integration-lane.md -->
<!-- Lists the reporting notes for evaluating integration lane outputs. -->
<!-- This exists so manual review can judge whether code draft bundles are ready to hand off to an app repo. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-eval-monitor.md, evals/datasets/integration-lane.md, docs/sdlc/templates/review-summary.md -->
# Integration Lane Report Notes

## Checkpoints
- target repo map と draft path が一致している
- frontend、backend、sqlite、test の 4 bundle がある
- revise 先が一意である
- approval pack が実装着手可否を明示している

## Pass Threshold
- `missing_bundle = 0`
- `route_ambiguity = 0`
- `repo_ready = yes`
- `approval_status` が明示されている

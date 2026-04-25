<!-- evals/datasets/integration-lane.md -->
<!-- Lists the dataset notes for checking code draft bundle coverage in the integration lane. -->
<!-- This exists so manual evaluation focuses on the minimum bundle fields before app repo handoff. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-eval-monitor.md, docs/sdlc/templates/approval-pack.md, docs/sdlc/samples/email-notification-integration-dry-run.md -->
# Integration Lane Dataset Notes

## Focus
- canonical root が維持されているか
- 4 つの draft bundle がそろっているか
- `files[]` に `skeleton_code` があるか
- approval pack に `approval_status` と `ready_for_repo` があるか

## Minimum Record
- `request_id`
- `slice_id`
- `review_decision`
- `approval_status`
- `missing_bundle`
- `repo_ready`

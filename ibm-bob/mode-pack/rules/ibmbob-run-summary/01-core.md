<!-- ibm-bob/mode-pack/rules/ibmbob-run-summary/01-core.md -->
<!-- Defines the final summary rules for IBM-Bob runs. -->
<!-- This exists so completed and blocked runs both end with one factual summary packet and markdown file. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/run-summary-packet.schema.json, ibm-bob/mode-pack/rules/orchestrator/01-core.md, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Run Summary Rules

## Required Output
- `status`
- `stage_results`
- `review_refs`
- `workspace_copy_root`
- `artifacts_root`
- `open_issues`
- `next_action`

## Summary Style
- 日本語で短く書く。
- 何が完了したか。
- どこで止まったか。
- 次に何をするべきか。

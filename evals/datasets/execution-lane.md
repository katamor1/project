<!-- evals/datasets/execution-lane.md -->
<!-- Lists the manual evaluation targets for the execution pack lane. -->
<!-- This exists so near-code bundles can be checked consistently before touching a real app repo. -->
<!-- RELEVANT FILES: .copilot/routing/execution-matrix.yaml, docs/sdlc/templates/test-codepack.md, evals/reports/execution-lane.md -->
# Execution Lane Dataset Notes

## 評価対象
- canonical root が `frontend/backend/db/tests` に揃っているか
- frontend codepack に target file, component contract, state flow があるか
- backend codepack に route, service, DB touchpoint, auth rule があるか
- sqlite change pack に migration draft と rollback notes があるか
- test codepack に unit, integration, regression があるか

## 最低記録項目
- request_id
- slice_id
- review_decision
- revise_to
- missing_pack
- repo_mapping_ready

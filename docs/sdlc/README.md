<!-- docs/sdlc/README.md -->
<!-- Explains the SDLC prompt registry, schemas, templates, and manual handoff flow across design, implementation, execution, and integration lanes. -->
<!-- This exists so the team can run upper SDLC, implementation prep, execution pack prep, and code draft prep inside Copilot with one consistent structure. -->
<!-- RELEVANT FILES: .copilot/routing/agent-matrix.yaml, .copilot/routing/integration-matrix.yaml, docs/sdlc/samples/email-notification-integration-dry-run.md -->
# SDLC Design Assets

このディレクトリは、`agent-research.md` を元にした SDLC 設計資産をまとめる場所です。

今回は `Copilot 内で準備する` ことを優先しています。

外部 runtime や自動実行基盤はまだ入れていません。

## Scope
- 対象は `P0` から `P21` までの設計レーン、実装準備レーン、execution pack レーン、integration レーンです。
- 実コード生成、migration 実行、test 実行、CI はまだ入れません。
- `legacy-search` は別ユースケースとしてそのまま残します。

## Directory Map
- `.copilot/prompts/`: prompt registry
- `.copilot/schemas/`: handoff schema
- `.copilot/routing/`: design lane、implementation lane、execution lane、integration lane の routing
- `docs/sdlc/templates/`: artifact template
- `docs/sdlc/samples/`: dry-run sample
- `reviews/spec/`, `reviews/code/`, `reviews/execution/`: review record placement
- `reviews/integration/`: integration review placement
- `artifacts/`: future artifact output placement
- `evals/`: future dataset and report placement

## Design Lane
1. `P0_orchestrator`
2. `P1_scope`
3. `P2_basic_design_author`
4. `P3_spec_reviewer`
5. `P2_detail_design_author`
6. `P3_spec_reviewer`
7. `P8_review_record`
8. `P9_eval_monitor`

`P3_spec_reviewer` は `pass | revise | block` を返します。

`revise` は直前の author に戻します。

`block` は `P0_orchestrator` に戻して human checkpoint を要求します。

## Implementation Lane
1. `P3_spec_reviewer` で `detail_design` が通過する
2. `P4_implementation_planner`
3. `P5_fullstack_slice_author`
4. `P6_test_author`
5. `P7_code_reviewer`
6. `P8_review_record`
7. `P9_eval_monitor`

`P7_code_reviewer` は `pass | revise | block` を返します。

`revise` は `P4`, `P5`, `P6` のどれかへ一意に戻します。

`pass` の時は `repo_bridge` を handoff artifact としてまとめます。

## Execution Pack Lane
1. `P7_code_reviewer` の通過、または `repo_bridge` 準備完了
2. `P10_execution_bundle_planner`
3. `P11_frontend_codepack_author`
4. `P12_backend_codepack_author`
5. `P13_test_codepack_author`
6. `P14_execution_reviewer`
7. `P8_review_record`
8. `P9_eval_monitor`

`P14_execution_reviewer` は `pass | revise | block` を返します。

`revise` は `P10`, `P11`, `P12`, `P13` のどれかへ一意に戻します。

`pass` の時は near-code pack 一式を review record へ渡します。

## Integration Lane
1. `P14_execution_reviewer` の通過、または `execution_bundle` 準備完了
2. `P15_integration_bundle_planner`
3. `P16_frontend_draft_author`
4. `P17_backend_draft_author`
5. `P18_sqlite_draft_author`
6. `P19_test_draft_author`
7. `P20_integration_reviewer`
8. `P21_approval_pack_author`
9. `P8_review_record`
10. `P9_eval_monitor`

`P20_integration_reviewer` は `pass | revise | block` を返します。

`revise` は `P15`, `P16`, `P17`, `P18`, `P19` のどれかへ一意に戻します。

`pass` の時は `approval_pack` を作り、実装着手可否と残リスクを明示します。

## How To Use
1. `P0_orchestrator` で raw request を `request_packet` にします。
2. `P1_scope` で対象と非対象を固定します。
3. `P2_basic_design_author` で基本設計を作ります。
4. `P3_spec_reviewer` でレビューします。
5. `detail_design` が通過したら implementation lane に入ります。
6. `P4` から `P7` で full-stack slice と test pack を固めます。
7. `repo_bridge` をまとめたら execution lane に進みます。
8. `P10` から `P14` で near-code pack を固めます。
9. `P15` から `P21` で code draft bundle と approval pack を固めます。
10. 最後に review record と eval へ進みます。

## Templates
- [scope.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/scope.md)
- [basic-design.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/basic-design.md)
- [detail-design.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/detail-design.md)
- [implementation-plan.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/implementation-plan.md)
- [fullstack-slice.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/fullstack-slice.md)
- [api-contract.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/api-contract.md)
- [db-change.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/db-change.md)
- [test-spec.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/test-spec.md)
- [repo-bridge.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/repo-bridge.md)
- [execution-bundle-plan.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/execution-bundle-plan.md)
- [target-repo-map.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/target-repo-map.md)
- [frontend-codepack.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/frontend-codepack.md)
- [backend-codepack.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/backend-codepack.md)
- [sqlite-change-pack.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/sqlite-change-pack.md)
- [test-codepack.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/test-codepack.md)
- [integration-bundle-plan.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/integration-bundle-plan.md)
- [frontend-draft-bundle.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/frontend-draft-bundle.md)
- [backend-draft-bundle.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/backend-draft-bundle.md)
- [sqlite-draft-bundle.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/sqlite-draft-bundle.md)
- [test-draft-bundle.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/test-draft-bundle.md)
- [approval-pack.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/approval-pack.md)
- [review-summary.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/review-summary.md)
- [review-record.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/review-record.md)
- [eval-report.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/eval-report.md)

## Dry Run
- [email-notification-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-dry-run.md)
- [email-notification-implementation-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-implementation-dry-run.md)
- [email-notification-execution-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-execution-dry-run.md)
- [email-notification-integration-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-integration-dry-run.md)

すべての sample は、`agent-research.md` のメール通知設定例を最小構成でなぞるためのものです。

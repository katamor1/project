<!-- docs/sdlc/README.md -->
<!-- Explains the SDLC prompt registry, schemas, templates, and manual handoff flow across design, test spec, implementation, execution, and integration lanes. -->
<!-- This exists so the team can run upper SDLC and downstream prep lanes inside entry with one consistent structure and a custom-engine bridge entry. -->
<!-- RELEVANT FILES: .copilot/routing/sdlc/ibmbob-design-flow.yaml, .copilot/routing/entry/ibmbob-entry-flow.yaml, docs/copilot-studio/custom-engine-bridge/README.md -->
# SDLC Design Assets

このディレクトリは、`agent-research.md` を元にした SDLC 設計資産をまとめる場所です。

今回は `entry 内で準備する` ことを優先しています。

外部 runtime や自動実行基盤はまだ入れていません。

## Scope
- 対象は `P0` から `P21` と `P2` 系の派生 author を含む設計レーン、test spec レーン、実装準備レーン、execution pack レーン、integration レーンです。
- 実コード生成、migration 実行、test 実行、CI はまだ入れません。
- `legacy-search` は別ユースケースとしてそのまま残します。
- source-backed generation は `docs/external-runtime/legacy-kb` からの `artifact_context_packet` を優先します。

## Directory Map
- `.copilot/prompts/`: prompt registry
- `.copilot/schemas/`: handoff schema
- `.copilot/routing/`: design lane、test spec lane、implementation lane、execution lane、integration lane の routing
- `docs/sdlc/templates/`: artifact template
- `docs/sdlc/samples/`: dry-run sample
- `docs/copilot-studio/custom-engine-bridge/`: entry 入口と external runtime の橋
- `docs/external-runtime/legacy-kb/`: legacy evidence ingest / retrieval / diff / context packing
- `reviews/spec/`, `reviews/code/`, `reviews/execution/`: review record placement
- `reviews/integration/`: integration review placement
- `artifacts/`: future artifact output placement
- `evals/`: future dataset and report placement

## Design Lane
1. `ibmbob-sdlc-request-orchestrator`
2. `ibmbob-sdlc-scope`
3. `ibmbob-sdlc-basic-design-author`
4. `ibmbob-sdlc-spec-reviewer`
5. `ibmbob-sdlc-detail-design-author`
6. `ibmbob-sdlc-spec-reviewer`
7. `ibmbob-sdlc-review-record`
8. `ibmbob-sdlc-eval-monitor`

`ibmbob-sdlc-spec-reviewer` は `pass | revise | block` を返します。

`revise` は直前の author に戻します。

`block` は `ibmbob-sdlc-request-orchestrator` に戻して human checkpoint を要求します。

## Test Spec Lane
1. `ibmbob-sdlc-spec-reviewer` で `detail_design` が通過する
2. `ibmbob-sdlc-functional-spec-author` または `ibmbob-sdlc-integration-spec-author`
3. `ibmbob-sdlc-spec-reviewer`
4. `ibmbob-sdlc-review-record`
5. `ibmbob-sdlc-eval-monitor`

`functional_spec` は業務シナリオ、画面操作、期待結果を固めます。

`integration_spec` は API、DB、依存境界、異常系、復旧観点を固めます。

`ibmbob-sdlc-spec-reviewer` の `revise` は作成した spec author に戻します。

## Implementation Lane
1. `ibmbob-sdlc-spec-reviewer` で `detail_design` が通過する
2. `ibmbob-sdlc-implementation-planner`
3. `ibmbob-sdlc-fullstack-slice-author`
4. `ibmbob-sdlc-test-author`
5. `ibmbob-sdlc-code-reviewer`
6. `ibmbob-sdlc-review-record`
7. `ibmbob-sdlc-eval-monitor`

`ibmbob-sdlc-code-reviewer` は `pass | revise | block` を返します。

`revise` は `P4`, `P5`, `P6` のどれかへ一意に戻します。

`pass` の時は `repo_bridge` を handoff artifact としてまとめます。

## Execution Pack Lane
1. `ibmbob-sdlc-code-reviewer` の通過、または `repo_bridge` 準備完了
2. `ibmbob-sdlc-execution-bundle-planner`
3. `ibmbob-sdlc-frontend-codepack-author`
4. `ibmbob-sdlc-backend-codepack-author`
5. `ibmbob-sdlc-test-codepack-author`
6. `ibmbob-sdlc-execution-reviewer`
7. `ibmbob-sdlc-review-record`
8. `ibmbob-sdlc-eval-monitor`

`ibmbob-sdlc-execution-reviewer` は `pass | revise | block` を返します。

`revise` は `P10`, `P11`, `P12`, `P13` のどれかへ一意に戻します。

`pass` の時は near-code pack 一式を review record へ渡します。

## Integration Lane
1. `ibmbob-sdlc-execution-reviewer` の通過、または `execution_bundle` 準備完了
2. `ibmbob-sdlc-integration-bundle-planner`
3. `ibmbob-sdlc-frontend-draft-author`
4. `ibmbob-sdlc-backend-draft-author`
5. `ibmbob-sdlc-sqlite-draft-author`
6. `ibmbob-sdlc-test-draft-author`
7. `ibmbob-sdlc-integration-reviewer`
8. `ibmbob-sdlc-approval-pack-author`
9. `ibmbob-sdlc-review-record`
10. `ibmbob-sdlc-eval-monitor`

`ibmbob-sdlc-integration-reviewer` は `pass | revise | block` を返します。

`revise` は `P15`, `P16`, `P17`, `P18`, `P19` のどれかへ一意に戻します。

`pass` の時は `approval_pack` を作り、実装着手可否と残リスクを明示します。

## How To Use
1. `ibmbob-sdlc-request-orchestrator` で raw request を `request_packet` にします。
2. `ibmbob-sdlc-scope` で対象と非対象を固定します。
3. legacy 文書を根拠にする時は、先に `docs/copilot-studio/custom-engine-bridge` の `C0-C5` で入口を通します。
4. bridge から `docs/external-runtime/legacy-kb` の `K0-K8` に入り、`artifact_context_packet` を作ります。
5. `ibmbob-sdlc-basic-design-author` 以降は `artifact_context_packet` を優先して使います。
6. `ibmbob-sdlc-spec-reviewer` でレビューします。
7. `detail_design` が通過したら、必要に応じて test spec lane と implementation lane に入ります。
8. `ibmbob-sdlc-functional-spec-author` と `ibmbob-sdlc-integration-spec-author` で spec を固めます。
9. `P4` から `P7` で full-stack slice と test pack を固めます。
10. `repo_bridge` をまとめたら execution lane に進みます。
11. `P10` から `P14` で near-code pack を固めます。
12. `P15` から `P21` で code draft bundle と approval pack を固めます。
13. 最後に review record と eval へ進みます。

## Templates
- [scope.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/scope.md)
- [basic-design.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/basic-design.md)
- [detail-design.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/detail-design.md)
- [functional-spec.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/functional-spec.md)
- [integration-spec.md](/C:/Users/stell/source/repos/project/docs/sdlc/templates/integration-spec.md)
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
- [email-notification-test-spec-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-test-spec-dry-run.md)
- [email-notification-implementation-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-implementation-dry-run.md)
- [email-notification-execution-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-execution-dry-run.md)
- [email-notification-integration-dry-run.md](/C:/Users/stell/source/repos/project/docs/sdlc/samples/email-notification-integration-dry-run.md)

すべての sample は、`agent-research.md` のメール通知設定例を最小構成でなぞるためのものです。



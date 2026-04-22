# SDLC Execution Pack 拡張計画 v1

## Summary
- 次フェーズは、今ある `design lane + implementation lane` の先に、`Execution Pack lane` を追加する。
- 目的は、`repo_bridge` の先を `Near-code Pack` まで具体化し、将来の app repo 実装へそのまま渡せる bundle を作ること。
- 前提は引き続き `Copilot-only`。実コード生成、migration 実行、test 実行、CI 連携はまだ入れない。
- target repo は `Single App Layout` に固定する。path 規約は `frontend/`, `backend/`, `db/` をトップレベルに分ける。
- bundle は `Code Intent Pack` ではなく `Near-code Pack` にする。つまり path, file role, interface shape に加えて、TSX / Python / SQL / test の雛形レベルまで設計資産に含める。

## Key Changes
- `.copilot/prompts/` に execution lane 用の 5 prompt を追加する。
- `P10_execution_bundle_planner`: `repo_bridge + fullstack_slice + test_pack` を `execution_bundle_packet` に変換する。
- `P11_frontend_codepack_author`: `frontend/app`, `frontend/components`, `frontend/lib` 向けの near-code pack を作る。
- `P12_backend_codepack_author`: `backend/api`, `backend/services`, `backend/models` 向けの near-code pack を作る。
- `P13_test_codepack_author`: `tests/unit`, `tests/integration`, `tests/regression` 向けの near-code test pack を作る。
- `P14_execution_reviewer`: 3 つの codepack と path mapping をレビューし、`pass | revise | block` を返す。
- `.copilot/routing/execution-matrix.yaml` を追加する。
- 標準ルートは `P7(code review pass) or repo_bridge_ready -> P10 -> P11 -> P12 -> P13 -> P14 -> P8 -> P9` にする。
- `P14 revise` は category ごとに戻す。
- `frontend/interface` の指摘は `P11`。
- `backend/db contract` の指摘は `P12`。
- `test coverage` の指摘は `P13`。
- `bundle boundary` や `repo mapping` の破綻は `P10`。
- `P14 block` は `P0` に戻して human checkpoint を要求する。
- `.copilot/schemas/` に execution lane 用 schema を追加する。
- `execution-bundle-packet.schema.json`: 必須は `request_id`, `slice_id`, `repo_bridge_ref`, `target_repo_shape`, `target_paths`, `bundle_targets`, `constraints`, `done_criteria`, `next_agent`。
- `target-repo-map.schema.json`: 必須は `repo_shape`, `path_style`, `frontend_roots`, `backend_roots`, `db_roots`, `test_roots`, `naming_rules`, `next_agent`。
- `frontend-codepack.schema.json`: 必須は `artifact_manifest`, `slice_id`, `target_files`, `component_contracts`, `state_flow`, `api_calls`, `acceptance_refs`, `next_agent`。
- `backend-codepack.schema.json`: 必須は `artifact_manifest`, `slice_id`, `target_files`, `route_contracts`, `service_flow`, `db_touchpoints`, `auth_rules`, `next_agent`。
- `sqlite-change-pack.schema.json`: 必須は `artifact_manifest`, `slice_id`, `target_files`, `schema_changes`, `migration_draft`, `rollback_notes`, `next_agent`。
- `test-codepack.schema.json`: 必須は `artifact_manifest`, `slice_id`, `target_files`, `unit_cases`, `integration_cases`, `regression_cases`, `fixtures`, `next_agent`。
- `execution-review-findings.schema.json`: 必須は `artifact_refs`, `review_decision`, `findings`, `revise_to`, `human_checkpoint`, `next_agent`。
- `artifact-manifest` の必須キーは維持し、`artifact_type` の運用値だけ追加する。
- `docs/sdlc/templates/` を execution lane 向けに拡張する。
- `execution-bundle-plan.md`: bundle target, repo shape, bundle scope, constraints, done criteria, open questions。
- `target-repo-map.md`: canonical roots, naming rules, file ownership, handoff order。
- `frontend-codepack.md`: target files, component outline, state flow, API client use, acceptance refs。
- `backend-codepack.md`: route outline, service outline, DB touchpoints, auth rules, error notes。
- `sqlite-change-pack.md`: target files, schema diff, migration draft, rollback notes。
- `test-codepack.md`: target files, unit, integration, regression, fixtures, execution notes。
- 既存の `review-summary.md` と `review-record.md` は execution lane にも再利用する。
- `docs/sdlc/README.md` は 3 レーン構成に更新する。
- `Execution Pack lane` を追加し、design -> implementation prep -> execution pack の接続を明記する。
- canonical root は次で固定する。
- `frontend/app/`, `frontend/components/`, `frontend/lib/`
- `backend/api/`, `backend/services/`, `backend/models/`
- `db/schema/`, `db/migrations/`
- `tests/unit/`, `tests/integration/`, `tests/regression/`
- `docs/sdlc/samples/` に `email-notification-execution-dry-run.md` を 1 本追加する。
- 流れは `repo_bridge -> execution_bundle_packet -> frontend_codepack -> backend_codepack -> sqlite_change_pack -> test_codepack -> execution_review_findings` にする。
- placeholder 置き場を追加する。
- `reviews/execution/` を新設して execution review 記録置き場にする。
- `artifacts/execution/` を新設して near-code pack の置き場にする。
- `evals/datasets/` と `evals/reports/` に execution lane 用の評価観点ファイルを 1 本ずつ追加する。

## Test Plan
- 静的確認を行う。
- 新しい 5 prompt が `System / User / Assistant / JSON Output Example` を持つことを確認する。
- 新しい schema が 4 行 header を除けば parse できることを確認する。
- 追加した Markdown / JSON / YAML がすべて 300 行未満であることを確認する。
- routing 確認を行う。
- `execution-matrix.yaml` が `repo_bridge_ready` または `P7 pass` 起点を持つこと。
- `P14 revise` が `P10`, `P11`, `P12`, `P13` のどれかに一意に戻ること。
- `P14 block` が `P0` に戻ること。
- dry-run 確認を 1 本行う。
- `repo_bridge` から `execution_bundle_packet` に path root が落ちずに渡ること。
- `frontend_codepack` に target file, component outline, state flow があること。
- `backend_codepack` に route, service, DB touchpoint, auth rule があること。
- `sqlite_change_pack` に migration draft と rollback notes があること。
- `test_codepack` に unit, integration, regression があること。
- 受入条件は次で固定する。
- 実装準備レーンの成果物から、single app repo 向けの near-code bundle まで repo 内だけで追跡できる。
- implementer が target path, file role, interface skeleton, test skeleton を自分で決めなくて済む。
- 次フェーズで本当に app repo へ接続する時、bundle contract と repo mapping を再設計しなくて済む。

## Assumptions
- このフェーズでも実コードファイル、migration 実行、test 実行、CI、外部 runtime は追加しない。
- `Near-code Pack` は runnable code ではなく、実装直前の skeleton と contract を含む設計資産として扱う。
- target repo は `single app repo` で固定し、path 規約は `frontend/backend/db` 分離にする。
- frontend は `Next.js App Router` 風の `frontend/app/` 前提で整理する。
- backend は framework 固定を避け、`backend/api + services + models` の Python 汎用構成として整理する。
- SQLite 変更は `backend codepack` とは別に `sqlite_change_pack` で明示し、DB 変更の責務を見失わない形にする。

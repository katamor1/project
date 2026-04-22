# SDLC Integration Lane 設計計画 v1

## Summary
- 次フェーズは、`Execution Pack lane` の先に `Integration Lane` を追加する。
- 目的は、near-code pack を `Code Draft Bundle` まで進めて、将来の app repo 実装へそのまま渡せる `skeleton code + approval pack` を作ること。
- まだ実コード生成、migration 実行、test 実行、CI 連携は入れない。
- target repo は引き続き `single app layout` 固定で、canonical root は `frontend/`, `backend/`, `db/`, `tests/` を維持する。
- code draft の粒度は `Skeleton Code` に固定する。つまり `imports`, `type/route/function signature`, `TODO body`, `SQL draft`, `test skeleton` を含むが、runnable code とは扱わない。
- Integration Lane の終点には `Explicit Approval Pack` を置く。review verdict だけで終わらせず、実装着手可否を独立 artifact で残す。

## Key Changes
- `.copilot/prompts/` に integration lane 用の 7 prompt を追加する。
- `P15_integration_bundle_planner`: `execution_bundle_packet + target_repo_map + near-code packs` を `code_draft_packet` に変換する。
- `P16_frontend_draft_author`: `frontend/app`, `frontend/components`, `frontend/lib` 向けの skeleton TSX/TS bundle を作る。
- `P17_backend_draft_author`: `backend/api`, `backend/services`, `backend/models` 向けの skeleton Python bundle を作る。
- `P18_sqlite_draft_author`: `db/schema`, `db/migrations` 向けの skeleton SQL bundle を作る。
- `P19_test_draft_author`: `tests/unit`, `tests/integration`, `tests/regression` 向けの skeleton test bundle を作る。
- `P20_integration_reviewer`: 4 つの draft bundle と repo mapping をレビューし、`pass | revise | block` を返す。
- `P21_approval_pack_author`: reviewer 結果を `approval_pack` にまとめ、実装着手可否、残リスク、手動手順を明示する。
- `.copilot/routing/integration-matrix.yaml` を追加する。
- 標準ルートは `P14 pass or execution_bundle_ready -> P15 -> P16 -> P17 -> P18 -> P19 -> P20 -> P21 -> P8 -> P9` に固定する。
- `P20 revise` は category ごとに一意に戻す。
- `frontend_contract` は `P16`
- `backend_contract` は `P17`
- `sqlite_migration` は `P18`
- `test_coverage` は `P19`
- `bundle_boundary` と `repo_mapping` は `P15`
- `P20 block` は `P0` に戻して human checkpoint を要求する。
- `.copilot/schemas/` に integration lane 用 schema を追加する。
- `code-draft-packet.schema.json`: 必須は `request_id`, `slice_id`, `execution_bundle_ref`, `target_repo_map_ref`, `draft_targets`, `constraints`, `done_criteria`, `next_agent`。
- `draft-file.schema.json`: 必須は `path`, `language`, `purpose`, `skeleton_code`, `open_questions`。
- `frontend-draft-bundle.schema.json`: 必須は `artifact_manifest`, `slice_id`, `files`, `component_contracts`, `state_flow`, `next_agent`。
- `backend-draft-bundle.schema.json`: 必須は `artifact_manifest`, `slice_id`, `files`, `route_contracts`, `service_flow`, `auth_rules`, `next_agent`。
- `sqlite-draft-bundle.schema.json`: 必須は `artifact_manifest`, `slice_id`, `files`, `schema_changes`, `migration_notes`, `rollback_notes`, `next_agent`。
- `test-draft-bundle.schema.json`: 必須は `artifact_manifest`, `slice_id`, `files`, `unit_cases`, `integration_cases`, `regression_cases`, `fixtures`, `next_agent`。
- `integration-review-findings.schema.json`: 必須は `artifact_refs`, `review_decision`, `findings`, `revise_to`, `human_checkpoint`, `next_agent`。
- `approval-pack.schema.json`: 必須は `artifact_manifest`, `artifact_refs`, `approval_status`, `ready_for_repo`, `manual_steps`, `residual_risks`, `next_agent`。
- `artifact-manifest` の必須キーは維持し、`artifact_type` の運用値だけ増やす。
- `docs/sdlc/templates/` を integration lane 向けに拡張する。
- `integration-bundle-plan.md`: draft target, repo refs, constraints, done criteria, open questions。
- `frontend-draft-bundle.md`: target files, component outline, state flow, skeleton code notes。
- `backend-draft-bundle.md`: route outline, service outline, auth rules, skeleton code notes。
- `sqlite-draft-bundle.md`: schema file, migration file, SQL skeleton, rollback notes。
- `test-draft-bundle.md`: unit, integration, regression の target files と skeleton notes。
- `approval-pack.md`: approval status, ready_for_repo, artifact refs, residual risks, manual steps。
- `review-summary.md` と `review-record.md` は integration lane にも再利用する。
- `docs/sdlc/README.md` は 4 レーン構成に更新する。
- `Integration Lane` を追加し、`execution pack -> code draft bundle -> approval pack` の接続を明記する。
- canonical root は現状維持で固定する。
- `frontend/app/`, `frontend/components/`, `frontend/lib/`
- `backend/api/`, `backend/services/`, `backend/models/`
- `db/schema/`, `db/migrations/`
- `tests/unit/`, `tests/integration/`, `tests/regression/`
- `docs/sdlc/samples/` に `email-notification-integration-dry-run.md` を 1 本追加する。
- 流れは `execution_bundle_packet -> frontend_draft_bundle -> backend_draft_bundle -> sqlite_draft_bundle -> test_draft_bundle -> integration_review_findings -> approval_pack` にする。
- placeholder 置き場を追加する。
- `reviews/integration/` を新設して integration review 記録置き場にする。
- `artifacts/integration/` を新設して code draft bundle の置き場にする。
- `evals/datasets/` と `evals/reports/` に integration lane 用の評価観点ファイルを 1 本ずつ追加する。

## Test Plan
- 静的確認を行う。
- 新しい 7 prompt が `System / User / Assistant / JSON Output Example` を持つことを確認する。
- 新しい schema が 4 行 header を除けば parse できることを確認する。
- 追加した Markdown / JSON / YAML がすべて 300 行未満であることを確認する。
- routing 確認を行う。
- `integration-matrix.yaml` が `P14 pass` または `execution_bundle_ready` 起点を持つこと。
- `P20 revise` が `P15`, `P16`, `P17`, `P18`, `P19` のどれかに一意に戻ること。
- `P20 block` が `P0` に戻ること。
- dry-run 確認を 1 本行う。
- `execution_bundle_packet` から `code_draft_packet` に canonical root と draft target が落ちずに渡ること。
- `frontend_draft_bundle` に `files[]` と `skeleton_code` があること。
- `backend_draft_bundle` に route/service/auth の skeleton があること。
- `sqlite_draft_bundle` に SQL skeleton と rollback notes があること。
- `test_draft_bundle` に unit, integration, regression の skeleton があること。
- `approval_pack` に `approval_status`, `ready_for_repo`, `manual_steps`, `residual_risks` があること。
- 受入条件は次で固定する。
- Execution Pack の成果物から、single app repo 向け `Code Draft Bundle + Approval Pack` まで repo 内だけで追跡できる。
- implementer が target path, file role, signature, TODO body, SQL draft, test skeleton, 実装着手可否を自分で決めなくて済む。
- 次フェーズで本当に app repo 接続や code generation を始める時、bundle contract と approval gate を再設計しなくて済む。

## Assumptions
- このフェーズでも runnable code、migration 実行、test 実行、CI、外部 runtime は追加しない。
- `Skeleton Code` は `imports`, `type/route/function signature`, `TODO body`, `SQL draft`, `test skeleton` を含むが、実行可能性は保証しない。
- frontend は `Next.js App Router` 風の TSX skeleton に固定する。
- backend は framework 固定を避け、`api + services + models` の Python 汎用 skeleton にする。
- SQLite は backend bundle と分けて `sqlite_draft_bundle` で明示し、DB 変更責務を独立管理する。
- approval gate は reviewer verdict の複製ではなく、`実装着手可否` と `残リスク` を明示する別 artifact として扱う。

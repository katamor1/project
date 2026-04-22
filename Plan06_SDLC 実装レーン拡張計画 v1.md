# SDLC 実装レーン拡張計画 v1

## Summary
- 次フェーズは、今ある上流 SDLC 資産を `Implementation SDLC` まで伸ばします。
- 前提は `Copilot-only` のままです。外部 custom engine や実コード生成はまだ入れません。
- ただし `Full Bridge` を選ぶので、将来の app repo に渡せる `implementation packet -> full-stack slice -> test pack -> code review -> repo bridge` の資産まで設計します。
- 最初の bridge 対象は `1 機能 = 1 full-stack slice` に固定します。画面、API、DB、test を同じ handoff 単位で扱います。

## Key Changes
- `.copilot/prompts/` に実装レーンの 4 prompt を追加する。
- `P4_implementation_planner`: 承認済み `detail_design` を `implementation_packet` に変換する。
- `P5_fullstack_slice_author`: 1 機能分の UI / API / DB / file intent をまとめた `fullstack_slice` を作る。
- `P6_test_author`: `fullstack_slice` から unit / integration / regression 観点の `test_pack` を作る。
- `P7_code_reviewer`: `fullstack_slice + test_pack` をレビューし、`pass | revise | block` を返す。
- routing は 2 本立てにする。
- 既存の上流レーンはそのまま残す。
- 新規に `.copilot/routing/implementation-matrix.yaml` を追加し、標準ルートを `P3(detail pass) -> P4 -> P5 -> P6 -> P7 -> P8 -> P9` に固定する。
- 差戻しは明示する。
- `P7` の code/interface 指摘は `P5` に戻す。
- `P7` の test coverage 指摘は `P6` に戻す。
- `P7` の slice boundary や前提破綻は `P4` に戻す。
- `P7` の `block` は `P0` に戻して human checkpoint を要求する。
- `.copilot/schemas/` に実装レーン用 schema を追加する。
- `implementation-packet.schema.json`: 必須は `request_id`, `detail_design_ref`, `slice_id`, `ui_scope`, `api_scope`, `db_scope`, `constraints`, `done_criteria`, `next_agent`。
- `fullstack-slice.schema.json`: 必須は `artifact_manifest`, `slice_id`, `frontend_changes`, `backend_changes`, `db_changes`, `contract_refs`, `risk_flags`, `next_agent`。
- `test-pack.schema.json`: 必須は `artifact_manifest`, `slice_id`, `test_levels`, `acceptance_cases`, `fixtures`, `coverage_focus`, `next_agent`。
- `code-review-findings.schema.json`: 必須は `artifact_ref`, `review_decision`, `findings`, `revise_to`, `human_checkpoint`, `next_agent`。
- `repo-bridge.schema.json`: 必須は `artifact_manifest`, `target_stack`, `target_paths`, `handoff_order`, `manual_steps`, `open_questions`, `next_agent`。
- 必要なら `api-contract.schema.json` と `db-change.schema.json` を補助 schema として追加する。
- `artifact-manifest` の必須キーはそのまま使い、`artifact_type` の運用値だけ増やす。
- `docs/sdlc/templates/` を実装レーン向けに拡張する。
- `implementation-plan.md`: 目的, slice boundary, UI changes, API changes, DB changes, dependencies, done criteria, open questions。
- `fullstack-slice.md`: frontend files, backend files, DB changes, contract refs, traceability, risks。
- `api-contract.md`: endpoint, auth, request, response, errors, source refs。
- `db-change.md`: tables, fields, migration, backfill, rollback, source refs。
- `test-spec.md`: happy path, validation, permission, regression, fixture notes。
- `repo-bridge.md`: target repo paths, file intent, execution order, manual handoff notes。
- 既存の `review-summary.md` と `review-record.md` はコードレビューにも再利用する。
- `docs/sdlc/README.md` は 2 レーン構成に更新する。
- 上流レーンと実装レーンのつながりを明記する。
- `full-stack slice` は `Next.js / TypeScript / Python / SQLite` 前提の handoff 例で書く。
- `docs/sdlc/samples/` に 1 本だけ実装側 dry-run を追加する。
- テーマは同じく `メール通知設定` に固定する。
- 流れは `detail_design -> implementation_packet -> fullstack_slice -> test_pack -> code_review_findings -> repo_bridge` にする。
- placeholder 置き場を追加する。
- `reviews/code/` を新設して code review 記録置き場にする。
- `artifacts/implementation/` を新設して将来の slice artifact 置き場にする。
- `evals/datasets/` と `evals/reports/` には implementation lane の評価観点 README を 1 枚ずつ追加する。

## Test Plan
- 静的確認を行う。
- 新しい 4 prompt が既存と同じ `System / User / Assistant / JSON Output Example` 構成を持つことを確認する。
- 新しい schema が 4 行 header を除けば parse できることを確認する。
- 追加した Markdown / JSON / YAML がすべて 300 行未満であることを確認する。
- ルーティング確認を行う。
- `implementation-matrix.yaml` が `P3(detail pass)` 起点で始まること。
- `P7 revise` が `P5`, `P6`, `P4` のどれかに一意に戻ること。
- `P7 block` が `P0` に戻ること。
- dry-run 確認を 1 本行う。
- `detail_design` から `implementation_packet` へ必要項目が落ちずに渡ること。
- `fullstack_slice` に UI / API / DB / test の 4 観点がそろうこと。
- `repo_bridge` に target path, handoff order, manual steps が入ること。
- 受入条件は次で固定する。
- 上流 SDLC の成果物から、将来の app repo 実装に渡すための bridge 資産まで repo 内だけで追跡できる。
- 1 機能分の full-stack slice を、人手で `設計 -> 実装準備 -> テスト準備 -> レビュー -> handoff` の順にたどれる。
- 次フェーズで本当にコード生成や app repo 接続を始める時、prompt / schema / artifact naming を再設計しなくて済む。

## Assumptions
- このフェーズでも実コード、migration、test 実行、CI、外部 runtime は追加しない。
- `Copilot-only` を守るので、repo bridge は docs/schema/handoff package までにとどめる。
- `Full Bridge` は「将来の app repo にそのまま渡せる設計資産」を意味し、実 repo への自動反映はまだ対象外とする。
- `Full-stack slice` は 1 機能単位で固定し、複数 slice の並列分割や dependency scheduler は後続フェーズに回す。
- test は executable test code ではなく、`test_pack` と `test-spec` の設計資産として扱う。

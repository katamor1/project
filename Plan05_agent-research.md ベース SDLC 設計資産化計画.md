# agent-research.md ベース SDLC 設計資産化計画

## Summary
- 次フェーズでは、既存の `legacy-search` とは別トラックで、`agent-research.md` の上流 SDLC 設計を repo 資産として落とす。
- 今回は `Copilot内で準備` に寄せる。外部 custom engine や実行基盤は入れず、将来の hybrid 化に耐える `prompt registry`, `schema`, `artifact template`, `routing` を先に固める。
- 対象範囲は `案件司令塔`, `要件正規化`, `基本設計`, `詳細設計`, `仕様レビュー`, `レビュー記録`, `評価監視` の 7 役まで。実装分割、コード生成、単体テスト、コードレビューはこのフェーズでは含めない。
- 現在の repo には `docs/claude-code/legacy-search` はあるが、`.copilot/`, `docs/sdlc/`, `reviews/`, `artifacts/`, `evals/` は未作成なので、ここを新設前提で計画する。

## Key Changes
- 新しい SDLC トラックを 3 層で作る。
- `.copilot/` に prompt と schema と routing を置く。
- `docs/sdlc/` に成果物テンプレートと運用ガイドを置く。
- `reviews/spec/` と `evals/` に review/eval の記録先テンプレートを置く。
- `legacy-search` は別ユースケースとして残し、今回の SDLC トラックへは統合しない。
- `.copilot/prompts/` には 7 本の prompt を置く。
- `P0_orchestrator.md`
- `P1_scope.md`
- `P2_basic_design_author.md`
- `P2_detail_design_author.md`
- `P3_spec_reviewer.md`
- `P8_review_record.md`
- `P9_eval_monitor.md`
- 各 prompt は `system / user / assistant` の 3 部で統一し、`next_agent`, `artifact_type`, `required_inputs`, `human_checkpoint`, `done_definition` の扱いを固定する。
- 外部 runtime 前提の記述は入れず、Copilot 内で準備できる粒度に落とす。
- `.copilot/schemas/` には 6 つの schema を置く。
- `request-packet.schema.json`
- `scope-packet.schema.json`
- `work-packet.schema.json`
- `review-findings.schema.json`
- `review-record.schema.json`
- `artifact-manifest.schema.json`
- `artifact-manifest` には少なくとも `artifact_id`, `artifact_type`, `status`, `prompt_id`, `prompt_version`, `model_id`, `upstream_refs`, `review_record_refs`, `language` を必須にする。
- `.copilot/routing/agent-matrix.yaml` を置く。
- `P0 -> P1 -> P2_basic -> P3 -> P2_detail -> P3 -> P8 -> P9` の標準ルートを定義する。
- 差戻しルートも明記する。
- `P3` は `pass | revise | block` で返し、`revise` は直前の author に戻す。
- `block` は `P0` に戻して human checkpoint を要求する。
- `docs/sdlc/` には詳細テンプレートを置く。
- `README.md` で全体像を説明する。
- `templates/scope.md`, `templates/basic-design.md`, `templates/detail-design.md`, `templates/review-summary.md`, `templates/review-record.md`, `templates/eval-report.md` を置く。
- すべて 4 行ヘッダーコメント付きにする。
- `basic-design.md` と `detail-design.md` は section を固定する。
- `scope`: 目的, 対象, 非対象, 受入条件, 前提, 用語, 未解決
- `basic design`: 目的, 画面, API, 業務ルール, 非対象, 受入条件, トレーサビリティ
- `detail design`: API 契約, 永続化, 状態, 例外, 権限, 前提, トレーサビリティ
- `reviews/spec/` には `basic/`, `detail/` の記録置き場だけを先に設計する。
- `evals/` には `datasets/`, `reports/` の 2 系統だけを置く。
- このフェーズでは traces の実装までは入れず、`eval-report.md` と dataset 定義のテンプレートだけにとどめる。
- サンプルは 1 本だけ入れる。
- `agent-research.md` のメール通知設定例を使い、`request_packet -> scope_packet -> basic_design -> review_findings -> review_record` までの最小サンプルを docs 側に置く。
- 目的は prompt と schema のつながり確認であり、実装コードや test spec までは作らない。

## Test Plan
- 静的確認を行う。
- `.copilot/`, `docs/sdlc/`, `reviews/spec/`, `evals/` の必要ディレクトリがそろっていることを確認する。
- すべての Markdown / JSON / YAML が 300 行未満または適切な分割になっていることを確認する。
- 7 本の prompt に共通見出しと JSON 出力例があることを確認する。
- schema 整合を確認する。
- `request_packet -> scope_packet -> work_packet -> review_findings -> review_record` の各サンプルが schema と矛盾しないことを確認する。
- artifact template 整合を確認する。
- `basic-design.md` と `detail-design.md` の front matter が `artifact-manifest.schema` と一致することを確認する。
- 1 本の dry-run シナリオで確認する。
- メール通知設定の例で、`P0` が `P1` に渡せること。
- `P1` から `P2_basic` へ必要項目がそろうこと。
- `P3` が `revise` を返した時に差戻し先が一意であること。
- `P8` が review record を生成できること。
- 受入条件は次で固定する。
- 上流 SDLC の 7 役が repo 内の資産だけで追跡可能になる。
- 1 本の要件から `scope`, `basic design`, `detail design`, `spec review`, `review record`, `eval report` までの流れが、人手でなぞれる。
- 次フェーズで implementation 系を足す時に、prompt / schema / artifact naming を再設計しなくて済む。

## Assumptions
- このフェーズでは `agent-research.md` の full 15 agent ではなく、上流+レビューの 7 役に絞る。
- 実装分割、UI/API/DB 実装、単体テスト、コードレビューは後続フェーズに回す。
- `Copilot内で準備` を優先し、external custom engine, MCP, CI 連携, trace 実装, artifact 自動生成は入れない。
- 詳細テンプレートは強めに固定するが、wire 仕様や validation の細部まではまだ決めない。
- `legacy-search` は検索ユースケース専用のまま残し、今回の SDLC トラックとは別ディレクトリで管理する。

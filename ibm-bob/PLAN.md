# IBM-Bob SDLC Custom Mode Pack

## Summary
- IBM Bob の project custom mode として、`orchestrator` を project override し、その配下に 8 個の子モードを持つ `IBM-Bob SDLC Orchestrator` を作る。
- 実行開始時に対象プロジェクトと入力基本設計書をワーク領域へ複製し、原本は一切編集しない。
- フローは `調査 -> 詳細設計 -> レビュー -> コーディング -> レビュー -> 単体テスト作成 -> レビュー -> 単体テスト実行 -> レビュー -> 機能テスト作成 -> レビュー -> 実行サマリ` に固定する。
- レビュー方針は `pass | revise | block`。`pass` は自動継続、`revise` は該当ステージへ差し戻し、`block` のみ human checkpoint で停止する。
- 正本は JSON、閲覧用は Markdown とし、v1 の受け入れ評価は `ibm_bob_評価用基本設計書_1-5.docx` の 5 件すべてを対象にする。

## Implementation Changes
- `ibm-bob/mode-pack/` を source of truth にして、Bob 登録用の `.bob/custom_modes.yaml` と `.bob/rules-{slug}/` を生成する installer を用意する。評価時は copied workspace の `.bob/` にだけ配置する。
- 追加する mode slug は `orchestrator`、`ibmbob-intake`、`ibmbob-context-scout`、`ibmbob-detail-design`、`ibmbob-review-gate`、`ibmbob-code-author`、`ibmbob-unit-test-author`、`ibmbob-unit-test-executor`、`ibmbob-functional-test-author`、`ibmbob-run-summary` に固定する。
- `orchestrator` は Bob の built-in Orchestrator を project override して使う。`whenToUse` を各子モードに定義し、Bob の Orchestrator が stage ごとに切り替えやすい形にする。
- `ibmbob-intake` は run id 発行、入力 docx 正規化、workspace copy 作成、workspace profile 読み込み、build/test command の存在確認を担当する。コマンド未設定や実行不可はこの時点で `block` にする。
- `ibmbob-context-scout` は doc 調査子モードと code 調査子モードを並列で使い、既存仕様、共有構造、公開 API、状態遷移、RT/TS 責務分離、影響ファイル候補を `project_context_packet` にまとめる。
- `ibmbob-detail-design` は入力基本設計書と既存文書・既存コード調査結果を使って詳細設計を作る。C サンプル向け v1 では API、共有メモリ、状態、エラー、RT/TS 配置、変更点トレーサビリティを必須にする。
- `ibmbob-review-gate` は共通 reviewer とし、`stage` でレビュー観点を切り替える。`detail_design` は設計妥当性、`code_change` は差分妥当性、`unit_test_pack` は網羅性、`unit_test_run` は失敗原因、`functional_test_pack` は業務シナリオ妥当性を判定する。
- `ibmbob-code-author` は workspace copy だけを編集対象にし、詳細設計と context packet に基づいてコード変更を実施する。複数モジュールに跨る場合だけ module 単位の子タスクを許可し、merge 競合を避けるため write scope を事前固定する。
- `ibmbob-unit-test-author` は単体テストコードを生成し、`ibmbob-unit-test-executor` は workspace profile の `build_command` と `unit_test_command` を実行して結果を `unit_test_run_packet` に保存する。失敗時は `code` か `unit_test` に差し戻し、環境起因は `intake` に戻す。
- `ibmbob-functional-test-author` は機能テストを作るが、v1 では実行しない。シナリオ、前提条件、操作手順、期待結果、異常系を JSON+Markdown で残す。
- 実行成果物は `artifacts/ibm-bob/runs/<run-id>/`、編集用 workspace copy は `artifacts/ibm-bob/workspaces/<run-id>/project/` に固定する。原本の `ibm-bob/samples/base/` と `ibm-bob/samples/評価用基本設計書/` は read-only 扱いにする。
- Shell と IDE は同じ mode pack を共有する。IDE は mode selector から使い、Shell は prepared workspace を開いて同じ `.bob/` 設定を使う前提にする。

## Contracts
- `run-request.schema.json`: `project_root`, `input_basic_design_doc`, `workspace_profile_ref`, `run_policy`, `locale`, `eval_case_id` を必須にする。
- `workspace-profile.schema.json`: `doc_globs`, `source_globs`, `build_command`, `unit_test_command`, `command_timeout_sec`, `artifact_root` を必須にする。UT 実行はこの profile がないと開始しない。
- `project-context-packet.schema.json`: `existing_docs`, `existing_code_findings`, `impacted_modules`, `impact_risks`, `traceability`, `next_agent` を持たせる。
- `detail-design-packet.schema.json`: `design_summary`, `api_changes`, `shared_memory_changes`, `state_changes`, `threading_rules`, `error_rules`, `traceability`, `next_agent` を持たせる。
- `code-change-packet.schema.json`: `touched_files`, `change_summary`, `design_refs`, `risk_flags`, `review_targets`, `next_agent` を持たせる。
- `unit-test-pack.schema.json`: `test_files`, `test_cases`, `fixtures`, `coverage_focus`, `commands_ref`, `next_agent` を持たせる。
- `unit-test-run-packet.schema.json`: `build_command`, `test_command`, `exit_code`, `status`, `log_refs`, `failed_cases`, `reroute_target`, `next_agent` を持たせる。
- `functional-test-pack.schema.json`: `scenarios`, `preconditions`, `steps`, `expected_results`, `negative_cases`, `next_agent` を持たせる。
- `review-decision-packet.schema.json` は全 stage 共通で、`stage`, `decision`, `findings`, `revise_to`, `human_checkpoint`, `next_agent` を必須にする。
- すべての packet に対応する Markdown renderer を用意し、人向けレビューは Markdown、mode 間受け渡しは JSON を使う。

## Test Plan
- `.bob/custom_modes.yaml` と各 schema の静的検証を行い、YAML/JSON の parse と必須項目チェックを通す。
- routing test で `pass`, `revise`, `block` の遷移を固定確認する。特に `unit_test_run` の差し戻し先が `code`, `unit_test`, `intake` に正しく分かれることを確認する。
- preflight test で sample 原本が未変更のまま、workspace copy と `.bob/` 設定が別場所に作られることを確認する。
- evaluation test で 5 件の基本設計書すべてについて、`detail_design -> reviewed_code_change -> reviewed_unit_test_pack -> unit_test_run_packet -> reviewed_functional_test_pack -> run_summary` が揃うことを確認する。
- missing toolchain / command 未設定ケースでは、`ibmbob-intake` が早期に `block` を返し、コード変更後まで進まないことを確認する。
- IDE / Shell smoke test で同じ copied workspace の mode pack が両方で読み込めることを確認する。

## Assumptions
- v1 は日本語出力固定にする。
- 機能テストは作成までで止め、実行は対象外にする。
- 単体テスト実行は Bob が toolchain を自動導入しない。workspace profile に既存の build/test command が定義されていることを前提にする。
- Bob の project custom mode は `.bob/custom_modes.yaml` と `.bob/rules-{slug}/` を使う前提で実装する。根拠は IBM Bob 公式 docs の [Custom modes](https://bob.ibm.com/docs/ide/configuration/custom-modes)。
- 子モードの自動切り替えは `whenToUse` を使って Orchestrator に委譲する前提にする。根拠は IBM Bob 公式 docs の [Modes](https://bob.ibm.com/docs/ide/features/modes)。

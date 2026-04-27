# AGENTS.md - IBM-Bob JSONテンプレート評価専用モード

## 目的

このワークスペースは、JSONテンプレート式ドキュメント生成能力を評価するための専用環境である。
基本設計書を入力として、以下の流れだけを実行する。

1. 基本設計書レビュー
2. 詳細設計書作成
3. 詳細設計書レビュー
4. 機能テスト仕様書作成
5. 機能テスト仕様書レビュー

## 絶対禁止

- Cソースコードを作成、編集、提案しない。
- 単体テストコードを作成しない。
- ビルドやテストを実行しない。
- 実機やハードウェアを操作しない。
- JSONを作らずにDOCXだけを生成しない。
- DOCXをOOXML手書きで直接生成しない。

## 必須ルール

- 各ステージは必ずJSONを先に作成する。
- DOCXは `skills/json-docx/scripts/generate_docx_from_json.py` で生成する。
- DOCX生成後は `skills/json-docx/scripts/validate_docx_output.py` で検証する。
- 各JSONには、該当テンプレートの `SECTION_ID` と `JSON Path` に対応する情報を入れる。
- レビュー結果は必ず `review_result` JSONとして作り、レビュー用テンプレートからDOCX化する。
- 指摘は `severity`, `location`, `finding`, `risk`, `recommendation`, `evidence` を含める。

## ステージ出力

| Stage | Mode | JSON | DOCX |
|---|---|---|---|
| 01 | basic_design_review | `review_result.json` | `review_result.docx` |
| 02 | detail_design_author | `detail_design.json` | `detail_design.docx` |
| 03 | detail_design_review | `review_result.json` | `review_result.docx` |
| 04 | function_test_design_author | `function_test_design.json` | `function_test_design.docx` |
| 05 | function_test_review | `review_result.json` | `review_result.docx` |

## 品質ゲート

- JSONは構文エラーなし。
- DOCXに未置換の `{{...}}` または `[[TABLE:...]]` が残らない。
- 5段階以外の成果物を生成しない。
- レビューで `blocker` がある場合、次工程の生成は「仮生成」扱いとして未確定事項に明記する。

## 使うテンプレート

- 詳細設計書: `templates/detail_design_template_v2.docx`
- 機能テスト仕様書: `templates/function_test_design_template_v1.docx`
- レビュー結果: `templates/review_result_template_v1.docx`

基本設計書は入力文書であり、本環境では基本設計書の新規作成は行わない。

# IBM-Bob JSONテンプレート評価専用環境

この環境は、IBM-Bobの通常SDLCフローから **コード作成・単体テスト作成/実行・実装レビュー** を外し、JSONテンプレート式ドキュメント生成の評価だけを行うための最小構成です。

## 実行対象フロー

入力は「基本設計書」です。実行するのは以下の5段階だけです。

1. 基本設計書レビュー
2. 詳細設計書作成
3. 詳細設計書レビュー
4. 機能テスト仕様書（機能テスト設計書）作成
5. 機能テスト仕様書レビュー

以下は禁止です。

- コーディング
- コードレビュー
- 単体テストコード作成
- 単体テスト実行
- 実機操作
- ビルド実行

本環境では、AIはJSONを作成し、DOCX生成は `skills/json-docx/scripts/generate_docx_from_json.py` が担当します。AIがDOCXのXMLを直接編集してはいけません。

## ディレクトリ構成

```text
ibm_bob_json_template_eval_env/
  AGENTS.md
  routing/eval_only_flow.json
  prompts/
  rules/
  templates/
  schemas/
  checklists/
  skills/json-docx/
  scripts/
  samples/interlock/input/
```

## 最小手順

### 1. 入力からラン用ディレクトリを作成

```bash
python scripts/prepare_run.py \
  --run-id ILK001 \
  --basic-docx samples/interlock/input/運転開始前インターロック集約判定機能_基本設計書.docx \
  --basic-json samples/interlock/input/運転開始前インターロック集約判定機能.json
```

### 2. AIエージェントで各ステージを順に実行

生成された `runs/ILK001/prompts/` 配下のプロンプトを、順番にIBM-Bobへ渡します。

```text
01_basic_design_review.md
02_detail_design_author.md
03_detail_design_review.md
04_function_test_design_author.md
05_function_test_review.md
```

### 3. JSONからDOCXを生成

AIが各ステージのJSONを出力した後、以下のようにDOCXを生成します。

```bash
python scripts/render_stage_docx.py --run runs/ILK001 --stage 01_basic_design_review
python scripts/render_stage_docx.py --run runs/ILK001 --stage 02_detail_design_author
python scripts/render_stage_docx.py --run runs/ILK001 --stage 03_detail_design_review
python scripts/render_stage_docx.py --run runs/ILK001 --stage 04_function_test_design_author
python scripts/render_stage_docx.py --run runs/ILK001 --stage 05_function_test_review
```

### 4. 出力検証

```bash
python scripts/validate_eval_outputs.py --run runs/ILK001
```

## 出力成果物

```text
runs/<run-id>/outputs/
  01_basic_design_review/review_result.json
  01_basic_design_review/review_result.docx
  02_detail_design_author/detail_design.json
  02_detail_design_author/detail_design.docx
  03_detail_design_review/review_result.json
  03_detail_design_review/review_result.docx
  04_function_test_design_author/function_test_design.json
  04_function_test_design_author/function_test_design.docx
  05_function_test_review/review_result.json
  05_function_test_review/review_result.docx
```

## 評価観点

- JSON Schemaに従って構造化JSONを生成できるか
- テンプレートDOCXへ欠落なく差し込めるか
- 未置換プレースホルダが残らないか
- レビュー結果が人間レビュー前のプレレビューとして使える粒度か
- 基本設計→詳細設計→機能テスト仕様のトレーサビリティが維持されるか

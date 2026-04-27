# Stage 05: 機能テスト仕様書レビュー

# 共通制約
- これはJSONテンプレート式ドキュメント生成の評価である。
- コーディング、単体テストコード作成、ビルド実行、実機操作は行わない。
- 出力は指定JSONファイルのみを先に作る。DOCX生成は `scripts/render_stage_docx.py` に任せる。
- 不明点は未確定事項に記録し、処理を止めない。
- IBM-Bob基盤のRT/TS分離、共有メモリ、公開API、状態遷移、フェイルセーフ、VC2026/clang観点を必ず確認する。

## 入力
- `input/basic_design.docx`
- `outputs/02_detail_design_author/detail_design.json`
- `outputs/04_function_test_design_author/function_test_design.json`
- `outputs/04_function_test_design_author/function_test_design.docx`
- `checklists/function_test_design_pre_review_checklist.json`
- `schemas/review_result.schema.json`

## タスク
機能テスト仕様書をレビューし、レビュー結果JSONを作成する。

## 出力
- `outputs/05_function_test_review/review_result.json`

## レビュー観点
- 基本設計/詳細設計からのテスト網羅性
- 正常系、異常系、境界値、状態遷移、API、ログ/証跡
- RT/TS分離の確認観点
- 共有メモリ観測点の十分性
- build/test profileの両対応
- 未確定事項の分離

# Stage 04: 機能テスト仕様書作成

# 共通制約
- これはJSONテンプレート式ドキュメント生成の評価である。
- コーディング、単体テストコード作成、ビルド実行、実機操作は行わない。
- 出力は指定JSONファイルのみを先に作る。DOCX生成は `scripts/render_stage_docx.py` に任せる。
- 不明点は未確定事項に記録し、処理を止めない。
- IBM-Bob基盤のRT/TS分離、共有メモリ、公開API、状態遷移、フェイルセーフ、VC2026/clang観点を必ず確認する。

## 入力
- `input/basic_design.docx`
- `outputs/02_detail_design_author/detail_design.json`
- `outputs/03_detail_design_review/review_result.json`
- `schemas/function_test_design.schema.json`
- `schemas/function_test_design.skeleton.json`

## タスク
基本設計書、詳細設計書、詳細設計レビュー結果を元に、機能テスト仕様書JSONを作成する。

## 出力
- `outputs/04_function_test_design_author/function_test_design.json`

## 必須内容
- テスト方針
- テスト環境
- 入力/出力/観測点
- テストデータ
- テストケース一覧と詳細
- 要件/設計項目とのトレーサビリティ
- カバレッジマトリクス
- 証跡設計
- 開始/終了条件
- 未確定事項

# Stage 03: 詳細設計書レビュー

# 共通制約
- これはJSONテンプレート式ドキュメント生成の評価である。
- コーディング、単体テストコード作成、ビルド実行、実機操作は行わない。
- 出力は指定JSONファイルのみを先に作る。DOCX生成は `scripts/render_stage_docx.py` に任せる。
- 不明点は未確定事項に記録し、処理を止めない。
- IBM-Bob基盤のRT/TS分離、共有メモリ、公開API、状態遷移、フェイルセーフ、VC2026/clang観点を必ず確認する。

## 入力
- `input/basic_design.docx`
- `outputs/02_detail_design_author/detail_design.json`
- `outputs/02_detail_design_author/detail_design.docx`
- `checklists/detail_design_pre_review_checklist.json`
- `schemas/review_result.schema.json`

## タスク
詳細設計書をレビューし、レビュー結果JSONを作成する。

## 出力
- `outputs/03_detail_design_review/review_result.json`

## レビュー観点
- 基本設計との整合性
- 関数責務と副作用
- 共有メモリ整合性、世代番号、busy/state更新順序
- RT禁止処理が混入していないか
- APIの参照専用/書込有無
- フェイルセーフ、タイムアウト、復帰条件
- テスト可能性

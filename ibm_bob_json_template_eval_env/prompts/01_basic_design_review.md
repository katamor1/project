# Stage 01: 基本設計書レビュー

# 共通制約
- これはJSONテンプレート式ドキュメント生成の評価である。
- コーディング、単体テストコード作成、ビルド実行、実機操作は行わない。
- 出力は指定JSONファイルのみを先に作る。DOCX生成は `scripts/render_stage_docx.py` に任せる。
- 不明点は未確定事項に記録し、処理を止めない。
- IBM-Bob基盤のRT/TS分離、共有メモリ、公開API、状態遷移、フェイルセーフ、VC2026/clang観点を必ず確認する。

## 入力
- `input/basic_design.docx`
- `input/basic_design.json` があれば補助情報として使用
- `checklists/basic_design_pre_review_checklist.json`
- `schemas/review_result.schema.json`

## タスク
基本設計書をレビューし、レビュー結果JSONを作成する。

## 出力
- `outputs/01_basic_design_review/review_result.json`

## レビュー観点
- 目的・対象範囲・要件の明確性
- RT/TS責務分離
- 共有メモリ/グローバル変数の所有者、読取元、書込元
- 公開APIの必要性、引数、戻り値、エラー
- 状態遷移・シーケンス・復帰条件
- フェイルセーフと安全側動作
- 影響調査対象の十分性
- 詳細設計へ引き継ぐ未確定事項

## 判定
`overall_decision` は `pass`, `conditional_pass`, `revise`, `block` のいずれか。

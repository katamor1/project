# Stage 02: 詳細設計書作成

# 共通制約
- これはJSONテンプレート式ドキュメント生成の評価である。
- コーディング、単体テストコード作成、ビルド実行、実機操作は行わない。
- 出力は指定JSONファイルのみを先に作る。DOCX生成は `scripts/render_stage_docx.py` に任せる。
- 不明点は未確定事項に記録し、処理を止めない。
- IBM-Bob基盤のRT/TS分離、共有メモリ、公開API、状態遷移、フェイルセーフ、VC2026/clang観点を必ず確認する。

## 入力
- `input/basic_design.docx`
- `input/basic_design.json` があれば補助情報として使用
- `outputs/01_basic_design_review/review_result.json`
- `schemas/detail_design.schema.json`
- `schemas/detail_design.skeleton.json`

## タスク
基本設計書と基本設計レビュー指摘を元に、詳細設計書JSONを作成する。

## 出力
- `outputs/02_detail_design_author/detail_design.json`

## 必須内容
- トレーサビリティ
- 影響ファイル/既存仕様確認対象
- モジュール構成
- 共有メモリ/グローバル変数設計
- RT/TS/APIスレッド設計
- 状態遷移
- 関数詳細仕様と擬似コード
- API詳細
- エラー/安全設計
- 性能見積と測定計画
- build/test commandの参照
- 未確定事項

<!-- ibm-bob/mode-pack/rules/ibmbob-sdlc-detail-design-author/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob SDLC Detail Design Author. -->
<!-- This exists so IBM-Bob can generate detail design JSON and DOCX from a reviewed basic design. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/structured-artifact-workflow.md, ibm-bob/mode-pack/structured-templates/detail-design/**, ibm-bob/mode-pack/routing/direct-mode-flow.json -->
# IBM-Bob SDLC Detail Design Author Rules

## Read First
- `.copilot/prompts/sdlc/ibmbob-sdlc-detail-design-author.md`
- `ibm-bob/mode-pack/structured-artifact-workflow.md`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/ibm_bob_detail_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/IBM-Bob_詳細設計書テンプレート_v2.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/ibm_bob_detail_design_pre_review_checklist.json`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `docs/sdlc/README.md`

## Responsibilities
- 対応する SDLC artifact 1 つだけを作る。
- `artifact_kind = detail_design` として扱う。
- reviewed basic design、影響調査、既存仕様/コード証跡から、実装者が迷わない詳細設計へ具体化する。
- まず中間JSONを作成し、`ibm_bob_detail_design_schema.json` に適合させる。
- 次に `IBM-Bob_詳細設計書テンプレート_v2.docx` の `{{json.path}}` と `SECTION_ID` に沿ってDOCXを作る。
- 直前 lane の packet を壊さず、next_agent を 1 つだけ返す。

## Required Outputs
- `artifacts/ibm-bob/<run_id>/detail_design/<feature_id>_detail_design.json`
- `artifacts/ibm-bob/<run_id>/detail_design/<feature_id>_detail_design.docx`
- handoff packet including `artifact_kind`, `source_basic_design_ref`, `schema_ref`, `template_ref`, `checklist_ref`, `json_ref`, `docx_ref`, `source_code_impact_refs`, `open_issues`, `next_agent`

## IBM-Bob Domain Coverage
- モジュール/関数一覧、入力/処理/出力、副作用、共有メモリ/グローバル変数、API、RT/TS処理配置、排他/整合性、状態遷移、シーケンス、タイムアウト/リトライ、エラー安全、性能予算、単体/機能テスト引き渡しを明記する。
- RT側では Windows API、ファイルI/O、動的メモリ確保、長時間ブロッキングを禁止する。
- TS側に寄せる処理はファイルI/O、ログ整形、UI連携、重計算、保存処理に限定して責務分離を明確にする。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。
- JSONとDOCXが不一致の場合は、DOCXを再生成するか、未解決事項として明記してから reviewer に渡す。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。

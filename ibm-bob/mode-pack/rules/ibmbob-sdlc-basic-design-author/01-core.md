<!-- ibm-bob/mode-pack/rules/ibmbob-sdlc-basic-design-author/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob SDLC Basic Design Author. -->
<!-- This exists so IBM-Bob can generate basic design JSON and DOCX from the approved scope and evidence. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/structured-artifact-workflow.md, ibm-bob/mode-pack/structured-templates/basic-design/**, ibm-bob/mode-pack/routing/direct-mode-flow.json -->
# IBM-Bob SDLC Basic Design Author Rules

## Read First
- `.copilot/prompts/sdlc/ibmbob-sdlc-basic-design-author.md`
- `ibm-bob/mode-pack/structured-artifact-workflow.md`
- `.bob/ibm-bob/references/sdlc/structured-templates/basic-design/ibm_bob_basic_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/basic-design/IBM-Bob_基本設計書テンプレート_v2.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/basic-design/ibm_bob_pre_review_checklist.json`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `docs/sdlc/README.md`

## Responsibilities
- 対応する SDLC artifact 1 つだけを作る。
- `artifact_kind = basic_design` として扱う。
- 要件定義、scope packet、artifact_context_packet、既存仕様/コード証跡から基本設計書ドラフトを作る。
- まず中間JSONを作成し、`ibm_bob_basic_design_schema.json` に適合させる。
- 次に `IBM-Bob_基本設計書テンプレート_v2.docx` の `{{json.path}}` と `SECTION_ID` に沿ってDOCXを作る。
- source-backed work では artifact_context_packet を優先し、足りなければ止まる。
- 直前 lane の packet を壊さず、next_agent を 1 つだけ返す。

## Required Outputs
- `artifacts/ibm-bob/<run_id>/basic_design/<feature_id>_basic_design.json`
- `artifacts/ibm-bob/<run_id>/basic_design/<feature_id>_basic_design.docx`
- handoff packet including `artifact_kind`, `schema_ref`, `template_ref`, `checklist_ref`, `json_ref`, `docx_ref`, `evidence_refs`, `open_issues`, `next_agent`

## IBM-Bob Domain Coverage
- RT/TS分離、共有メモリ、グローバル変数、公開API、状態遷移、シーケンス、フェイルセーフ、ログ/診断、VC2026/clang build-test 観点を必要に応じて章へ反映する。
- 基本設計レベルに留め、関数内部の詳細処理や擬似コードは詳細設計へ委譲する。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。
- JSONとDOCXが不一致の場合は、DOCXを再生成するか、未解決事項として明記してから reviewer に渡す。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。
- source-backed generation は artifact_context_packet が無い状態で始めない。

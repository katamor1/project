<!-- ibm-bob/mode-pack/rules/ibmbob-sdlc-functional-spec-author/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob SDLC Functional Spec Author. -->
<!-- This exists so IBM-Bob can generate function test design JSON and DOCX from reviewed design artifacts. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/structured-artifact-workflow.md, ibm-bob/mode-pack/structured-templates/function-test-design/**, ibm-bob/mode-pack/routing/direct-mode-flow.json -->
# IBM-Bob SDLC Functional Spec Author Rules

## Read First
- `.copilot/prompts/sdlc/ibmbob-sdlc-functional-spec-author.md`
- `ibm-bob/mode-pack/structured-artifact-workflow.md`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/ibm_bob_function_test_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/IBM-Bob_機能テスト設計書テンプレート_v1.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/ibm_bob_function_test_pre_review_checklist.json`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `docs/sdlc/README.md`

## Responsibilities
- 対応する SDLC artifact 1 つだけを作る。
- IBM-Bobでは `functional_spec` を `artifact_kind = function_test_design` として扱う。
- reviewed basic design と reviewed detail design から、機能テスト設計書を作る。
- まず中間JSONを作成し、`ibm_bob_function_test_design_schema.json` に適合させる。
- 次に `IBM-Bob_機能テスト設計書テンプレート_v1.docx` の `{{json.path}}` と `SECTION_ID` に沿ってDOCXを作る。
- 直前 lane の packet を壊さず、next_agent を 1 つだけ返す。

## Required Outputs
- `artifacts/ibm-bob/<run_id>/function_test_design/<feature_id>_function_test_design.json`
- `artifacts/ibm-bob/<run_id>/function_test_design/<feature_id>_function_test_design.docx`
- handoff packet including `artifact_kind`, `source_basic_design_ref`, `source_detail_design_ref`, `schema_ref`, `template_ref`, `checklist_ref`, `json_ref`, `docx_ref`, `coverage_matrix`, `open_issues`, `next_agent`

## IBM-Bob Domain Coverage
- 正常系、異常系、境界値、状態遷移、シーケンス、共有メモリ、公開API、RT周期性能、フェイルセーフ、ログ/証跡、回帰、VC2026/clang build-testをカバーする。
- 機能テスト設計であり、テストコード実装や実行までは行わない。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。
- JSONとDOCXが不一致の場合は、DOCXを再生成するか、未解決事項として明記してから reviewer に渡す。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。

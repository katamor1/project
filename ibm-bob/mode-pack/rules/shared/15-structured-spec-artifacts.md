<!-- ibm-bob/mode-pack/rules/shared/15-structured-spec-artifacts.md -->
<!-- Shared structured artifact rule for IBM-Bob direct SDLC modes. -->
<!-- This exists so design/test authoring and review use the same DOCX templates and intermediate JSON files. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/structured-artifact-workflow.md, ibm-bob/mode-pack/structured-templates/**, ibm-bob/mode-pack/scripts/structured_artifact_tools.py -->
# Structured Specification Artifact Rule

## Required Source Of Truth

For `basic_design`, `detail_design`, and `function_test_design`, produce and preserve both:

- intermediate JSON conforming to the kind-specific schema
- human-reviewable DOCX generated from the kind-specific Word template

The JSON is the machine-readable source for AI handoff and automated review. The DOCX is the human review and approval artifact.

## Installed Template Paths

Use these installed paths when running inside a workspace:

- `.bob/ibm-bob/references/sdlc/structured-templates/basic-design/IBM-Bob_基本設計書テンプレート_v2.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/basic-design/ibm_bob_basic_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/basic-design/ibm_bob_pre_review_checklist.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/IBM-Bob_詳細設計書テンプレート_v2.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/ibm_bob_detail_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/ibm_bob_detail_design_pre_review_checklist.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/IBM-Bob_機能テスト設計書テンプレート_v1.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/ibm_bob_function_test_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/ibm_bob_function_test_pre_review_checklist.json`

## Authoring Discipline

1. Create the JSON first.
2. Validate it against the schema.
3. Fill or render the DOCX template from JSON.
4. Keep JSON and DOCX in the same artifact folder.
5. Include `artifact_kind`, JSON path, DOCX path, schema path, template path, checklist path, and source evidence in handoff.

## Reviewer Discipline

1. Review JSON schema conformance.
2. Review DOCX section coverage by `SECTION_ID` and `JSON Path`.
3. Use the kind-specific checklist.
4. Output review JSON and a human-readable review artifact.
5. Use `pass`, `revise`, or `block`; do not silently approve JSON/DOCX disagreement.

## IBM-Bob Control-System Checks

Always check RT/TS separation, shared-memory/global-variable ownership, API header compatibility, state/sequence timeout behavior, fail-safe outputs, evidence logging, and `ibm-bob-vc2026` / `ibm-bob-clang-win` build/test references.

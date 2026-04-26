<!-- ibm-bob/mode-pack/structured-artifact-workflow.md -->
<!-- Describes how IBM-Bob author/reviewer modes use Word templates plus intermediate JSON. -->
<!-- This exists so basic design, detail design, function test design, and reviews can be automated without losing human-reviewable DOCX artifacts. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/structured-templates/**, ibm-bob/mode-pack/scripts/structured_artifact_tools.py, ibm-bob/mode-pack/rules/ibmbob-sdlc-spec-reviewer/01-core.md -->
# IBM-Bob Structured Artifact Workflow

## Purpose

IBM-Bob の仕様書作成自動化では、設計書の正本を次の二重構造で扱う。

1. **中間JSON**: AIエージェント間の引き渡し、機械レビュー、差分比較、再生成の正本。
2. **Word DOCX**: 人間レビュー、承認、配布の正本。

基本設計書、詳細設計書、機能テスト設計書、および各プレレビューは、JSONとDOCXを必ずペアで作成する。

## Artifact Types

| artifact_kind | JSON Schema | DOCX Template | Review Checklist |
|---|---|---|---|
| `basic_design` | `structured-templates/basic-design/ibm_bob_basic_design_schema.json` | `structured-templates/basic-design/IBM-Bob_基本設計書テンプレート_v2.docx` | `structured-templates/basic-design/ibm_bob_pre_review_checklist.json` |
| `detail_design` | `structured-templates/detail-design/ibm_bob_detail_design_schema.json` | `structured-templates/detail-design/IBM-Bob_詳細設計書テンプレート_v2.docx` | `structured-templates/detail-design/ibm_bob_detail_design_pre_review_checklist.json` |
| `function_test_design` | `structured-templates/function-test-design/ibm_bob_function_test_design_schema.json` | `structured-templates/function-test-design/IBM-Bob_機能テスト設計書テンプレート_v1.docx` | `structured-templates/function-test-design/ibm_bob_function_test_pre_review_checklist.json` |

Installed workspace paths are under `.bob/ibm-bob/references/sdlc/structured-templates/`.

## Authoring Contract

Author modes must follow this order.

1. Read upstream artifacts and evidence.
2. Create or update the intermediate JSON first.
3. Validate the JSON against the matching schema.
4. Render or fill the DOCX template using the JSON paths in the template.
5. Save both files in the same artifact directory.
6. Hand off JSON path, DOCX path, source evidence, and open issues to the reviewer.

Recommended artifact paths:

```text
artifacts/ibm-bob/<run_id>/<artifact_kind>/<feature_id>_<artifact_kind>.json
artifacts/ibm-bob/<run_id>/<artifact_kind>/<feature_id>_<artifact_kind>.docx
artifacts/ibm-bob/<run_id>/<artifact_kind>/<feature_id>_<artifact_kind>_pre_review.json
artifacts/ibm-bob/<run_id>/<artifact_kind>/<feature_id>_<artifact_kind>_pre_review.docx
```

## Reviewer Contract

Reviewer modes must review the JSON and DOCX as one pair.

1. Validate JSON schema.
2. Confirm required DOCX sections exist by `SECTION_ID`.
3. Confirm JSON paths in DOCX are filled or intentionally marked 未定/TBD.
4. Run the matching checklist and record each item as `OK`, `WARN`, `NG`, or `N/A`.
5. Return `pass`, `revise`, or `block` with a single next agent.

Review JSON must include:

- `artifact_kind`
- `review_target_json`
- `review_target_docx`
- `checklist_id`
- `decision`
- `findings[]`
- `open_issues[]`
- `next_agent`

## Design Rules For IBM-Bob C/Windows Control System

All artifact types must preserve these domain rules.

- RT thread must not perform Windows system calls, file I/O, dynamic memory allocation, or long blocking operations.
- TS thread handles file I/O, logs, UI/app integration, and heavy pre-computation.
- Shared memory/global variables must state owner, writer, reader, update timing, and consistency rule.
- API additions must state public header impact, argument validation, return codes, and RT/TS responsibility split.
- State transition and sequential handshakes must include timeout, retry, abort, and fail-safe behavior.
- Build/test references must distinguish `ibm-bob-vc2026` and `ibm-bob-clang-win` workspace profiles.

## Tooling

`python ibm-bob/mode-pack/scripts/structured_artifact_tools.py list`

Shows registered templates.

`python ibm-bob/mode-pack/scripts/structured_artifact_tools.py validate --kind basic_design --json path/to/basic.json`

Validates JSON against the kind-specific schema.

`python ibm-bob/mode-pack/scripts/structured_artifact_tools.py render-docx --kind basic_design --json path/to/basic.json --out out.docx`

Copies the kind-specific DOCX template and performs best-effort scalar placeholder replacement.

`python ibm-bob/mode-pack/scripts/structured_artifact_tools.py extract-sections --kind basic_design --docx in.docx --out extracted.json`

Extracts a section-oriented JSON snapshot from a DOCX for review and migration use.

## Human Review Policy

If the DOCX cannot be generated or validated, do not silently continue with JSON only. Mark the artifact as `block` or request a human checkpoint.

If JSON and DOCX disagree, JSON is treated as the machine-readable source for regeneration, but the disagreement must be reported as a review finding before approval.

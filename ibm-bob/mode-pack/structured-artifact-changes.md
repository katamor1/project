<!-- ibm-bob/mode-pack/structured-artifact-changes.md -->
<!-- Summarizes the changes made to enable IBM-Bob Word templates and intermediate JSON artifacts. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/structured-templates/**, ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/modes/custom_modes.source.yaml -->
# Structured Artifact Changes

## Added

- `structured-templates/basic-design/` with Word template, schema, skeleton JSON, and checklist.
- `structured-templates/detail-design/` with Word template, schema, skeleton JSON, and checklist.
- `structured-templates/function-test-design/` with Word template, schema, skeleton JSON, and checklist.
- `structured-artifact-workflow.md` describing the JSON/DOCX author/reviewer contract.
- `scripts/structured_artifact_tools.py` for list, new, validate, render-docx, and extract-sections operations.
- Schema aliases under `schemas/ibm-bob-*-document.schema.json`.

## Modified

- `scripts/install_mode_pack.py` now installs structured templates into `.bob/ibm-bob/references/sdlc/structured-templates/`.
- Direct SDLC author/reviewer rules now require JSON-first authoring and DOCX pair output.
- Old-family detail design, functional test, and review gate rules now understand structured JSON/DOCX artifacts.
- `modes/custom_modes.source.yaml` edit permissions now allow `.docx` artifacts where needed.

## Artifact Contract

Each approved design/test artifact should have:

```text
<feature_id>_<artifact_kind>.json
<feature_id>_<artifact_kind>.docx
<feature_id>_<artifact_kind>_pre_review.json
<feature_id>_<artifact_kind>_pre_review.docx or .md
```

The JSON is the machine-readable source. The DOCX is the human-review artifact.

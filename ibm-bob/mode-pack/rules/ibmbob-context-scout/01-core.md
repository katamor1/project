<!-- ibm-bob/mode-pack/rules/ibmbob-context-scout/01-core.md -->
<!-- Defines the context scouting rules for existing documents, code, and impact mapping. -->
<!-- This exists so detailed design starts from evidence about the copied project instead of assumptions. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/project-context-packet.schema.json, ibm-bob/mode-pack/rules/ibmbob-detail-design/01-core.md, ibm-bob/mode-pack/rules/ibmbob-code-author/01-core.md -->
# IBM-Bob Context Scout Rules

## Scope
- 既存 docs を読む。
- 既存 source を読む。
- 共有構造、公開 API、状態遷移、RT/TS 分離、影響候補モジュールを整理する。

## Output
- `project-context-packet` を作る。
- 同じ内容の Markdown も残す。

## Required Coverage
- `existing_docs`
- `existing_code_findings`
- `impacted_modules`
- `impact_risks`
- `traceability`

## Safety
- ここでは source code を編集しない。
- evidence が弱い時は `impact_risks` に残す。

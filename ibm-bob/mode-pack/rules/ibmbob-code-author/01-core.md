<!-- ibm-bob/mode-pack/rules/ibmbob-code-author/01-core.md -->
<!-- Defines the code authoring rules for editing only the copied workspace and recording the touched files. -->
<!-- This exists so IBM-Bob can implement approved changes without drifting outside the intended write scope. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/code-change-packet.schema.json, ibm-bob/mode-pack/rules/ibmbob-unit-test-author/01-core.md, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Code Author Rules

## Scope
- copied workspace の `source/` だけを編集する。
- run artifact に `code-change-packet` も残す。

## Required Output
- 実際に触ったファイルを `touched_files` に書く。
- `change_summary`, `design_refs`, `risk_flags`, `review_targets` を埋める。

## Multi-Module Rule
- 複数モジュールに跨る場合は、先に write scope を固定する。
- merge 競合を避けるため、1 回の作業で触るファイル集合を明示する。

## Safety
- 原本や別 run の workspace を編集しない。
- build/test command は変更しない。

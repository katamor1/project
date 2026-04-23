<!-- ibm-bob/mode-pack/rules/ibmbob-unit-test-author/01-core.md -->
<!-- Defines the unit-test authoring rules for the copied workspace and unit-test packet. -->
<!-- This exists so IBM-Bob records concrete test intent before executing the workspace profile commands. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/unit-test-pack.schema.json, ibm-bob/mode-pack/rules/ibmbob-unit-test-executor/01-core.md, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Unit Test Author Rules

## Scope
- copied workspace の test code を追加または更新する。
- `unit-test-pack` を更新する。

## Required Output
- `test_files`
- `test_cases`
- `fixtures`
- `coverage_focus`
- `commands_ref`

## Safety
- build/test command は workspace profile の値を参照するだけにする。
- command 自体は書き換えない。

<!-- ibm-bob/mode-pack/rules/ibmbob-unit-test-executor/01-core.md -->
<!-- Defines the exact command execution rules for IBM-Bob unit-test execution. -->
<!-- This exists so execution logs, exit codes, and reroute decisions are recorded consistently. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/unit-test-run-packet.schema.json, ibm-bob/mode-pack/rules/ibmbob-intake/01-core.md, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Unit Test Executor Rules

## Command Policy
- `build_command` と `unit_test_command` をそのまま使う。
- 代替 command は使わない。
- real 実行では stdout, stderr, exit code を必ず保存する。

## Reroute Classification
- toolchain 不足や command 不備は `intake`
- 実装不整合は `code`
- test expectation や fixture 問題は `unit_test`

## Output
- `unit-test-run-packet`
- 実行ログ
- 同内容の Markdown

## Safety
- 機能テストはここで実行しない。

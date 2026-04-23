<!-- ibm-bob/mode-pack/rules/ibmbob-intake/01-core.md -->
<!-- Defines the intake rules for copied workspace preparation and command readiness checks. -->
<!-- This exists so IBM-Bob blocks early when workspace setup or command readiness is not good enough. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/run-request.schema.json, ibm-bob/mode-pack/schemas/workspace-profile.schema.json, ibm-bob/mode-pack/rules/orchestrator/01-core.md -->
# IBM-Bob Intake Rules

## Read First
- `.bob/ibm-bob/current_run.json`
- `.bob/ibm-bob/workspace_profile.json`
- `source/AGENTS.md`

## Responsibilities
- copied workspace の位置を確認する。
- 入力基本設計書の位置を確認する。
- workspace profile の `build_command` と `unit_test_command` を確認する。
- `execution_mode = real` の時だけ command 実行可否を確認する。

## Early Block Conditions
- `workspace_profile_ref` がない。
- `build_command` または `unit_test_command` が空。
- command はあるが toolchain 不足で実行不能。
- project copy や input doc の参照先が欠けている。

## Packet Rules
- 正常時は `run-request` を維持し、次は `ibmbob-context-scout` に進める。
- block 時は `review-decision-packet` を stage `intake` で作る。
- `human_checkpoint = true` は block 時だけ使う。

## Safety
- command は workspace profile の値をそのまま使う。
- 代替 build/test command を自分で作らない。

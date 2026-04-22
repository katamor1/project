<!-- docs/claude-code/legacy-search/pilot-runbook.md -->
<!-- This file defines the pilot runbook for the Claude Code legacy search workflow pack v2. -->
<!-- This file exists to help the pilot team run tests, record findings, and decide go or no-go. -->
<!-- RELEVANT FILES: docs/claude-code/legacy-search/readiness/pilot-scorecard.yaml, docs/claude-code/legacy-search/readiness/triage-playbook.md, docs/claude-code/legacy-search/readiness/go-no-go-checklist.md -->
# Pilot Runbook

## Roles

- `Workflow owner`: prompt 管理、knowledge root 接続、pilot 公開を担当する
- `Knowledge owner`: SharePoint / OneDrive の範囲、名称、説明文、権限を管理する
- `Pilot user`: `LS-01 Intake Router` 起点で利用し、結果を記録する

## Pilot Rules

- Pilot は 5 人以内で始めます。
- 各人 5 問で始めます。
- 新規会話は必ず `LS-01 Intake Router` から始めます。
- 各ステップで `handoff_packet` を次 agent に貼り替えます。
- 結果は `readiness/pilot-scorecard.yaml` へ転記します。

## Execution Order

1. `LS-01 Intake Router` で開始する
2. `handoff_packet` を次 agent に貼る
3. 最終回答または停止回答まで進める
4. `chat_response` と `pm_copy_template` を確認する
5. `pilot-scorecard.yaml` へ転記する
6. `triage-playbook.md` で問題種別を判定する
7. `go-no-go-checklist.md` に最終判断を残す

## Go / No-Go Rule

以下を満たした時だけ `Pilot team` 公開へ進みます。

- `誤推測 0`
- `競合見逃し 0`
- `handoff_packet` 欠落 0
- `pm_copy_template` が代表 10 ケースでそのまま転記可能

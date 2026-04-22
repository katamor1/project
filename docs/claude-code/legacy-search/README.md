<!-- docs/claude-code/legacy-search/README.md -->
<!-- This file indexes the v2 Claude Code workflow pack for legacy asset search. -->
<!-- This file exists to help the team operate the pack with explicit routing and handoff packets. -->
<!-- RELEVANT FILES: docs/claude-code/legacy-search/shared-io-contract.yaml, docs/claude-code/legacy-search/implementation-guide.md, docs/claude-code/legacy-search/readiness/knowledge-onboarding.md -->
# Claude Code Legacy Search Workflow Pack

このフォルダは、現行サービスの旧資産検索に特化した `Claude Code` 向け Workflow Pack の `v2` です。

主利用者は `PM / 設計者` です。

主なナレッジソースは `1 系統の SharePoint / OneDrive` です。

回答方針は `根拠付き`, `日本語`, `根拠不足時は abstain` です。

## Contents

- `shared-io-contract.yaml`: 全エージェントで共有する v2 I/O 契約
- `routing-matrix.md`: 質問タイプごとの推奨ルートと運用ルール
- `implementation-guide.md`: Claude Code 上の実行手順と公開条件
- `pilot-runbook.md`: Pilot team 向けの実施手順
- `readiness/`: Pilot readiness 用の台帳と判断資料
- `agents/`: 8 エージェント分のコピー用 prompt
- `tests/evaluation-set.yaml`: 20 問の受入ケースと 5 問の運用確認

## Operational Agent Names

1. `LS-01 Intake Router`
2. `LS-02 Glossary Normalizer`
3. `LS-03 Document Locator`
4. `LS-04 Evidence Verifier`
5. `LS-05 Grounded Answerer`
6. `LS-06 Gap Reporter`
7. `LS-07 Renewal Impact Mapper`
8. `LS-08 Change Diff Scout`

## Recommended Setup

- 1 ワークフロー 1 役割で分けます。
- 全ステップで同じ `allowed knowledge roots` を共有します。
- hidden handoff には依存しません。
- 新規会話は必ず `LS-01 Intake Router` から始めます。
- 各エージェントは必ず `next_agent` と `handoff_packet` を返します。
- 人が `handoff_packet` を次のステップへ貼り替えて進めます。
- 最終系エージェントは `chat_response` と `pm_copy_template` を返します。

## Operating Rules

- `Pilot team` 限定で始めます。
- コードリポジトリや外部 Web は v1 では使いません。
- 一般知識で補完しません。
- `allowed knowledge roots` の外は検索しません。
- 競合する資料がある時は、強い資料を優先しつつ競合を明示します。
- 資料が広すぎる時は、まず検索対象を狭めます。
- 回答は短くします。
- `LS-02` 以降を単独で使った場合は、通常処理せず `LS-01 Intake Router` に戻します。

## Basic Flow

通常の現行仕様確認は次の流れです。

`LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-05 Grounded Answerer`

刷新観点の整理は次の流れです。

`LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-07 Renewal Impact Mapper`

新要求との差分確認は次の流れです。

`LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-08 Change Diff Scout`

根拠不足や対象外は次の流れで止めます。

`LS-01 Intake Router -> LS-06 Gap Reporter`

または

`LS-03 Document Locator / LS-04 Evidence Verifier -> LS-06 Gap Reporter`

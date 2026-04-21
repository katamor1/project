<!-- docs/copilot-studio/legacy-search/routing-matrix.md -->
<!-- This file defines the explicit v2 routing matrix for the legacy search multi-agent pack. -->
<!-- This file exists so the team can operate the pack with Router-first entry and handoff packets. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/shared-io-contract.yaml, docs/copilot-studio/legacy-search/README.md, docs/copilot-studio/legacy-search/pilot-runbook.md -->
# Routing Matrix

## Default Routes

| Intent | Route | Final Agent | Stop Rule |
| --- | --- | --- | --- |
| 現行仕様を知りたい | LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-05 Grounded Answerer | `LS-05 Grounded Answerer` | 根拠が弱ければ `LS-06 Gap Reporter` |
| 用語が曖昧な質問 | LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-05 Grounded Answerer | `LS-05 Grounded Answerer` | 正規化できなければ `LS-06 Gap Reporter` |
| どの資料を見ればよいか知りたい | LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier | `LS-04 Evidence Verifier` | 有力資料がなければ `LS-06 Gap Reporter` |
| 資料同士が食い違うか確認したい | LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-06 Gap Reporter | `LS-06 Gap Reporter` | 競合が見つかった時点で停止 |
| 刷新時の見直し観点を知りたい | LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-07 Renewal Impact Mapper | `LS-07 Renewal Impact Mapper` | 根拠不足なら `LS-06 Gap Reporter` |
| 新要求との差分を知りたい | LS-01 Intake Router -> LS-02 Glossary Normalizer -> LS-03 Document Locator -> LS-04 Evidence Verifier -> LS-08 Change Diff Scout | `LS-08 Change Diff Scout` | 現行根拠不足なら `LS-06 Gap Reporter` |
| 質問が広すぎる | LS-01 Intake Router -> LS-06 Gap Reporter | `LS-06 Gap Reporter` | 検索を開始しない |
| SharePoint / OneDrive 外の質問 | LS-01 Intake Router -> LS-06 Gap Reporter | `LS-06 Gap Reporter` | 対象外として停止 |

## Agent Responsibilities

| Agent | Main Job | Should Not Do |
| --- | --- | --- |
| `LS-01 Intake Router` | 意図分類と次の 1 手の決定 | 資料本文の要約回答 |
| `LS-02 Glossary Normalizer` | 用語揺れの正規化 | 根拠付き最終回答 |
| `LS-03 Document Locator` | 候補資料の抽出 | 候補資料の真偽判定 |
| `LS-04 Evidence Verifier` | 承認状態、鮮度、競合の確認 | 検証前提のない推測回答 |
| `LS-05 Grounded Answerer` | 検証済み根拠から簡潔回答 | 新しい検索 |
| `LS-06 Gap Reporter` | 不足資料と不足条件の明示 | 推測で空欄を埋める |
| `LS-07 Renewal Impact Mapper` | 刷新観点の整理 | 差分判定の断定 |
| `LS-08 Change Diff Scout` | 踏襲 / 変更 / 不明 の差分整理 | 根拠なしの変更提案 |

## Operating Rules

- 新規会話は `LS-01 Intake Router` だけから始めます。
- `LS-02` 以降を直接開いた場合は通常処理をせず、`LS-01 Intake Router` へ戻します。
- 各エージェントは必ず `handoff_packet` を返します。
- 人が `handoff_packet` を次のエージェントへ貼り替えて進めます。
- `complete` を既定で返してよいのは `LS-05`, `LS-07`, `LS-08` だけです。
- `human_follow_up` を既定で返してよいのは `LS-06` だけです。

## Fallback Rules

- `LS-03 Document Locator` が有力候補を出せない時は `LS-06 Gap Reporter`
- `LS-04 Evidence Verifier` が競合を検知した時は `LS-06 Gap Reporter`
- `LS-05 Grounded Answerer` は `LS-04 Evidence Verifier` を通っていない資料を使わない
- `LS-07 Renewal Impact Mapper` と `LS-08 Change Diff Scout` は `verified evidence` がない時は回答しない
- `complete` は最終回答がユーザーへ返る時だけ使う

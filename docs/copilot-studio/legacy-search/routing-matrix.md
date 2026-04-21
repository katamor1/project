<!-- docs/copilot-studio/legacy-search/routing-matrix.md -->
<!-- This file defines the explicit routing matrix for the legacy search multi-agent pack. -->
<!-- This file exists so the team can operate the pack without hidden handoff logic. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/shared-io-contract.yaml, docs/copilot-studio/legacy-search/agents/intake-router.md, docs/copilot-studio/legacy-search/tests/evaluation-set.yaml -->
# Routing Matrix

## Default Routes

| Intent | Route | Final Agent | Stop Rule |
| --- | --- | --- | --- |
| 現行仕様を知りたい | Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Grounded Answerer | `Grounded Answerer` | 根拠が弱ければ `Gap Reporter` |
| 用語が曖昧な質問 | Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Grounded Answerer | `Grounded Answerer` | 正規化できなければ `Gap Reporter` |
| どの資料を見ればよいか知りたい | Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier | `Evidence Verifier` | 有力資料がなければ `Gap Reporter` |
| 資料同士が食い違うか確認したい | Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Gap Reporter | `Gap Reporter` | 競合が見つかった時点で停止 |
| 刷新時の見直し観点を知りたい | Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Renewal Impact Mapper | `Renewal Impact Mapper` | 根拠不足なら `Gap Reporter` |
| 新要求との差分を知りたい | Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Change Diff Scout | `Change Diff Scout` | 現行根拠不足なら `Gap Reporter` |
| 質問が広すぎる | Intake Router -> Gap Reporter | `Gap Reporter` | 検索を開始しない |
| SharePoint / OneDrive 外の質問 | Intake Router -> Gap Reporter | `Gap Reporter` | 対象外として停止 |

## Agent Responsibilities

| Agent | Main Job | Should Not Do |
| --- | --- | --- |
| `Intake Router` | 意図分類と次の 1 手の決定 | 資料本文の要約回答 |
| `Glossary Normalizer` | 用語揺れの正規化 | 根拠付き最終回答 |
| `Document Locator` | 候補資料の抽出 | 候補資料の真偽判定 |
| `Evidence Verifier` | 承認状態、鮮度、競合の確認 | 検証前提のない推測回答 |
| `Grounded Answerer` | 検証済み根拠から簡潔回答 | 新しい検索 |
| `Gap Reporter` | 不足資料と不足条件の明示 | 推測で空欄を埋める |
| `Renewal Impact Mapper` | 刷新観点の整理 | 差分判定の断定 |
| `Change Diff Scout` | 踏襲 / 変更 / 不明 の差分整理 | 根拠なしの変更提案 |

## Fallback Rules

- `Document Locator` が有力候補を出せない時は `Gap Reporter`
- `Evidence Verifier` が競合を検知した時は `Gap Reporter`
- `Grounded Answerer` は `Evidence Verifier` を通っていない資料を使わない
- `Renewal Impact Mapper` と `Change Diff Scout` は `verified evidence` がない時は回答しない
- `complete` は最終回答がユーザーへ返る時だけ使う

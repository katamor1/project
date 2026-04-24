<!-- docs/copilot-studio/legacy-search/routing-matrix.md -->
<!-- This file defines the explicit v2 routing matrix for the legacy search multi-agent pack. -->
<!-- This file exists so the team can operate the pack with Router-first entry and handoff packets. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/shared-io-contract.yaml, docs/copilot-studio/legacy-search/README.md, docs/copilot-studio/legacy-search/pilot-runbook.md -->
# Routing Matrix

## Default Routes

| Intent | Route | Final Agent | Stop Rule |
| --- | --- | --- | --- |
| 現行仕様を知りたい | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-grounded-answerer | `ibmbob-legacy-search-grounded-answerer` | 根拠が弱ければ `ibmbob-legacy-search-gap-reporter` |
| 用語が曖昧な質問 | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-grounded-answerer | `ibmbob-legacy-search-grounded-answerer` | 正規化できなければ `ibmbob-legacy-search-gap-reporter` |
| どの資料を見ればよいか知りたい | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier | `ibmbob-legacy-search-evidence-verifier` | 有力資料がなければ `ibmbob-legacy-search-gap-reporter` |
| 資料同士が食い違うか確認したい | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-gap-reporter | `ibmbob-legacy-search-gap-reporter` | 競合が見つかった時点で停止 |
| 刷新時の見直し観点を知りたい | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-renewal-impact-mapper | `ibmbob-legacy-search-renewal-impact-mapper` | 根拠不足なら `ibmbob-legacy-search-gap-reporter` |
| 新要求との差分を知りたい | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-change-diff-scout | `ibmbob-legacy-search-change-diff-scout` | 現行根拠不足なら `ibmbob-legacy-search-gap-reporter` |
| 質問が広すぎる | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-gap-reporter | `ibmbob-legacy-search-gap-reporter` | 検索を開始しない |
| SharePoint / OneDrive 外の質問 | ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-gap-reporter | `ibmbob-legacy-search-gap-reporter` | 対象外として停止 |

## Agent Responsibilities

| Agent | Main Job | Should Not Do |
| --- | --- | --- |
| `ibmbob-legacy-search-intake-router` | 意図分類と次の 1 手の決定 | 資料本文の要約回答 |
| `ibmbob-legacy-search-glossary-normalizer` | 用語揺れの正規化 | 根拠付き最終回答 |
| `ibmbob-legacy-search-document-locator` | 候補資料の抽出 | 候補資料の真偽判定 |
| `ibmbob-legacy-search-evidence-verifier` | 承認状態、鮮度、競合の確認 | 検証前提のない推測回答 |
| `ibmbob-legacy-search-grounded-answerer` | 検証済み根拠から簡潔回答 | 新しい検索 |
| `ibmbob-legacy-search-gap-reporter` | 不足資料と不足条件の明示 | 推測で空欄を埋める |
| `ibmbob-legacy-search-renewal-impact-mapper` | 刷新観点の整理 | 差分判定の断定 |
| `ibmbob-legacy-search-change-diff-scout` | 踏襲 / 変更 / 不明 の差分整理 | 根拠なしの変更提案 |

## Operating Rules

- 新規会話は `ibmbob-legacy-search-intake-router` だけから始めます。
- `LS-02` 以降を直接開いた場合は通常処理をせず、`ibmbob-legacy-search-intake-router` へ戻します。
- 各エージェントは必ず `handoff_packet` を返します。
- 人が `handoff_packet` を次のエージェントへ貼り替えて進めます。
- `complete` を既定で返してよいのは `LS-05`, `LS-07`, `LS-08` だけです。
- `human_follow_up` を既定で返してよいのは `LS-06` だけです。

## Fallback Rules

- `ibmbob-legacy-search-document-locator` が有力候補を出せない時は `ibmbob-legacy-search-gap-reporter`
- `ibmbob-legacy-search-evidence-verifier` が競合を検知した時は `ibmbob-legacy-search-gap-reporter`
- `ibmbob-legacy-search-grounded-answerer` は `ibmbob-legacy-search-evidence-verifier` を通っていない資料を使わない
- `ibmbob-legacy-search-renewal-impact-mapper` と `ibmbob-legacy-search-change-diff-scout` は `verified evidence` がない時は回答しない
- `complete` は最終回答がユーザーへ返る時だけ使う


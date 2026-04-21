<!-- docs/copilot-studio/legacy-search/README.md -->
<!-- This file indexes the Copilot Studio prompt pack for legacy asset search. -->
<!-- This file exists to help PMs and designers deploy the pack in a simple, consistent way. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/shared-io-contract.yaml, docs/copilot-studio/legacy-search/routing-matrix.md, docs/copilot-studio/legacy-search/tests/evaluation-set.yaml -->
# Copilot Studio Legacy Search Prompt Pack

このフォルダは、現行サービスの旧資産検索に特化した `Copilot Studio` 向け Prompt Pack です。

主利用者は `PM / 設計者` です。

主なナレッジソースは `SharePoint / OneDrive` です。

回答方針は `根拠付き`, `日本語`, `根拠不足時は abstain` です。

## Contents

- `shared-io-contract.yaml`: 全エージェントで共有する入力と出力の契約
- `routing-matrix.md`: 質問タイプごとの推奨ルート
- `agents/`: 8 エージェント分のコピー用 prompt
- `tests/evaluation-set.yaml`: v1 の評価用 20 問セット

## Agents

1. `Intake Router`
2. `Glossary Normalizer`
3. `Document Locator`
4. `Evidence Verifier`
5. `Grounded Answerer`
6. `Gap Reporter`
7. `Renewal Impact Mapper`
8. `Change Diff Scout`

## Recommended Setup

- 1 エージェント 1 役割で分けます。
- 全エージェントで同じ knowledge source を参照します。
- hidden handoff には依存しません。
- 各エージェントは必ず `next_agent` を返します。
- 検索結果の引用元は、`タイトル`, `保存場所`, `section/page` を残します。
- 根拠が弱い時は答えを作らず、`Gap Reporter` に止めます。

## Operating Rules

- コードリポジトリや外部 Web は v1 では使いません。
- 一般知識で補完しません。
- 競合する資料がある時は、強い資料を優先しつつ競合を明示します。
- 資料が広すぎる時は、まず検索対象を狭めます。
- 回答は短くします。

## Basic Flow

通常の現行仕様確認は次の流れです。

`Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Grounded Answerer`

刷新観点の整理は次の流れです。

`Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Renewal Impact Mapper`

新要求との差分確認は次の流れです。

`Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Change Diff Scout`

根拠不足や対象外は次の流れで止めます。

`Intake Router -> Gap Reporter`

または

`Document Locator / Evidence Verifier -> Gap Reporter`

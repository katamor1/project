<!-- docs/copilot-studio/legacy-search/implementation-guide.md -->
<!-- This file defines the Copilot Studio implementation guide for the legacy search prompt pack v2. -->
<!-- This file exists to help the maker configure agents, knowledge, and release settings consistently. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/readiness/knowledge-source-catalog.yaml, docs/copilot-studio/legacy-search/readiness/knowledge-onboarding.md, docs/copilot-studio/legacy-search/pilot-runbook.md -->
# Copilot Studio Implementation Guide

## Agent Layout

- `LS-01 Intake Router` を唯一の入口にします。
- `LS-02` から `LS-08` は独立 agent として作ります。
- 全 agent に同じ knowledge source を `agent-level` で載せます。
- knowledge source は 1 系統に固定します。

## Knowledge Source Naming

knowledge source 名は `<domain>-legacy-search-main` 形式に固定します。

例:

- `billing-legacy-search-main`
- `member-legacy-search-main`
- `ops-legacy-search-main`

詳細な台帳は `readiness/knowledge-source-catalog.yaml` を使います。

初回の範囲確認は `readiness/knowledge-onboarding.md` を使います。

## Knowledge Source Description Template

以下の 6 行を description に必ず入れます。

```text
対象業務:
含む文書:
含まない文書:
主な用語:
更新責任者:
鮮度の目安:
```

## Agent-Level Settings

- 全 8 agent で `Web Search` を無効化します。
- 全 8 agent で `Use general knowledge` を無効化します。
- knowledge source は全 agent 共通にします。
- source 名と説明文は一意にします。
- 検索結果の差を作らないため、topic-level の追加知識は v1 では使いません。

## Publish Flow

1. knowledge source を追加する
2. source の status が `Ready` になるまで待つ
3. `LS-01` から `LS-08` を順に作る
4. 各 agent prompt を貼る
5. Test panel で接続確認を行う
6. `readiness/pilot-scorecard.yaml` を用意する
7. 20 問 + 5 問の評価を回す
8. 合格後に `Pilot team` のみへ公開する

## Pilot Release Conditions

- 25 ケース中 23 ケース以上で期待どおり
- abstain 対象で推測回答が 0 件
- `handoff_packet` 欠落が 0 件
- final 系 agent で `chat_response` と `pm_copy_template` の欠落が 0 件

## Block Conditions

以下のいずれかがある時は公開しません。

- `Restricted SharePoint Search` が有効
- guest user 前提
- knowledge source が `Ready` でない
- 初回の権限検証に失敗

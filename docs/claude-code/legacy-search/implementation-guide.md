<!-- docs/claude-code/legacy-search/implementation-guide.md -->
<!-- This file defines the Claude Code implementation guide for the legacy search workflow pack v2. -->
<!-- This file exists to help the team configure workflow steps, knowledge roots, and pilot settings consistently. -->
<!-- RELEVANT FILES: docs/claude-code/legacy-search/readiness/knowledge-source-catalog.yaml, docs/claude-code/legacy-search/readiness/knowledge-onboarding.md, docs/claude-code/legacy-search/pilot-runbook.md -->
# Claude Code Implementation Guide

## Agent Layout

- `LS-01 Intake Router` を唯一の入口にします。
- `LS-02` から `LS-08` は独立ステップとして扱います。
- 全ステップで同じ `allowed knowledge roots` を使います。
- knowledge root は 1 系統に固定します。

## Knowledge Root Naming

knowledge root 名は `<domain>-legacy-search-main` 形式に固定します。

例:

- `billing-legacy-search-main`
- `member-legacy-search-main`
- `ops-legacy-search-main`

詳細な台帳は `readiness/knowledge-source-catalog.yaml` を使います。

初回の範囲確認は `readiness/knowledge-onboarding.md` を使います。

## Knowledge Root Description Template

以下の 6 行を description に必ず入れます。

```text
対象業務:
含む文書:
含まない文書:
主な用語:
更新責任者:
鮮度の目安:
```

## Claude Code Rules

- `allowed knowledge roots` は全 8 ステップで共通にします。
- `search order` は `approved docs -> working docs -> unknown docs` に固定します。
- `forbidden sources` は `code repo`, `external web`, `general knowledge` に固定します。
- 1 thread 内で step-by-step handoff を進めます。
- source 名と説明文は一意にします。

## Publish Flow

1. knowledge source を追加する
2. access と root path が確定しているか確認する
3. `LS-01` から `LS-08` の prompt を順に用意する
4. 各 step 用 prompt を thread で使える形に整える
5. root 接続と検索順序を確認する
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

- knowledge root が未確定
- connector auth が未確定
- allowed root の境界が未確定
- 初回の access 検証に失敗

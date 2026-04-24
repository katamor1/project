<!-- ibm-bob/mode-pack/legacy-search-guide.md -->
<!-- Explains how to operate the LS custom modes inside IBM Bob using the existing legacy-search contracts. -->
<!-- This exists so the team can run legacy document search in Bob without depending on Copilot Studio agent settings. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, docs/copilot-studio/legacy-search/README.md, docs/copilot-studio/legacy-search/shared-io-contract.yaml -->
# Legacy Search Guide

## Entry Rule

Bob では `ls-01-intake-router` が唯一の入口です。

`ls-02` 以降から始めないでください。

## Operating Model

- 1 会話で 1 問に絞ります。
- `handoff_packet` を明示的に次 mode へ渡します。
- `candidate_sources` は `ls-03` 以降でだけ増やします。
- 競合資料や根拠不足は `ls-06-gap-reporter` で止めます。

## Recommended Routes

### 現行仕様の確認

`ls-01 -> ls-02 -> ls-03 -> ls-04 -> ls-05`

### 刷新観点

`ls-01 -> ls-02 -> ls-03 -> ls-04 -> ls-07`

### 新旧差分

`ls-01 -> ls-02 -> ls-03 -> ls-04 -> ls-08`

### 安全停止

`ls-01 -> ls-06`

## Knowledge Boundary

v1 では canonical docs の前提を維持します。

つまり、対象は `SharePoint / OneDrive` 相当の legacy 文書 root です。

Bob custom mode pack では、これを `allowed knowledge roots` として運用で固定します。

## Output Discipline

最終系の `ls-05`, `ls-06`, `ls-07`, `ls-08` は次を返します。

- `chat_response`
- `pm_copy_template`
- `next_agent`
- `handoff_packet`

検索系の `ls-01` から `ls-04` は `handoff_packet` を省略しません。

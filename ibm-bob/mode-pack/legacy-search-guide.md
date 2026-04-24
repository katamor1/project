<!-- ibm-bob/mode-pack/legacy-search-guide.md -->
<!-- Explains how the new Bob direct-mode family runs the LS workflow while keeping the old orchestrator family unchanged. -->
<!-- This exists so the team can operate legacy-search in Bob without confusing the generated overlay with the old copied-workspace flow. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/modes/custom_modes.source.yaml, docs/copilot-studio/legacy-search/shared-io-contract.yaml -->
# Legacy Search Guide

## Scope

これは new direct-mode family の guide です。

old `ibmbob-orchestrator` family には入りません。

## Entry Rule

`ibmbob-legacy-search-intake-router` が唯一の入口です。

途中 mode から始めません。

## Main Route

`ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-grounded-answerer`

## Safe Stop

資料が弱い時や不足する時は次に落とします。

`ibmbob-legacy-search-document-locator or ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-gap-reporter`

## Operating Rules

- `handoff_packet` を毎段で維持する
- `candidate_sources` は `document-locator` と `evidence-verifier` でだけ増減する
- `chat_response` と `pm_copy_template` は最終系で返す
- weak evidence は補完せず止まる

## Knowledge Boundary

canonical `legacy-search` docs の前提をそのまま使います。

Bob 側ではこれを `allowed knowledge roots` の運用として扱います。


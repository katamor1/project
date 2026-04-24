<!-- ibm-bob/mode-pack/samples/direct-modes/legacy-search-dry-run.md -->
<!-- Shows a dry run for the new Bob direct legacy-search family. -->
<!-- This exists so the team can validate explicit handoff-based search separately from the old orchestrator family. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/legacy-search-guide.md, ibm-bob/mode-pack/pilot-runbook.md, docs/copilot-studio/legacy-search/README.md -->
# Legacy Search Dry Run

## User Intent

「通知設定画面の現行仕様を、根拠付きで短く知りたい」

## Suggested Mode Path

`ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-grounded-answerer`

## What To Check

- `handoff_packet` が毎段で維持されること
- `candidate_sources` が 3-5 件程度へ絞られること
- 最終系が `chat_response` と `pm_copy_template` を返すこと


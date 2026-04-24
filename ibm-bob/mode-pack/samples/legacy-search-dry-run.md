<!-- ibm-bob/mode-pack/samples/legacy-search-dry-run.md -->
<!-- Shows a Bob dry run for the legacy-search workflow using the LS custom modes. -->
<!-- This exists so the team can validate that the LS overlay works in Bob with explicit handoff packets. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/legacy-search-guide.md, .bob/custom_modes.yaml, docs/copilot-studio/legacy-search/README.md -->
# Legacy Search Dry Run

## User Intent

「通知設定画面の現行仕様を、根拠付きで短く知りたい」

## Suggested Mode Path

`ls-01-intake-router -> ls-02-glossary-normalizer -> ls-03-document-locator -> ls-04-evidence-verifier -> ls-05-grounded-answerer`

## What To Check

- `ls-01` が `handoff_packet` を作ること
- `ls-03` が `candidate_sources` を 3-5 件で絞ること
- `ls-04` が弱い資料を落とすこと
- `ls-05` が `chat_response` と `pm_copy_template` を返すこと

## Safe Stop Variant

資料が足りない時は次に落ちることを確認します。

`ls-03 or ls-04 -> ls-06-gap-reporter`

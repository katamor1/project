<!-- ibm-bob/mode-pack/routing-map.md -->
<!-- Summarizes the coexistence of the old fixed-stage family and the new direct-mode family inside one Bob mode pack. -->
<!-- This exists so the team can operate each family with the correct routing source and not mix stage flow with Bob-native direct flow. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/stage-flow.json, ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/pilot-runbook.md -->
# Routing Map

## Old Family

old family は copied-workspace run orchestration です。

正本は `routing/stage-flow.json` です。

代表フローは次です。

`ibmbob-orchestrator -> ibmbob-intake -> ibmbob-context-scout -> ibmbob-detail-design -> ibmbob-review-gate -> ibmbob-code-author -> ibmbob-review-gate -> ibmbob-unit-test-author -> ibmbob-review-gate -> ibmbob-unit-test-executor -> ibmbob-review-gate -> ibmbob-functional-test-author -> ibmbob-review-gate -> ibmbob-run-summary`

## New Direct-Mode Family

new family は overlay です。

family 一覧の正本は `routing/direct-mode-flow.json` です。

実際の canonical routing source は次です。

- `.copilot/routing/entry/ibmbob-entry-flow.yaml`
- `.copilot/routing/runtime/ibmbob-runtime-ingest-flow.yaml`
- `.copilot/routing/runtime/ibmbob-runtime-query-flow.yaml`
- `.copilot/routing/sdlc/ibmbob-design-flow.yaml`
- `.copilot/routing/sdlc/ibmbob-test-spec-flow.yaml`
- `.copilot/routing/sdlc/ibmbob-implementation-flow.yaml`
- `.copilot/routing/sdlc/ibmbob-execution-flow.yaml`
- `.copilot/routing/sdlc/ibmbob-integration-flow.yaml`
- `.copilot/routing/legacy-search/ibmbob-legacy-search-flow.yaml`

### Entry

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author`

### Source-Backed Design

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-retrieval-planner -> ibmbob-artifact-context-packer -> ibmbob-evidence-reviewer -> ibmbob-sdlc-basic-design-author`

### Diff

`ibmbob-entry-router -> ibmbob-intent-packet-builder -> ibmbob-identity-scope-guard -> ibmbob-dispatch-packet-author -> ibmbob-runtime-orchestrator -> ibmbob-legacy-diff-checker -> ibmbob-evidence-reviewer -> ibmbob-entry-response-shaper`

### Legacy Search

`ibmbob-legacy-search-intake-router -> ibmbob-legacy-search-glossary-normalizer -> ibmbob-legacy-search-document-locator -> ibmbob-legacy-search-evidence-verifier -> ibmbob-legacy-search-grounded-answerer`

### Review / Eval

`ibmbob-entry-router -> ... -> ibmbob-sdlc-review-record`

`ibmbob-entry-router -> ... -> ibmbob-sdlc-eval-monitor`

## Gate Rules

- `identity_missing`, `tenant_mismatch`, `scope_missing` は `ibmbob-identity-scope-guard` で止める
- `low confidence`, `ACL mismatch`, `authoritative conflict` は `ibmbob-entry-response-shaper` の abstain に落とす
- `artifact_context_packet` が無い時は `ibmbob-sdlc-basic-design-author`, `ibmbob-sdlc-implementation-planner`, `ibmbob-sdlc-test-author` を始めない

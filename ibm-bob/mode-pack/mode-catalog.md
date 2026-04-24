<!-- ibm-bob/mode-pack/mode-catalog.md -->
<!-- Lists the old orchestrator family and the new direct-mode family that are generated into one Bob custom mode file. -->
<!-- This exists so the team can see which Bob slug belongs to which family and canonical source. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/routing/stage-flow.json -->
# Mode Catalog

## Old Family

この 10 mode は既存 family です。

順序も slug も維持します。

| Slug | Role |
|---|---|
| `ibmbob-orchestrator` | fixed stage run の入口 |
| `ibmbob-intake` | copied workspace と command readiness の確認 |
| `ibmbob-context-scout` | 基本設計の context 抽出 |
| `ibmbob-detail-design` | 詳細設計 packet の作成 |
| `ibmbob-review-gate` | pass / revise / block 判定 |
| `ibmbob-code-author` | code change packet 作成 |
| `ibmbob-unit-test-author` | unit test pack 作成 |
| `ibmbob-unit-test-executor` | unit test 実行 |
| `ibmbob-functional-test-author` | functional test pack 作成 |
| `ibmbob-run-summary` | run summary 作成 |

## New Direct-Mode Family

### Entry

| Slug | Canonical Prompt |
|---|---|
| `ibmbob-entry-router` | `ibmbob-entry-router.md` |
| `ibmbob-intent-packet-builder` | `ibmbob-intent-packet-builder.md` |
| `ibmbob-identity-scope-guard` | `ibmbob-identity-scope-guard.md` |
| `ibmbob-dispatch-packet-author` | `ibmbob-dispatch-packet-author.md` |
| `ibmbob-entry-response-shaper` | `ibmbob-entry-response-shaper.md` |
| `ibmbob-entry-eval-monitor` | `ibmbob-entry-eval-monitor.md` |

### Runtime

| Slug | Canonical Prompt |
|---|---|
| `ibmbob-runtime-orchestrator` | `ibmbob-runtime-orchestrator.md` |
| `ibmbob-intake-normalizer` | `ibmbob-intake-normalizer.md` |
| `ibmbob-taxonomy-mapper` | `ibmbob-taxonomy-mapper.md` |
| `ibmbob-summary-tree-builder` | `ibmbob-summary-tree-builder.md` |
| `ibmbob-relation-extractor` | `ibmbob-relation-extractor.md` |
| `ibmbob-retrieval-planner` | `ibmbob-retrieval-planner.md` |
| `ibmbob-legacy-diff-checker` | `ibmbob-legacy-diff-checker.md` |
| `ibmbob-artifact-context-packer` | `ibmbob-artifact-context-packer.md` |
| `ibmbob-evidence-reviewer` | `ibmbob-evidence-reviewer.md` |
| `ibmbob-runtime-eval-monitor` | `ibmbob-runtime-eval-monitor.md` |

### SDLC

| Slug | Canonical Prompt |
|---|---|
| `ibmbob-sdlc-request-orchestrator` | `ibmbob-sdlc-request-orchestrator.md` |
| `ibmbob-sdlc-scope` | `ibmbob-sdlc-scope.md` |
| `ibmbob-sdlc-basic-design-author` | `ibmbob-sdlc-basic-design-author.md` |
| `ibmbob-sdlc-detail-design-author` | `ibmbob-sdlc-detail-design-author.md` |
| `ibmbob-sdlc-functional-spec-author` | `ibmbob-sdlc-functional-spec-author.md` |
| `ibmbob-sdlc-integration-spec-author` | `ibmbob-sdlc-integration-spec-author.md` |
| `ibmbob-sdlc-spec-reviewer` | `ibmbob-sdlc-spec-reviewer.md` |
| `ibmbob-sdlc-implementation-planner` | `ibmbob-sdlc-implementation-planner.md` |
| `ibmbob-sdlc-fullstack-slice-author` | `ibmbob-sdlc-fullstack-slice-author.md` |
| `ibmbob-sdlc-test-author` | `ibmbob-sdlc-test-author.md` |
| `ibmbob-sdlc-code-reviewer` | `ibmbob-sdlc-code-reviewer.md` |
| `ibmbob-sdlc-review-record` | `ibmbob-sdlc-review-record.md` |
| `ibmbob-sdlc-eval-monitor` | `ibmbob-sdlc-eval-monitor.md` |
| `ibmbob-sdlc-execution-bundle-planner` | `ibmbob-sdlc-execution-bundle-planner.md` |
| `ibmbob-sdlc-frontend-codepack-author` | `ibmbob-sdlc-frontend-codepack-author.md` |
| `ibmbob-sdlc-backend-codepack-author` | `ibmbob-sdlc-backend-codepack-author.md` |
| `ibmbob-sdlc-test-codepack-author` | `ibmbob-sdlc-test-codepack-author.md` |
| `ibmbob-sdlc-execution-reviewer` | `ibmbob-sdlc-execution-reviewer.md` |
| `ibmbob-sdlc-integration-bundle-planner` | `ibmbob-sdlc-integration-bundle-planner.md` |
| `ibmbob-sdlc-frontend-draft-author` | `ibmbob-sdlc-frontend-draft-author.md` |
| `ibmbob-sdlc-backend-draft-author` | `ibmbob-sdlc-backend-draft-author.md` |
| `ibmbob-sdlc-sqlite-draft-author` | `ibmbob-sdlc-sqlite-draft-author.md` |
| `ibmbob-sdlc-test-draft-author` | `ibmbob-sdlc-test-draft-author.md` |
| `ibmbob-sdlc-integration-reviewer` | `ibmbob-sdlc-integration-reviewer.md` |
| `ibmbob-sdlc-approval-pack-author` | `ibmbob-sdlc-approval-pack-author.md` |

### Legacy Search

| Slug | Canonical Source |
|---|---|
| `ibmbob-legacy-search-intake-router` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-intake-router.md` |
| `ibmbob-legacy-search-glossary-normalizer` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-glossary-normalizer.md` |
| `ibmbob-legacy-search-document-locator` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-document-locator.md` |
| `ibmbob-legacy-search-evidence-verifier` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-evidence-verifier.md` |
| `ibmbob-legacy-search-grounded-answerer` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-grounded-answerer.md` |
| `ibmbob-legacy-search-gap-reporter` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-gap-reporter.md` |
| `ibmbob-legacy-search-renewal-impact-mapper` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-renewal-impact-mapper.md` |
| `ibmbob-legacy-search-change-diff-scout` | `.copilot/prompts/legacy-search/ibmbob-legacy-search-change-diff-scout.md` |



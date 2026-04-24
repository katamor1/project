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
| `ibmbob-entry-router` | `C0_copilot_entry_router.md` |
| `ibmbob-intent-packet-builder` | `C1_intent_packet_builder.md` |
| `ibmbob-identity-scope-guard` | `C2_identity_scope_guard.md` |
| `ibmbob-dispatch-packet-author` | `C3_dispatch_packet_author.md` |
| `ibmbob-entry-response-shaper` | `C4_bridge_response_shaper.md` |
| `ibmbob-entry-eval-monitor` | `C5_bridge_eval_monitor.md` |

### Runtime

| Slug | Canonical Prompt |
|---|---|
| `ibmbob-runtime-orchestrator` | `K0_runtime_orchestrator.md` |
| `ibmbob-intake-normalizer` | `K1_intake_normalizer.md` |
| `ibmbob-taxonomy-mapper` | `K2_taxonomy_mapper.md` |
| `ibmbob-summary-tree-builder` | `K3_summary_tree_builder.md` |
| `ibmbob-relation-extractor` | `K4_relation_extractor.md` |
| `ibmbob-retrieval-planner` | `K5_retrieval_planner.md` |
| `ibmbob-legacy-diff-checker` | `K6_legacy_diff_checker.md` |
| `ibmbob-artifact-context-packer` | `K7_artifact_context_packer.md` |
| `ibmbob-evidence-reviewer` | `K8_evidence_reviewer.md` |
| `ibmbob-runtime-eval-monitor` | `K9_runtime_eval_monitor.md` |

### SDLC

| Slug | Canonical Prompt |
|---|---|
| `ibmbob-sdlc-request-orchestrator` | `P0_orchestrator.md` |
| `ibmbob-sdlc-scope` | `P1_scope.md` |
| `ibmbob-sdlc-basic-design-author` | `P2_basic_design_author.md` |
| `ibmbob-sdlc-detail-design-author` | `P2_detail_design_author.md` |
| `ibmbob-sdlc-functional-spec-author` | `P2_functional_spec_author.md` |
| `ibmbob-sdlc-integration-spec-author` | `P2_integration_spec_author.md` |
| `ibmbob-sdlc-spec-reviewer` | `P3_spec_reviewer.md` |
| `ibmbob-sdlc-implementation-planner` | `P4_implementation_planner.md` |
| `ibmbob-sdlc-fullstack-slice-author` | `P5_fullstack_slice_author.md` |
| `ibmbob-sdlc-test-author` | `P6_test_author.md` |
| `ibmbob-sdlc-code-reviewer` | `P7_code_reviewer.md` |
| `ibmbob-sdlc-review-record` | `P8_review_record.md` |
| `ibmbob-sdlc-eval-monitor` | `P9_eval_monitor.md` |
| `ibmbob-sdlc-execution-bundle-planner` | `P10_execution_bundle_planner.md` |
| `ibmbob-sdlc-frontend-codepack-author` | `P11_frontend_codepack_author.md` |
| `ibmbob-sdlc-backend-codepack-author` | `P12_backend_codepack_author.md` |
| `ibmbob-sdlc-test-codepack-author` | `P13_test_codepack_author.md` |
| `ibmbob-sdlc-execution-reviewer` | `P14_execution_reviewer.md` |
| `ibmbob-sdlc-integration-bundle-planner` | `P15_integration_bundle_planner.md` |
| `ibmbob-sdlc-frontend-draft-author` | `P16_frontend_draft_author.md` |
| `ibmbob-sdlc-backend-draft-author` | `P17_backend_draft_author.md` |
| `ibmbob-sdlc-sqlite-draft-author` | `P18_sqlite_draft_author.md` |
| `ibmbob-sdlc-test-draft-author` | `P19_test_draft_author.md` |
| `ibmbob-sdlc-integration-reviewer` | `P20_integration_reviewer.md` |
| `ibmbob-sdlc-approval-pack-author` | `P21_approval_pack_author.md` |

### Legacy Search

| Slug | Canonical Source |
|---|---|
| `ibmbob-legacy-search-intake-router` | `docs/copilot-studio/legacy-search/agents/intake-router.md` |
| `ibmbob-legacy-search-glossary-normalizer` | `docs/copilot-studio/legacy-search/agents/glossary-normalizer.md` |
| `ibmbob-legacy-search-document-locator` | `docs/copilot-studio/legacy-search/agents/document-locator.md` |
| `ibmbob-legacy-search-evidence-verifier` | `docs/copilot-studio/legacy-search/agents/evidence-verifier.md` |
| `ibmbob-legacy-search-grounded-answerer` | `docs/copilot-studio/legacy-search/agents/grounded-answerer.md` |
| `ibmbob-legacy-search-gap-reporter` | `docs/copilot-studio/legacy-search/agents/gap-reporter.md` |
| `ibmbob-legacy-search-renewal-impact-mapper` | `docs/copilot-studio/legacy-search/agents/renewal-impact-mapper.md` |
| `ibmbob-legacy-search-change-diff-scout` | `docs/copilot-studio/legacy-search/agents/change-diff-scout.md` |


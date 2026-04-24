<!-- ibm-bob/mode-pack/mode-catalog.md -->
<!-- Lists the IBM Bob custom modes that mirror the existing C, K, P, and LS prompt families. -->
<!-- This exists so the team can see which Bob slug maps to which canonical prompt and artifact. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, .copilot/prompts, ibm-bob/mode-pack/routing-map.md -->
# Mode Catalog

## Entry Bridge Modes

| Slug | Canonical Prompt | Main Output | Main Use |
|---|---|---|---|
| `c0-entry-router` | `C0_copilot_entry_router.md` | `bridge_route_decision` | 入口分類 |
| `c1-intent-packet-builder` | `C1_intent_packet_builder.md` | `bridge_request_packet` | chat 正規化 |
| `c2-identity-scope-guard` | `C2_identity_scope_guard.md` | `bridge_identity_context` / `bridge_human_checkpoint` | identity / scope gate |
| `c3-dispatch-packet-author` | `C3_dispatch_packet_author.md` | `bridge_dispatch_packet` | runtime dispatch |
| `c4-entry-response-shaper` | `C4_bridge_response_shaper.md` | `bridge_response` | Bob 向け応答整形 |
| `c5-entry-eval-monitor` | `C5_bridge_eval_monitor.md` | `bridge_eval_note` | 入口品質監視 |

## Legacy Runtime Modes

| Slug | Canonical Prompt | Main Output | Main Use |
|---|---|---|---|
| `k0-runtime-orchestrator` | `K0_runtime_orchestrator.md` | `runtime_task_packet` | runtime 次アクション |
| `k1-intake-normalizer` | `K1_intake_normalizer.md` | `ingest_job` | ingest 正規化 |
| `k2-taxonomy-mapper` | `K2_taxonomy_mapper.md` | `document_taxonomy_bundle` | taxonomy 付与 |
| `k3-summary-tree-builder` | `K3_summary_tree_builder.md` | `summary_tree_bundle` | map / chunk 生成 |
| `k4-relation-extractor` | `K4_relation_extractor.md` | `relation_edge_bundle` | relation graph |
| `k5-retrieval-planner` | `K5_retrieval_planner.md` | `retrieval_plan` | retrieval 計画 |
| `k6-legacy-diff-checker` | `K6_legacy_diff_checker.md` | `diff_report` | 旧新差分 |
| `k7-artifact-context-packer` | `K7_artifact_context_packer.md` | `artifact_context_packet` | P レーン handoff |
| `k8-evidence-reviewer` | `K8_evidence_reviewer.md` | `evidence_review` | confidence / ACL / conflict gate |
| `k9-runtime-eval-monitor` | `K9_runtime_eval_monitor.md` | `runtime_eval_note` | runtime 評価 |

## SDLC Modes

| Slug | Canonical Prompt | Main Output | Main Use |
|---|---|---|---|
| `p0-orchestrator` | `P0_orchestrator.md` | `request_packet` | 生依頼の正規化 |
| `p1-scope` | `P1_scope.md` | `scope_packet` | 対象 / 非対象整理 |
| `p2-basic-design-author` | `P2_basic_design_author.md` | `basic_design` | 基本設計 |
| `p2-detail-design-author` | `P2_detail_design_author.md` | `detail_design` | 詳細設計 |
| `p2-functional-spec-author` | `P2_functional_spec_author.md` | `functional_spec` | 機能試験仕様 |
| `p2-integration-spec-author` | `P2_integration_spec_author.md` | `integration_spec` | 結合試験仕様 |
| `p3-spec-reviewer` | `P3_spec_reviewer.md` | `review_findings` | 仕様レビュー |
| `p4-implementation-planner` | `P4_implementation_planner.md` | `implementation_packet` | 実装計画 |
| `p5-fullstack-slice-author` | `P5_fullstack_slice_author.md` | `fullstack_slice` | full-stack slice |
| `p6-test-author` | `P6_test_author.md` | `test_pack` | テスト設計 |
| `p7-code-reviewer` | `P7_code_reviewer.md` | `code_review_findings` | 実装準備レビュー |
| `p8-review-record` | `P8_review_record.md` | `review_record` | レビュー記録 |
| `p9-eval-monitor` | `P9_eval_monitor.md` | `eval_report` | 評価整理 |
| `p10-execution-bundle-planner` | `P10_execution_bundle_planner.md` | `execution_bundle_packet` | execution pack 計画 |
| `p11-frontend-codepack-author` | `P11_frontend_codepack_author.md` | `frontend_codepack` | frontend near-code pack |
| `p12-backend-codepack-author` | `P12_backend_codepack_author.md` | `backend_codepack` / `sqlite_change_pack` | backend near-code pack |
| `p13-test-codepack-author` | `P13_test_codepack_author.md` | `test_codepack` | test near-code pack |
| `p14-execution-reviewer` | `P14_execution_reviewer.md` | `execution_review_findings` | execution pack review |
| `p15-integration-bundle-planner` | `P15_integration_bundle_planner.md` | `code_draft_packet` | integration bundle 計画 |
| `p16-frontend-draft-author` | `P16_frontend_draft_author.md` | `frontend_draft_bundle` | frontend draft |
| `p17-backend-draft-author` | `P17_backend_draft_author.md` | `backend_draft_bundle` | backend draft |
| `p18-sqlite-draft-author` | `P18_sqlite_draft_author.md` | `sqlite_draft_bundle` | SQLite draft |
| `p19-test-draft-author` | `P19_test_draft_author.md` | `test_draft_bundle` | test draft |
| `p20-integration-reviewer` | `P20_integration_reviewer.md` | `integration_review_findings` | integration review |
| `p21-approval-pack-author` | `P21_approval_pack_author.md` | `approval_pack` | 実装着手可否 |

## Legacy Search Modes

| Slug | Canonical Prompt | Main Output | Main Use |
|---|---|---|---|
| `ls-01-intake-router` | `intake-router.md` | router JSON + `handoff_packet` | 唯一の入口 |
| `ls-02-glossary-normalizer` | `glossary-normalizer.md` | normalized entities | 用語正規化 |
| `ls-03-document-locator` | `document-locator.md` | `candidate_sources` | 資料候補抽出 |
| `ls-04-evidence-verifier` | `evidence-verifier.md` | verified candidates | 根拠検証 |
| `ls-05-grounded-answerer` | `grounded-answerer.md` | `chat_response` | 根拠付き回答 |
| `ls-06-gap-reporter` | `gap-reporter.md` | gap report | 安全停止 |
| `ls-07-renewal-impact-mapper` | `renewal-impact-mapper.md` | impact areas | 刷新観点整理 |
| `ls-08-change-diff-scout` | `change-diff-scout.md` | diff items | 新旧差分整理 |

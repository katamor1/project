# External Knowledge Runtime Blueprint v1

## Summary
- 次フェーズは、今ある `docs/sdlc` の `Copilot-only` 設計資産を、`Azure-first` の外部 `Knowledge / Retrieval / Orchestrator` 設計へ接続する。
- 実装単位は新しい独立トラック `docs/external-runtime/legacy-kb/` とし、既存の `docs/claude-code` と `docs/sdlc` はそのまま残す。
- 今回は `Full Blueprint` で切るが、まだ runnable service code、IaC、デプロイ実装は入れない。追加するのは docs、schemas、prompts、routing、samples、eval/runbook まで。
- 既存の `P*` レーンは維持し、以後の source-backed generation は `source_refs` 直渡しではなく `evidence_bundle` / `artifact_context_packet` を優先入力にする。

## Key Changes
- `docs/external-runtime/legacy-kb/` を新設し、少なくとも次を追加する。
- `README.md`: 全体像と既存 `docs/sdlc` との接続点
- `architecture.md`: M365 Copilot → custom engine → Azure AI Search / Blob の責務分離
- `azure-topology.md`: Azure-first 前提の runtime, storage, search, ACL, trace 配置
- `data-pipeline.md`: 収集 → normalize → map → chunk → summary tree → relation graph → incremental reindex
- `query-diff-flow.md`: retrieval と diff の段階フロー
- `security-acl.md`: ACL attach, query-time trim, evidence-time recheck
- `api-spec.md`: 外部 runtime API と contract
- `eval-ops.md`: KPI, trace, pilot exit criteria
- `runbook.md`: ingestion, reindex, incident, rollback の運用手順
- `samples/`: `email-notification-ingest-dry-run.md`, `email-notification-query-dry-run.md`, `email-notification-diff-dry-run.md`

- `.copilot/prompts/` に外部 knowledge track 用の `K*` prompt を追加する。
- `K0_runtime_orchestrator`: 入口、next action と stop condition を決める
- `K1_intake_normalizer`: file metadata と naming を正規化する
- `K2_taxonomy_mapper`: `doc_type`, `module`, `business_domain`, `approval_status` を付ける
- `K3_summary_tree_builder`: document map / section summary / chapter summary を作る
- `K4_relation_extractor`: screen, API, table, error, batch の relation edge を作る
- `K5_retrieval_planner`: metadata filter → hybrid retrieve → rerank → leaf read を設計する
- `K6_legacy_diff_checker`: `change / carry_over / unknown` で差分を返す
- `K7_artifact_context_packer`: `evidence_bundle` を `artifact_context_packet` に変換して `P*` レーンへ渡す
- `K8_evidence_reviewer`: evidence conflict, weak authority, ACL leak を判定する
- `K9_runtime_eval_monitor`: retrieval/diff/runtime の評価観点を整理する

- `.copilot/routing/` に 2 本の routing を追加する。
- `legacy-kb-ingest-matrix.yaml`: `K0 -> K1 -> K2 -> K3 -> K4 -> K9`
- `legacy-kb-query-matrix.yaml`: `K0 -> K5 -> K7 or K6 -> K8 -> existing P* / P8 / P9`
- 共通ルールは固定する。
- `confidence < 0.65`, ACL mismatch, conflicting authoritative sources は `K0` に戻して `human_checkpoint_required`
- `0.65-0.78` は rewrite + relation expansion + second pass
- `>= 0.78` のみ downstream artifact generation に進める

- `.copilot/schemas/` に外部 runtime contract を追加する。
- `document-record.schema.json`
- `section-node.schema.json`
- `chunk-record.schema.json`
- `relation-edge.schema.json`
- `acl-principal-set.schema.json`
- `ingest-job.schema.json`
- `retrieval-plan.schema.json`
- `evidence-bundle.schema.json`
- `artifact-context-packet.schema.json`
- `diff-report.schema.json`
- `run-trace.schema.json`
- core field は固定する。
- `document-record`: `doc_id`, `version_id`, `source_path`, `doc_type`, `module`, `approval_status`, `acl`, `hash`
- `chunk-record`: `chunk_id`, `parent_id`, `heading_path`, `page_or_sheet`, `authority_score`, `embedding_model`
- `evidence-bundle`: `task_id`, `query_plan`, `retrieved_items`, `coverage_assessment`, `confidence`, `risk_flags`
- `artifact-context-packet`: `artifact_request`, `evidence_bundle_ref`, `resolved_entities`, `open_questions`, `allowed_assertions`
- `diff-report`: `diff_items`, `carry_over_items`, `unknown_items`, `impact_estimate`
- `run-trace`: `run_id`, `agent_path`, `tool_path`, `retrieved_doc_ids`, `decision_flags`, `latency_ms`, `token_usage`

- 外部 runtime API は Azure-first で固定する。
- `POST /ingest/files`
- `POST /ingest/reindex`
- `POST /kb/search`
- `POST /kb/read`
- `POST /kb/graph-expand`
- `POST /diff/check`
- `POST /context/pack`
- `GET /run/{id}`
- `POST /eval/replay`
- index 構成も固定する。
- `legacy_doc_map`
- `legacy_leaf_chunks`
- `legacy_relation_edges`
- raw file は Azure Blob、検索は Azure AI Search、trace/eval metadata は app-managed store に置く。

- 既存 SDLC との接続を明文化する。
- `P2_basic_design_author`, `P2_detail_design_author`, `P2_functional_spec_author`, `P2_integration_spec_author`, `P4_implementation_planner`, `P6_test_author` は `evidence_bundle_ref` を優先入力にする
- `source_refs` は fallback として残すが、新規 sample と README は `artifact_context_packet` 前提に切り替える
- `P3_spec_reviewer`, `P7_code_reviewer`, `P20_integration_reviewer` は `evidence_coverage` と `authority/conflict note` を参照できる前提に広げる

## Test Plan
- 静的確認を行う。
- 新規 `K*` prompt が既存と同じ `System / User / Assistant / JSON Output Example` 構成を持つこと
- 新規 schema が 4 行 header を除けば parse できること
- 新規 Markdown / JSON / YAML がすべて 300 行未満であること

- ingestion と retrieval の評価ケースを固定する。
- Word / PDF / Excel の 3 種が `document_record -> section_node -> chunk_record` へ落ちる
- 既存文書差し替え時に partial reindex だけで済む
- 同じ query を別 ACL principal で打った時に返却 doc が変わる
- 画面 ID / API 名 / error code の exact match と自然文 query の両方が取れる
- conflicting source と low confidence は downstream に進めず abstain する

- diff と downstream 接続の評価ケースを固定する。
- `change / carry_over / unknown` が根拠付きで返る
- `artifact_context_packet` を使って `P2` / `P4` 系が raw source browsing なしで開始できる
- `unsupported claim` は approved output で 0 件
- pilot exit criteria は次に固定する。
- `Recall@10 >= 0.90`
- `authoritative_source_hit_rate >= 0.85`
- `route_ambiguity = 0`
- `ACL leak = 0`
- 日本語 summary / diff / review note が pilot で読める品質に達する

## Assumptions
- 基盤は `Azure-first` とし、Azure AI Search、Azure Blob、Azure-hosted custom engine を前提にする。
- Microsoft 365 Copilot は薄い入口のまま残し、重い retrieval / diff / generation orchestration は外部 runtime に寄せる。
- このフェーズでは blueprint 資産だけを追加し、実サービスコード、IaC、index provisioning、connector 実装、CI 連携はまだ入れない。
- repo 上の配置は `docs/claude-code`, `docs/sdlc`, `docs/external-runtime` の 3 トラックで固定する。
- 外部 knowledge track は legacy 文書活用専用とし、現段階では app repo 実装トラックと混ぜない。

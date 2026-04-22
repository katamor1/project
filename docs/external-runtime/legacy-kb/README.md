<!-- docs/external-runtime/legacy-kb/README.md -->
<!-- Explains the Azure-first external knowledge runtime that feeds evidence into the SDLC lanes. -->
<!-- This exists so legacy document retrieval, diff, and context packing have a dedicated blueprint behind the Copilot custom engine bridge. -->
<!-- RELEVANT FILES: docs/copilot-studio/custom-engine-bridge/README.md, docs/sdlc/README.md, .copilot/routing/legacy-kb-query-matrix.yaml -->
# External Knowledge Runtime Blueprint

このトラックは、`docs/sdlc` の `P*` レーンへ evidence を渡すための外部 runtime 設計です。

役割は 3 つです。

1. 旧アプリ文書を Azure-first の KB に正規化して蓄積する。

2. query / diff ごとに必要最小限の `evidence_bundle` を作る。

3. `artifact_context_packet` に変換して既存の `P2`, `P4`, `P6` 系へ渡す。

## Scope
- このトラックは blueprint 専用です。
- runnable service code、IaC、connector 実装、CI はまだ入れません。
- 対象は legacy 文書活用専用です。
- app repo 実装トラックとは混ぜません。

## Connection To SDLC
- `docs/copilot-studio/custom-engine-bridge` が front door です。
- `C3_dispatch_packet_author` から `K0_runtime_orchestrator` に入ります。
- `K0-K9` は外部 runtime 側の prompt registry です。
- `K7_artifact_context_packer` が `artifact_context_packet` を作り、既存 `P*` レーンへ handoff します。
- `P2_basic_design_author`, `P2_detail_design_author`, `P2_functional_spec_author`, `P2_integration_spec_author`, `P4_implementation_planner`, `P6_test_author` は `artifact_context_packet` を優先入力にします。
- `source_refs` は fallback だけに残します。

## Directory Map
- `architecture.md`: 論理責務の分離
- `azure-topology.md`: Azure 配置と index 構成
- `data-pipeline.md`: ingest と増分更新
- `query-diff-flow.md`: retrieval / diff / confidence gate
- `security-acl.md`: ACL と block 条件
- `api-spec.md`: 外部 runtime API
- `eval-ops.md`: KPI と pilot exit criteria
- `runbook.md`: ingest / reindex / incident / rollback
- `samples/`: ingest / query / diff の dry-run

## Runtime Lanes
### Ingest Lane
`K0 -> K1 -> K2 -> K3 -> K4 -> K9`

- `K1` が file metadata と naming を正規化します。
- `K2` が taxonomy を付けます。
- `K3` が map / summary / chunk を作ります。
- `K4` が relation edge を作ります。

### Query / Diff Lane
`K0 -> K5 -> K7 or K6 -> K8 -> existing P* / P8 / P9`

- `K5` が retrieval plan を作ります。
- `K6` は diff 要求の時だけ動きます。
- `K7` が `artifact_context_packet` を作ります。
- `K8` が confidence, conflict, ACL を最終確認します。

## Confidence Gate
- `confidence < 0.65`: `K0` に戻し、`human_checkpoint_required`
- `0.65-0.78`: rewrite + relation expansion + second pass
- `>= 0.78`: downstream generation へ進行可

## Core Contracts
- `document_record`
- `section_node`
- `chunk_record`
- `relation_edge`
- `acl_principal_set`
- `ingest_job`
- `retrieval_plan`
- `evidence_bundle`
- `artifact_context_packet`
- `diff_report`
- `run_trace`

## Current Default
- storage: Azure Blob
- search: Azure AI Search
- trace / eval metadata: app-managed store
- front door: `docs/copilot-studio/custom-engine-bridge` の `C0-C5`
- orchestration: Azure-hosted custom engine

Source-backed generation の開始点は、このトラックで `artifact_context_packet` を作るところまでです。

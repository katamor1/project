<!-- docs/external-runtime/legacy-kb/architecture.md -->
<!-- Describes the logical component split between M365 Copilot, custom engine orchestration, and Azure knowledge services. -->
<!-- This exists so the external runtime stays thin at the edge and explicit in its retrieval and generation responsibilities. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/azure-topology.md, docs/external-runtime/legacy-kb/query-diff-flow.md, docs/sdlc/README.md -->
# Architecture

## Goal
Microsoft 365 Copilot を薄い入口として残し、重い retrieval / diff / context packing は外部 custom engine に逃がします。

## Responsibility Split
### Microsoft 365 Copilot
- ユーザー対話の入口
- tenant / identity の受け口
- 外部 runtime の起動要求

### Custom Engine Runtime
- `K0-K9` の orchestration
- retrieval planning
- diff execution
- `artifact_context_packet` の生成
- stop condition と `human_checkpoint_required` の判定

### Azure Knowledge Services
- raw file storage
- hybrid search index
- ACL-aware retrieval
- incremental reindex

## Component View
- `Copilot Entry`: request capture と auth context 受け渡し
- `Runtime Orchestrator`: `K0` 実行と lane 分岐
- `Ingest Worker`: `K1-K4`
- `Query Worker`: `K5-K8`
- `Azure Blob`: raw file と normalized artifact の保存
- `Azure AI Search`: `legacy_doc_map`, `legacy_leaf_chunks`, `legacy_relation_edges`
- `Trace Store`: `run_trace`, eval metadata, pilot notes

## Handoff Rules
- retrieval の最終成果物は `evidence_bundle`
- generation 側への handoff は `artifact_context_packet`
- `source_refs` の生配列だけでは downstream を開始しない
- low confidence, ACL mismatch, authoritative conflict は downstream に流さない

## Why This Split
- Copilot 側の knowledge 制約を外せます。
- retrieval と generation を別々に評価できます。
- ACL と authority を generation 前に再確認できます。
- `docs/sdlc` の `P*` レーンを壊さずに source-backed 化できます。

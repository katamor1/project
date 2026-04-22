<!-- docs/external-runtime/legacy-kb/architecture.md -->
<!-- Describes the logical component split between Claude Code, external runtime orchestration, and generic knowledge services. -->
<!-- This exists so the external runtime stays thin at the edge and explicit in its retrieval and generation responsibilities. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/runtime-topology.md, docs/external-runtime/legacy-kb/query-diff-flow.md, docs/sdlc/README.md -->
# Architecture

## Goal
Claude Code を薄い入口として残し、重い retrieval / diff / context packing は外部 runtime に寄せます。

## Responsibility Split
### Claude Code Entry
- ユーザー対話の入口
- session scope の受け口
- 外部 runtime の起動要求

### External Runtime
- `K0-K9` の orchestration
- retrieval planning
- diff execution
- `artifact_context_packet` の生成
- stop condition と `human_checkpoint_required` の判定

### Runtime Services
- raw file storage
- hybrid search index
- ACL-aware retrieval
- incremental reindex

## Component View
- `Claude Code Entry`: request capture と session scope 受け渡し
- `Runtime Orchestrator`: `K0` 実行と lane 分岐
- `Ingest Worker`: `K1-K4`
- `Query Worker`: `K5-K8`
- `Object Storage`: raw file と normalized artifact の保存
- `Hybrid Search`: `legacy_doc_map`, `legacy_leaf_chunks`, `legacy_relation_edges`
- `Trace Store`: `run_trace`, eval metadata, pilot notes

## Handoff Rules
- retrieval の最終成果物は `evidence_bundle`
- generation 側への handoff は `artifact_context_packet`
- `source_refs` の生配列だけでは downstream を開始しない
- low confidence, ACL mismatch, authoritative conflict は downstream に流さない

## Why This Split
- Claude Code 側の knowledge 制約を外せます。
- retrieval と generation を別々に評価できます。
- ACL と authority を generation 前に再確認できます。
- `docs/sdlc` の `P*` レーンを壊さずに source-backed 化できます。

<!-- docs/external-runtime/legacy-kb/runtime-topology.md -->
<!-- Describes the runtime-agnostic deployment topology for the external knowledge runtime. -->
<!-- This exists so storage, search, ACL, and trace placement are fixed before any service implementation starts. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/architecture.md, docs/external-runtime/legacy-kb/security-acl.md, docs/external-runtime/legacy-kb/api-spec.md -->
# Runtime Topology

## Fixed Services
- object storage: raw file と normalized export の保存
- hybrid search: hybrid index と ACL-aware retrieval
- runtime orchestrator: `K0-K9` orchestration
- app-managed trace store: `run_trace`, eval result, incident metadata

## Index Layout
### `legacy_doc_map`
- 文書単位と section summary 単位の map
- 主キー: `doc_id`, `section_id`

### `legacy_leaf_chunks`
- 葉チャンクの全文検索と vector 検索
- 主キー: `chunk_id`

### `legacy_relation_edges`
- screen, API, table, error, batch の relation
- 主キー: `edge_id`

## Stored Data
### Object Storage
- original Word / PDF / Excel
- normalized text export
- section tree export
- relation export

### Search Index
- metadata
- summary text
- chunk text
- embedding refs
- ACL filter fields

### Trace Store
- `run_id`
- `agent_path`
- `retrieved_doc_ids`
- confidence band
- latency
- token usage

## Network / Trust Assumptions
- Claude Code entry bridge から external runtime へ session scope を渡す
- external runtime から object storage / hybrid search へ private access を前提にする
- app-managed trace store は本文全文を長期保存しない

## Default Search Shape
- metadata pre-filter
- hybrid retrieve top 40
- rerank top 10
- leaf read max 6
- relation expansion は second pass の時だけ

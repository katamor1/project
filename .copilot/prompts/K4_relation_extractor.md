<!-- .copilot/prompts/K4_relation_extractor.md -->
<!-- Defines the relation extractor prompt for building screen, API, table, error, and batch edges from normalized legacy content. -->
<!-- This exists so multi-hop retrieval can expand related entities instead of relying on raw chunk similarity alone. -->
<!-- RELEVANT FILES: .copilot/schemas/relation-edge.schema.json, .copilot/prompts/K3_summary_tree_builder.md, docs/external-runtime/legacy-kb/query-diff-flow.md -->
# K4 Relation Extractor

## System
- あなたは relation 抽出担当です。
- screen, API, table, error, batch の関係を edge に落とします。
- relation graph は retrieval expansion 用です。

## User
- 入力は `section_nodes`, `chunk_records`, `taxonomy_bundle` です。
- edge は `source_entity`, `target_entity`, `relation_type`, `source_refs` を必ず持たせてください。
- 根拠が弱い edge は `low_confidence_edges` に残してください。

## Assistant
- `artifact_type` は `relation_edge_bundle` 固定です。
- `required_inputs` は `section_nodes`, `chunk_records`, `taxonomy_bundle` です。
- `next_agent` は `K9_runtime_eval_monitor` 固定です。
- `human_checkpoint` は大量 conflict 時だけ `required` にできます。
- `done_definition` は「usable relation edges と low confidence edges が分かれる」です。
- 出力は JSON で返してください。

## JSON Output Example
```json
{
  "prompt_id": "K4_relation_extractor",
  "prompt_version": "1.0",
  "artifact_type": "relation_edge_bundle",
  "required_inputs": ["section_nodes", "chunk_records", "taxonomy_bundle"],
  "human_checkpoint": "none",
  "done_definition": [
    "relation edges exist",
    "source refs exist",
    "low confidence edges are explicit",
    "next agent is explicit"
  ],
  "next_agent": "K9_runtime_eval_monitor",
  "relation_edge_bundle": {
    "relation_edges": ["EDGE-EMAIL-UI-API"],
    "low_confidence_edges": [],
    "next_agent": "K9_runtime_eval_monitor"
  }
}
```

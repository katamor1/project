<!-- docs/external-runtime/legacy-kb/data-pipeline.md -->
<!-- Describes the ingest pipeline that turns legacy files into searchable map, chunk, and graph artifacts. -->
<!-- This exists so Word, PDF, and Excel sources are normalized the same way before retrieval begins. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/query-diff-flow.md, .copilot/schemas/document-record.schema.json, .copilot/schemas/chunk-record.schema.json -->
# Data Pipeline

## Pipeline
1. collect
2. normalize
3. map
4. chunk
5. summary tree
6. relation graph
7. index
8. incremental reindex

## Step Details
### Collect
- SharePoint / OneDrive / file export から raw file を受け取る
- file hash と source metadata を保存する

### Normalize
- Word / PDF / Excel を表示順つき text に変換する
- page, sheet, heading path を保持する
- `document_record` を作る

### Map
- chapter / section の `section_node` を作る
- document map と summary tree の親子関係を作る

### Chunk
- heading-boundary 優先で `chunk_record` を作る
- Excel は sheet ではなく logical table 単位で切る
- `authority_score` と `page_or_sheet` を保持する

### Summary Tree
- section summary
- chapter summary
- document summary

### Relation Graph
- screen -> API
- API -> table
- API -> error
- batch -> table

## Incremental Reindex Rule
- file hash が変わらなければ再処理しない
- section hash が変わった部分だけ `section_node` と `chunk_record` を差し替える
- relation edge は changed section に接続したものだけ再計算する

## Required Outputs
- `document_record`
- `section_node`
- `chunk_record`
- `relation_edge`
- `ingest_job`

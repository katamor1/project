<!-- docs/external-runtime/legacy-kb/runbook.md -->
<!-- Provides the operating runbook for ingest, reindex, incident handling, and rollback in the external knowledge runtime. -->
<!-- This exists so the future runtime can be operated safely before automation and IaC are added. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/api-spec.md, docs/external-runtime/legacy-kb/eval-ops.md, docs/external-runtime/legacy-kb/security-acl.md -->
# Runbook

## Ingest
1. source metadata を確認する
2. `/ingest/files` を実行する
3. `ingest_job` の status を確認する
4. `document_record`, `section_node`, `chunk_record`, `relation_edge` の件数を確認する

## Reindex
1. change source を確認する
2. `/ingest/reindex` を changed path だけで実行する
3. partial update になっているか確認する
4. affected query set を replay する

## Incident
### Low Confidence Spike
- query rewrite の失敗率を見る
- metadata filter と synonyms を確認する
- authoritative source の欠落を確認する

### ACL Incident
- `run_id` から `retrieved_doc_ids` を確認する
- principal set と index filter を見直す
- downstream handoff が起きていれば即 stop する

## Rollback
- prompt version rollback
- index snapshot rollback
- source version rollback
- pilot gate を通るまで downstream handoff を止める

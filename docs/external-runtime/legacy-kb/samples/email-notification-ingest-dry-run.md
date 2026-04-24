<!-- docs/external-runtime/legacy-kb/samples/email-notification-ingest-dry-run.md -->
<!-- Shows a minimal ingest dry run for the email notification legacy document set. -->
<!-- This exists so the team can verify how raw files become map, chunk, relation, and ingest job artifacts. -->
<!-- RELEVANT FILES: .copilot/prompts/runtime/ibmbob-intake-normalizer.md, .copilot/schemas/document-record.schema.json, .copilot/schemas/ingest-job.schema.json -->
# Email Notification Ingest Dry Run

## document_record
```json
{
  "doc_id": "DOC-EMAIL-NOTIFY-SPEC-001",
  "version_id": "v2026-04-23",
  "source_path": "sharepoint://legacy/notification/email-setting-spec.docx",
  "doc_type": "functional_spec",
  "module": "notification_settings",
  "business_domain": "account",
  "approval_status": "approved",
  "acl": {
    "users": [],
    "groups": ["app-pm", "app-design"],
    "tenant": "contoso"
  },
  "hash": "sha256:example"
}
```

## section_node
```json
{
  "section_id": "SEC-EMAIL-NOTIFY-UI",
  "doc_id": "DOC-EMAIL-NOTIFY-SPEC-001",
  "parent_id": "ROOT-EMAIL-NOTIFY",
  "heading_path": ["通知設定", "メール通知 UI"],
  "section_level": 2,
  "page_or_sheet": "p.4",
  "summary": "ユーザーがメール通知を ON/OFF 変更する画面仕様",
  "child_ids": ["CHUNK-EMAIL-NOTIFY-UI-001"]
}
```

## chunk_record
```json
{
  "chunk_id": "CHUNK-EMAIL-NOTIFY-UI-001",
  "doc_id": "DOC-EMAIL-NOTIFY-SPEC-001",
  "parent_id": "SEC-EMAIL-NOTIFY-UI",
  "heading_path": ["通知設定", "メール通知 UI"],
  "page_or_sheet": "p.4",
  "authority_score": 0.92,
  "embedding_model": "text-embedding-3-large",
  "hash": "sha256:chunk-example"
}
```

## relation_edge
```json
{
  "edge_id": "EDGE-EMAIL-UI-API",
  "source_entity": "screen:notification_settings",
  "target_entity": "api:patch_notification_setting",
  "relation_type": "calls",
  "source_refs": ["DOC-EMAIL-NOTIFY-SPEC-001#p.4"],
  "confidence": 0.88
}
```

## ingest_job
```json
{
  "job_id": "INGEST-EMAIL-NOTIFY-001",
  "status": "completed",
  "input_files": ["email-setting-spec.docx"],
  "created_document_ids": ["DOC-EMAIL-NOTIFY-SPEC-001"],
  "reindex_mode": "full",
  "next_agent": "ibmbob-taxonomy-mapper"
}
```


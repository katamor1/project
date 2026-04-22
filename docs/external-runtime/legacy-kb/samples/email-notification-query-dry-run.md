<!-- docs/external-runtime/legacy-kb/samples/email-notification-query-dry-run.md -->
<!-- Shows a minimal query dry run from retrieval planning to artifact context packing. -->
<!-- This exists so the team can verify how evidence becomes a downstream SDLC handoff packet. -->
<!-- RELEVANT FILES: .copilot/prompts/K5_retrieval_planner.md, .copilot/schemas/evidence-bundle.schema.json, .copilot/schemas/artifact-context-packet.schema.json -->
# Email Notification Query Dry Run

## retrieval_plan
```json
{
  "task_id": "TASK-BASIC-EMAIL-NOTIFY-001",
  "query": "メール通知設定変更の旧仕様を集める",
  "metadata_filters": {
    "module": "notification_settings",
    "approval_status": "approved"
  },
  "retrieve_top_k": 40,
  "rerank_top_k": 10,
  "leaf_read_max": 6,
  "fallback_strategy": ["query_rewrite", "relation_expand"],
  "next_agent": "K7_artifact_context_packer"
}
```

## evidence_bundle
```json
{
  "task_id": "TASK-BASIC-EMAIL-NOTIFY-001",
  "query_plan": "metadata -> hybrid -> rerank -> leaf read",
  "retrieved_items": [
    {
      "doc_id": "DOC-EMAIL-NOTIFY-SPEC-001",
      "chunk_id": "CHUNK-EMAIL-NOTIFY-UI-001",
      "authority_score": 0.92
    }
  ],
  "coverage_assessment": "UI, API, DB の主要観点をカバー",
  "confidence": 0.84,
  "risk_flags": [],
  "next_agent": "K7_artifact_context_packer"
}
```

## artifact_context_packet
```json
{
  "artifact_request": {
    "artifact_type": "basic_design",
    "target_agent": "P2_basic_design_author"
  },
  "evidence_bundle_ref": "EVB-BASIC-EMAIL-NOTIFY-001",
  "resolved_entities": [
    "screen:notification_settings",
    "api:patch_notification_setting",
    "table:user_notification_settings"
  ],
  "open_questions": [],
  "allowed_assertions": [
    "通知設定画面の主要 UI",
    "更新 API の存在",
    "保存先テーブル"
  ],
  "evidence_coverage": "approved legacy sources cover core scope",
  "authority_conflict_note": "no conflict",
  "next_agent": "P2_basic_design_author"
}
```

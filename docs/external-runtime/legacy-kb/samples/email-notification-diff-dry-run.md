<!-- docs/external-runtime/legacy-kb/samples/email-notification-diff-dry-run.md -->
<!-- Shows a minimal diff dry run from evidence retrieval to diff classification and gate review. -->
<!-- This exists so the team can verify how legacy comparison is grounded before downstream review or generation. -->
<!-- RELEVANT FILES: .copilot/prompts/runtime/ibmbob-legacy-diff-checker.md, .copilot/schemas/diff-report.schema.json, .copilot/prompts/runtime/ibmbob-evidence-reviewer.md -->
# Email Notification Diff Dry Run

## retrieval_plan
```json
{
  "task_id": "TASK-DIFF-EMAIL-NOTIFY-001",
  "query": "メール通知設定の旧仕様と新要求の差分確認",
  "metadata_filters": {
    "module": "notification_settings",
    "approval_status": "approved"
  },
  "retrieve_top_k": 40,
  "rerank_top_k": 10,
  "leaf_read_max": 6,
  "fallback_strategy": ["query_rewrite", "relation_expand"],
  "next_agent": "ibmbob-legacy-diff-checker"
}
```

## diff_report
```json
{
  "diff_items": [
    {
      "area": "UI",
      "summary": "旧仕様は email のみ、新要求は digest frequency を追加",
      "source_refs": ["DOC-EMAIL-NOTIFY-SPEC-001#p.4"]
    }
  ],
  "carry_over_items": [
    {
      "area": "API",
      "summary": "PATCH で設定更新する構造は踏襲",
      "source_refs": ["DOC-EMAIL-NOTIFY-SPEC-001#p.6"]
    }
  ],
  "unknown_items": [
    {
      "area": "Ops",
      "summary": "配信ジョブの再実行条件は旧文書から判別不能"
    }
  ],
  "impact_estimate": "medium",
  "next_agent": "ibmbob-evidence-reviewer"
}
```

## evidence_review
```json
{
  "decision": "pass",
  "evidence_coverage": "UI と API は十分、ops は unknown を維持",
  "authority_conflict_note": "no conflict",
  "human_checkpoint": false,
  "next_agent": "ibmbob-sdlc-review-record"
}
```


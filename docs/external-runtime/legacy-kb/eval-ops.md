<!-- docs/external-runtime/legacy-kb/eval-ops.md -->
<!-- Defines runtime evaluation, pilot metrics, and operational quality gates for the external knowledge track. -->
<!-- This exists so retrieval and diff quality can be judged independently from downstream SDLC generation quality. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/runbook.md, docs/external-runtime/legacy-kb/samples/email-notification-query-dry-run.md, .copilot/prompts/K9_runtime_eval_monitor.md -->
# Eval And Ops

## KPI
- `Recall@10`
- `authoritative_source_hit_rate`
- `route_ambiguity`
- `ACL_leak`
- `low_confidence_abort_rate`
- `unsupported_claim_rate`
- `p95_latency_ms`
- `cache_hit_ratio`

## Pilot Exit Criteria
- `Recall@10 >= 0.90`
- `authoritative_source_hit_rate >= 0.85`
- `route_ambiguity = 0`
- `ACL leak = 0`
- 日本語 summary / diff / review note が pilot で読める

## Required Evaluation Cases
- Word / PDF / Excel の 3 種が同じ ingest contract に落ちる
- exact match と自然文 query の両方で relevant chunk が取れる
- conflict source は downstream に流れない
- partial reindex だけで changed section が更新される
- `artifact_context_packet` から `P2` / `P4` / `P6` が開始できる

## Operational Notes
- ingest, query, diff は別メトリクスで取る
- query failure は retrieval 失敗と ACL block を分けて記録する
- eval は runtime 側と `docs/sdlc` 側で分けて集計する

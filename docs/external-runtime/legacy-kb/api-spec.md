<!-- docs/external-runtime/legacy-kb/api-spec.md -->
<!-- Defines the external runtime API surface for ingest, retrieval, diff, context packing, and eval replay. -->
<!-- This exists so future service implementation can follow one fixed contract behind the Claude Code runtime bridge. -->
<!-- RELEVANT FILES: docs/claude-code/runtime-bridge/dispatch-contract.md, docs/external-runtime/legacy-kb/runbook.md, .copilot/schemas/artifact-context-packet.schema.json -->
# API Spec

## Front Door
- Claude Code 側の入口は `docs/claude-code/runtime-bridge` です。
- `entry_request_packet -> entry_dispatch_packet -> runtime_request` の変換は bridge 側で行います。
- この API は external runtime の内側 contract です。

## Endpoints
| Method | Path | Purpose | Main Output |
|---|---|---|---|
| `POST` | `/ingest/files` | raw file 登録 | `ingest_job` |
| `POST` | `/ingest/reindex` | 差分再索引 | `ingest_job` |
| `POST` | `/kb/search` | metadata + hybrid 検索 | `evidence_bundle` |
| `POST` | `/kb/read` | chunk 本文取得 | `chunk_record[]` |
| `POST` | `/kb/graph-expand` | relation 近傍展開 | `relation_edge[]` |
| `POST` | `/diff/check` | 旧新差分 | `diff_report` |
| `POST` | `/context/pack` | evidence を P* 用 packet へ変換 | `artifact_context_packet` |
| `GET` | `/run/{id}` | trace 取得 | `run_trace` |
| `POST` | `/eval/replay` | eval 再実行 | eval result summary |

## Request Shape Notes
- `/ingest/files`: file metadata, source path, source type, owner
- `/kb/search`: `task_id`, query, metadata filters, principal set
- `/diff/check`: new artifact ref, old evidence bundle ref, diff mode
- `/context/pack`: artifact request, evidence bundle ref, downstream target
- `runtime_request` は `entry_dispatch_packet` の `runtime_target` と `runtime_task_type` から導出します

## Response Shape Notes
- `/kb/search` は `confidence` と `risk_flags` を必ず返す
- `/diff/check` は `change`, `carry_over`, `unknown` を必ず返す
- `/context/pack` は `allowed_assertions` と `open_questions` を必ず返す
- `/run/{id}` は `agent_path`, `tool_path`, `latency_ms`, `token_usage` を返す

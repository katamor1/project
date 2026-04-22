<!-- docs/claude-code/runtime-bridge/README.md -->
<!-- Explains the Claude Code entry bridge that hands chat requests into the external legacy KB runtime and back. -->
<!-- This exists so Claude Code can stay a thin entry while the external runtime does heavy retrieval and diff work. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/README.md, .copilot/routing/entry-bridge-matrix.yaml, docs/sdlc/README.md -->
# Claude Code Runtime Bridge Pack

このトラックは、`Claude Code -> external runtime -> existing P* lane` の入口設計です。

主経路は `entry bridge` です。

`MCP / plugin / OpenAPI action` は補助経路として文書化だけ行います。

## Goal
- chat request を `entry_request_packet` に正規化する
- `session_scope_context` を先に確認する
- `entry_dispatch_packet` を作って `K0_runtime_orchestrator` に渡す
- runtime や `P*` の結果を、Claude Code 向けの短い日本語応答へ戻す

## Standard Flow
`C0 -> C1 -> C2 -> C3 -> K0 -> C4 -> C5`

### Query / Diff Branch
- `legacy diff`: `C3 -> K0 -> K6 -> K8 -> C4`
- `source-backed generation`: `C3 -> K0 -> K5 -> K7 -> K8 -> P* -> C4`

## Core Contracts
- `entry_request_packet`
- `session_scope_context`
- `entry_dispatch_packet`
- `entry_response`
- `entry_human_checkpoint`
- `entry_trace`

## Gate Rules
- `session_scope_missing`
- `workspace_target_mismatch`
- `allowed_target_missing`

この 3 つは `C2_session_scope_guard` で止めます。

- `confidence < 0.65`
- `ACL mismatch`
- `authoritative conflict`

この 3 つは `K0/K8` から `C4_entry_response_shaper` に戻し、Claude Code 側では `abstain + next action` を返します。

## Connection
- bridge の先は `docs/external-runtime/legacy-kb` です。
- source-backed generation の先は `docs/sdlc` の `P*` レーンです。
- review / eval 要求は `P8_review_record`, `P9_eval_monitor` に戻せます。

## Contents
- `entry-flow.md`
- `identity-scope.md`
- `dispatch-contract.md`
- `response-shaping.md`
- `fallback-policy.md`
- `pilot-runbook.md`
- `samples/`

<!-- docs/copilot-studio/custom-engine-bridge/README.md -->
<!-- Explains the Copilot-side bridge that hands chat requests into the external legacy KB runtime and back. -->
<!-- This exists so Microsoft 365 Copilot can stay a thin entry while the external runtime does heavy retrieval and diff work. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/README.md, .copilot/routing/custom-engine-bridge-matrix.yaml, docs/sdlc/README.md -->
# Copilot Custom Engine Bridge Pack

このトラックは、`Microsoft 365 Copilot -> external runtime -> existing P* lane` の入口設計です。

主経路は `Custom Engine` です。

`MCP / plugin / OpenAPI action` は補助経路として文書化だけ行います。

## Goal
- chat request を `bridge_request_packet` に正規化する
- identity / tenant / scope を先に確認する
- `bridge_dispatch_packet` を作って `K0_runtime_orchestrator` に渡す
- runtime や `P*` の結果を、Copilot 向けの短い日本語応答へ戻す

## Standard Flow
`C0 -> C1 -> C2 -> C3 -> K0 -> C4 -> C5`

### Query / Diff Branch
- `legacy diff`: `C3 -> K0 -> K6 -> K8 -> C4`
- `source-backed generation`: `C3 -> K0 -> K5 -> K7 -> K8 -> P* -> C4`

## Core Contracts
- `bridge_request_packet`
- `bridge_identity_context`
- `bridge_dispatch_packet`
- `bridge_response`
- `bridge_human_checkpoint`
- `bridge_trace`

## Gate Rules
- `identity missing`
- `tenant mismatch`
- `scope missing`

この 3 つは `C2_identity_scope_guard` で止めます。

- `confidence < 0.65`
- `ACL mismatch`
- `authoritative conflict`

この 3 つは `K0/K8` から `C4_bridge_response_shaper` に戻し、Copilot 側では `abstain + next action` を返します。

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

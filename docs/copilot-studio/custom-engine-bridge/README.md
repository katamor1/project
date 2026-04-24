<!-- docs/copilot-studio/custom-engine-bridge/README.md -->
<!-- Explains the Copilot-side bridge that hands chat requests into the external legacy KB runtime and back. -->
<!-- This exists so Microsoft 365 entry can stay a thin entry while the external runtime does heavy retrieval and diff work. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/README.md, .copilot/routing/entry/ibmbob-entry-flow.yaml, docs/sdlc/README.md -->
# entry Custom Engine Bridge Pack

このトラックは、`Microsoft 365 entry -> external runtime -> existing P* lane` の入口設計です。

主経路は `Custom Engine` です。

`MCP / plugin / OpenAPI action` は補助経路として文書化だけ行います。

## Goal
- chat request を `entry_request_packet` に正規化する
- identity / tenant / scope を先に確認する
- `entry_dispatch_packet` を作って `ibmbob-runtime-orchestrator` に渡す
- runtime や `P*` の結果を、entry 向けの短い日本語応答へ戻す

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
- `identity missing`
- `tenant mismatch`
- `scope missing`

この 3 つは `ibmbob-identity-scope-guard` で止めます。

- `confidence < 0.65`
- `ACL mismatch`
- `authoritative conflict`

この 3 つは `K0/K8` から `ibmbob-entry-response-shaper` に戻し、entry 側では `abstain + next action` を返します。

## Connection
- bridge の先は `docs/external-runtime/legacy-kb` です。
- source-backed generation の先は `docs/sdlc` の `P*` レーンです。
- review / eval 要求は `ibmbob-sdlc-review-record`, `ibmbob-sdlc-eval-monitor` に戻せます。

## Contents
- `entry-flow.md`
- `identity-scope.md`
- `dispatch-contract.md`
- `response-shaping.md`
- `fallback-policy.md`
- `pilot-runbook.md`
- `samples/`



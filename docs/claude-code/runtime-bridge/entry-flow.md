<!-- docs/claude-code/runtime-bridge/entry-flow.md -->
<!-- Describes the step-by-step Claude Code entry flow before and after the external runtime runs. -->
<!-- This exists so entry routing stays explicit and thin before any registration work is added. -->
<!-- RELEVANT FILES: .copilot/routing/entry-bridge-matrix.yaml, docs/claude-code/runtime-bridge/dispatch-contract.md, docs/claude-code/runtime-bridge/response-shaping.md -->
# Entry Flow

## Flow
1. `C0_entry_router`
2. `C1_intent_packet_builder`
3. `C2_session_scope_guard`
4. `C3_dispatch_packet_author`
5. `K0_runtime_orchestrator`
6. `K*` または `P*`
7. `C4_entry_response_shaper`
8. `C5_entry_eval_monitor`

## What Each Step Does
### `C0`
- request type を判定する
- `generation`, `diff`, `review`, `eval`, `checkpoint` を分ける

### `C1`
- chat request を `entry_request_packet` にする

### `C2`
- workspace, repo, connector scope, allowed target を確認する
- NG なら `human_checkpoint_required`

### `C3`
- `entry_dispatch_packet` を作る
- runtime target と callback contract を固定する

### `K0-K8`
- retrieval, diff, evidence gate を実行する

### `C4`
- result を Claude Code 向け日本語応答に整形する
- `answer`, `abstain`, `checkpoint`, `handoff` を分ける

### `C5`
- bridge trace と checkpoint 品質を記録する

## Stop Points
- `C2`: workspace / repo / connector scope / allowed target
- `K0/K8`: confidence, ACL, authority conflict
- `C4`: user-facing abstain / checkpoint shaping

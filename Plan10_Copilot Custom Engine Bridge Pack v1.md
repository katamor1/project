# Copilot Custom Engine Bridge Pack v1

## Summary
- 次フェーズでは、`docs/claude-code` 側に `custom engine bridge` トラックを追加し、`Microsoft 365 Copilot -> external runtime -> existing P* lane` の入口設計を repo 資産として固める。
- 主軸は `Custom Engine` に固定する。`MCP / plugin / OpenAPI action` は補助経路として文書化だけ行い、主実装トラックにはしない。
- 今回は `Repo Assets` に限定する。追加するのは docs、schemas、prompts、routing、samples、pilot/runbook までで、実サービスコードや Copilot registration 実装は含めない。
- bridge の標準フローは `C0 -> C1 -> C2 -> C3 -> K0 -> K8 -> C4 -> C5` に固定し、Copilot 側は薄い入口、runtime 側は既存 `K*` を再利用する。

## Key Changes
- `docs/claude-code/runtime-bridge/` を新設する。
- `README.md`: 全体像と `docs/external-runtime/legacy-kb` / `docs/sdlc` との接続
- `entry-flow.md`: Copilot 入口から runtime dispatch までの段階
- `identity-scope.md`: user principal, group, tenant, allowed scopes の扱い
- `dispatch-contract.md`: `entry_request_packet -> entry_dispatch_packet -> runtime_request` の変換
- `response-shaping.md`: runtime 結果を Copilot 向け応答へ戻す整形ルール
- `fallback-policy.md`: custom engine 主軸、MCP / plugin は補助であることの整理
- `pilot-runbook.md`: pilot 実施順、観測ポイント、block 条件
- `samples/`: `email-notification-entry-dry-run.md`, `email-notification-diff-dry-run.md`, `email-notification-abstain-dry-run.md`

- `.copilot/prompts/` に bridge 用の `C*` prompt を追加する。
- `C0_entry_router`: Copilot 入口で request type を振り分ける
- `C1_intent_packet_builder`: chat request を `entry_request_packet` に正規化する
- `C2_session_scope_guard`: principal / tenant / allowed scope を確認する
- `C3_dispatch_packet_author`: `entry_dispatch_packet` を作り `K0_runtime_orchestrator` に渡す
- `C4_entry_response_shaper`: `K*` / `P*` 側の結果を Copilot 向け日本語応答に整形する
- `C5_entry_eval_monitor`: bridge 特有の routing / checkpoint / abstain 品質を監視する

- `.copilot/routing/` に `custom-engine-bridge-matrix.yaml` を追加する。
- 標準ルートは `C0 -> C1 -> C2 -> C3 -> K0 -> C4 -> C5`
- `legacy diff` 要求は `C3 -> K0 -> K6 -> K8 -> C4`
- `source-backed generation` 要求は `C3 -> K0 -> K5 -> K7 -> K8 -> P* -> C4`
- `identity missing`, `tenant mismatch`, `scope missing` は `C2` で止めて `human_checkpoint_required`
- `confidence < 0.65`, ACL mismatch, authoritative conflict は `K0/K8` から `C4` に戻し、Copilot 側では `abstain + next action` を返す
- `0.65-0.78` は `K5` の second pass を待ち、直接 `P*` へは流さない

- `.copilot/schemas/` に bridge contract を追加する。
- `bridge-request-packet.schema.json`
- `bridge-identity-context.schema.json`
- `bridge-dispatch-packet.schema.json`
- `bridge-response.schema.json`
- `bridge-human-checkpoint.schema.json`
- `bridge-trace.schema.json`
- 重要フィールドは固定する。
- `entry_request_packet`: `request_id`, `user_intent`, `requested_outcome`, `requested_lane`, `locale`, `session_context_ref`
- `session_scope_context`: `user_principal`, `groups`, `tenant`, `execution_surface`, `allowed_scopes`
- `entry_dispatch_packet`: `entry_request_ref`, `session_scope_ref`, `runtime_target`, `runtime_task_type`, `callback_contract`, `next_agent`
- `entry_response`: `response_mode`, `user_message`, `downstream_target`, `handoff_state`, `human_checkpoint`, `citations_policy`
- `entry_human_checkpoint`: `reason`, `required_user_action`, `blocked_target`, `retry_entry`
- `entry_trace`: `request_id`, `execution_surface`, `bridge_path`, `runtime_run_ref`, `checkpoint_flags`, `latency_ms`

- 既存トラックとの接続を明文化する。
- `K0_runtime_orchestrator` の入力説明を `runtime_request` 単体から `entry_dispatch_packet + runtime_request` 前提に広げる
- `K8_evidence_reviewer` は Copilot 側へ戻す `gate_result` を持てる前提にする
- `docs/external-runtime/legacy-kb/README.md` と `api-spec.md` は front door として bridge を参照するよう更新する
- `docs/sdlc/README.md` は source-backed generation の開始点を `Copilot bridge -> K* -> P*` に更新する
- 新規 sample は `source_refs` 直渡しではなく、必ず `entry_request_packet` と `artifact_context_packet` の両方を含める

## Test Plan
- 静的確認を行う。
- 新規 `C*` prompt が既存と同じ `System / User / Assistant / JSON Output Example` 構成を持つこと
- 新規 bridge schema が 4 行 header を除けば parse できること
- 新規 Markdown / JSON / YAML がすべて 300 行未満であること

- bridge flow の評価ケースを固定する。
- design 要求が `C0 -> C3 -> K0 -> K7 -> P2_basic_design_author` に流れる
- diff 要求が `C0 -> C3 -> K0 -> K6 -> K8 -> C4` に流れる
- review / eval 要求が `P8 / P9` へ正しく振り分けられる
- `identity missing`, `tenant mismatch`, `scope missing` は `C2` で止まる
- `low confidence`, `ACL mismatch`, `authoritative conflict` は `C4` で abstain 応答になる
- `artifact_context_packet` がある時だけ downstream `P*` が開始される

- 応答品質の評価ケースを固定する。
- Copilot 側応答が日本語で短く、`何が分かったか / 何が足りないか / 次に何をするか` を返す
- `human_checkpoint_required` の時に required action が具体的である
- diff 応答で `change / carry_over / unknown` が崩れない
- bridge trace から request, runtime run, checkpoint が追える

## Assumptions
- 主経路は `Custom Engine` 固定とし、MCP / plugin / OpenAPI action は fallback documentation のみにとどめる。
- 今回は repo 資産のみを追加し、Copilot registration、manifest、実サービスコード、connector 実装、IaC、CI は追加しない。
- 既存の 3 トラック構成 `docs/claude-code`, `docs/external-runtime`, `docs/sdlc` は維持し、bridge は `docs/claude-code` 配下に置く。
- bridge は legacy 文書活用専用の入口とし、現段階では app repo 実装や deployment automation には接続しない。

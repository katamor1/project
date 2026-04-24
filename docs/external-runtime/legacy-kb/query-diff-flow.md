<!-- docs/external-runtime/legacy-kb/query-diff-flow.md -->
<!-- Describes the staged retrieval and diff flow that gates downstream SDLC generation. -->
<!-- This exists so evidence quality is checked before any design or implementation artifact is produced. -->
<!-- RELEVANT FILES: .copilot/routing/runtime/ibmbob-runtime-query-flow.yaml, .copilot/schemas/evidence-bundle.schema.json, .copilot/schemas/diff-report.schema.json -->
# Query And Diff Flow

## Retrieval Flow
1. `K0` が task type を判定する
2. `K5` が metadata filter を作る
3. hybrid retrieve を実行する
4. rerank する
5. 必要箇所だけ leaf read する
6. `evidence_bundle` を作る
7. `K8` が confidence / ACL / conflict を再確認する

## Confidence Bands
- `< 0.65`: stop, `human_checkpoint_required`
- `0.65-0.78`: query rewrite, relation expansion, second pass
- `>= 0.78`: downstream generation へ handoff

## Diff Flow
1. `K5` が旧仕様の evidence を集める
2. `K6` が new draft と old evidence を比較する
3. `change / carry_over / unknown` に分ける
4. `impact_estimate` を返す
5. `K8` が authoritative conflict を再確認する

## Downstream Handoff
- 通常生成: `K7 -> artifact_context_packet -> P2 / P4 / P6`
- diff 要求: `K6 -> diff_report -> K8 -> P8 / P9 or human checkpoint`

## Mandatory Block Conditions
- conflicting authoritative sources
- ACL mismatch
- unsupported claim risk
- evidence bundle が acceptance threshold 未満


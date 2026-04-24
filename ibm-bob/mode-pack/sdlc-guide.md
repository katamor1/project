<!-- ibm-bob/mode-pack/sdlc-guide.md -->
<!-- Explains how to operate the P custom modes inside IBM Bob across design, test spec, implementation, execution, and integration lanes. -->
<!-- This exists so the team can use Bob custom modes as a thin overlay on top of the canonical SDLC design assets. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, docs/sdlc/README.md, ibm-bob/mode-pack/handoff-contracts.md -->
# SDLC Guide

## Design Entry

純粋な設計依頼は `p0-orchestrator` から始めます。

legacy 文書を根拠に使う時は、先に entry bridge と runtime を通します。

つまり開始点は 2 種です。

- 非 source-backed: `p0-orchestrator`
- source-backed: `c0-entry-router`

## Upper Design Lane

`p0 -> p1 -> p2-basic -> p3 -> p2-detail -> p3 -> p8 -> p9`

`p3` は `pass | revise | block` を返します。

## Test Spec Lane

`p3(detail pass) -> p2-functional or p2-integration -> p3 -> p8 -> p9`

## Implementation Lane

`p3(detail pass) -> p4 -> p5 -> p6 -> p7 -> p8 -> p9`

`p4` と `p6` は source-backed の時に `artifact_context_packet` を優先します。

## Execution Pack Lane

`p7(pass) or repo_bridge_ready -> p10 -> p11 -> p12 -> p13 -> p14 -> p8 -> p9`

## Integration Lane

`p14(pass) or execution_bundle_ready -> p15 -> p16 -> p17 -> p18 -> p19 -> p20 -> p21 -> p8 -> p9`

## Review / Eval Requests

review だけ欲しい時は `c0-entry-router` から入り、必要なら `p8-review-record` に流します。

eval だけ欲しい時は `c0-entry-router` から入り、必要なら `p9-eval-monitor` に流します。

## Source-Backed Rule

`artifact_context_packet` が無い時は、`p2`, `p4`, `p6` の source-backed generation を開始しません。

この時は entry bridge 側に戻して evidence を先にそろえます。

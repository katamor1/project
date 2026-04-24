<!-- .bob/rules/03-handoff-discipline.md -->
<!-- Defines the cross-cutting handoff discipline for Bob custom modes. -->
<!-- This exists so every mode keeps routing, checkpoints, and handoff state explicit. -->
<!-- RELEVANT FILES: .copilot/routing/custom-engine-bridge-matrix.yaml, docs/copilot-studio/legacy-search/shared-io-contract.yaml, ibm-bob/mode-pack/routing-map.md -->
handoff を省略しないでください。

次の mode がある時は `next_agent` を明示してください。

legacy-search では `handoff_packet` を必ず返してください。

block が必要な時は `human_checkpoint` を明示してください。

曖昧なまま downstream に流さないでください。

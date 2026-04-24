<!-- .bob/rules/02-source-backed-generation.md -->
<!-- Defines the cross-cutting rule for source-backed generation in Bob modes. -->
<!-- This exists so evidence-bound work always prefers bounded context packets over raw browsing. -->
<!-- RELEVANT FILES: .copilot/schemas/artifact-context-packet.schema.json, .copilot/prompts/K7_artifact_context_packer.md, ibm-bob/mode-pack/handoff-contracts.md -->
source-backed generation では `artifact_context_packet` を最優先で使ってください。

`artifact_context_packet` がある時は、raw source の再探索を始めないでください。

`source_refs` は fallback です。

根拠が弱い、競合している、または不足している時は abstain してください。

unsupported claim は出さないでください。

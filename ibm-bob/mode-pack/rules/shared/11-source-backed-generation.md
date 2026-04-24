<!-- ibm-bob/mode-pack/rules/shared/11-source-backed-generation.md -->
<!-- Defines the shared source-backed generation rule for the direct-mode IBM Bob family. -->
<!-- This exists so direct-mode SDLC work starts from bounded evidence instead of raw source browsing. -->
<!-- RELEVANT FILES: .copilot/schemas/artifact-context-packet.schema.json, ibm-bob/mode-pack/handoff-contracts.md, ibm-bob/mode-pack/sdlc-guide.md -->
source-backed generation では `artifact_context_packet` を優先する。

`source_refs` は fallback としてだけ使う。

根拠が弱い、競合している、または不足している時は abstain する。

unsupported claim は出さない。

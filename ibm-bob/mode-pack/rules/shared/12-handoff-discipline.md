<!-- ibm-bob/mode-pack/rules/shared/12-handoff-discipline.md -->
<!-- Defines the shared handoff discipline for the direct-mode IBM Bob family. -->
<!-- This exists so routing, checkpoint, and packet handoff stay explicit across the direct-mode family. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/legacy-search-guide.md, ibm-bob/mode-pack/sdlc-guide.md -->
次の mode がある時は `next_agent` を明示する。

legacy-search では `handoff_packet` を省略しない。

block が必要な時は `human_checkpoint` を明示する。

曖昧なまま downstream に流さない。

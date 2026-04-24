<!-- .bob/rules/04-command-safety.md -->
<!-- Defines the cross-cutting command safety rule for Bob power-user custom modes. -->
<!-- This exists so broad command access does not bypass human approval for risky actions. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, ibm-bob/mode-pack/tool-profile.md, ibm-bob/mode-pack/pilot-runbook.md -->
destructive command は human checkpoint なしで実行しないでください。

特に次は必ず止めてください。

- recursive delete
- forced checkout or reset
- bulk move
- production-like deploy
- credential or permission changes

command を使う前に、対象と目的を短く明示してください。

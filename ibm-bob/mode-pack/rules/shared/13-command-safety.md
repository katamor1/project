<!-- ibm-bob/mode-pack/rules/shared/13-command-safety.md -->
<!-- Defines the shared command safety rule for the direct-mode IBM Bob family. -->
<!-- This exists so broad command access does not bypass human review for destructive actions. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/tool-profile.md, ibm-bob/mode-pack/pilot-runbook.md -->
destructive command は human checkpoint なしで実行しない。

recursive delete, forced checkout or reset, bulk move, deploy, permission change は止める。

command を使う前に、対象と目的を短く明示する。

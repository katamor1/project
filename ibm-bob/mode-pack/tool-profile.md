<!-- ibm-bob/mode-pack/tool-profile.md -->
<!-- Defines the permission policy that applies across the old orchestrator family and the new direct-mode family. -->
<!-- This exists so IBM Bob stays powerful for design assets without drifting into unrestricted application edits. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/rules/shared/13-command-safety.md, ibm-bob/mode-pack/pilot-runbook.md -->
# Tool Profile

## Default Permission Set

全 mode は次を持ちます。

- `read`
- `mcp`
- `command`
- `edit`

## Edit Restriction

new family の `edit` は設計資産ルートに固定します。

```text
^(\.bob|ibm-bob|docs|\.copilot)/.*
```

これは generated `.bob/` を含みますが、source of truth は `ibm-bob/mode-pack/` です。

## Family Difference

old family の copied-workspace edit 制限は変えません。

new family だけが上の regex へ統一されます。

## Command Safety

destructive command は human checkpoint 必須です。

new family では shared rule から展開します。

old family は既存 rule 挙動を維持します。


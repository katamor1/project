<!-- ibm-bob/mode-pack/tool-profile.md -->
<!-- Defines the IBM Bob tool-group policy and edit restrictions for the custom mode pack. -->
<!-- This exists so full-parity Bob modes stay powerful without being allowed to mutate application code. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, .bob/rules/04-command-safety.md, ibm-bob/mode-pack/pilot-runbook.md -->
# Tool Profile

## Default Groups

全 custom mode は次を持ちます。

- `read`
- `mcp`
- `command`
- `edit`

この pack は `Power User` 方針です。

ただし、無制限編集にはしません。

## Edit Restriction

`edit` は次の設計資産ルートだけに制限します。

- `.bob/`
- `ibm-bob/`
- `docs/`
- `.copilot/`

regex は次です。

```text
^(\.bob|ibm-bob|docs|\.copilot)/.*
```

## Why This Restriction Exists

- Bob custom mode pack の v1 は設計資産用です。
- app code や runtime service code の実装は対象外です。
- built-in `code` や `advanced` を残しているので、実装側と役割を分けられます。

## Command Policy

`command` は全 mode にあります。

ただし `.bob/rules/04-command-safety.md` を全 mode に適用します。

特に destructive command は human checkpoint なしで進めません。

## MCP Policy

`mcp` は全 mode にあります。

source-backed generation, legacy runtime, review のどれでも evidence 参照が必要だからです。

<!-- ibm-bob/mode-pack/rules/ibmbob-identity-scope-guard/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob Identity Scope Guard. -->
<!-- This exists so IBM-Bob can mirror the canonical direct flow without rewriting the canonical prompt and routing sources. -->
<!-- RELEVANT FILES: .copilot/prompts/C2_identity_scope_guard.md, ibm-bob/mode-pack/routing/direct-mode-flow.json, ibm-bob/mode-pack/handoff-contracts.md -->
# IBM-Bob Identity Scope Guard Rules

## Read First
- `.copilot/prompts/C2_identity_scope_guard.md`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `ibm-bob/mode-pack/handoff-contracts.md`

## Responsibilities
- direct-mode-flow の入口責務だけを扱う。
- request の分類、packet 化、scope guard、dispatch、response shaping を混ぜない。
- checkpoint が必要な時は downstream へ進めず、理由と next action を明示する。
- 主な成果物は `identity gate result or checkpoint` として扱う。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。
- tenant, scope, principal の不一致を見つけたら必ず checkpoint に止める。

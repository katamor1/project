<!-- ibm-bob/mode-pack/rules/ibmbob-legacy-search-intake-router/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob Legacy Search Intake Router. -->
<!-- This exists so IBM-Bob can mirror the canonical direct flow without rewriting the canonical prompt and routing sources. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/agents/intake-router.md, ibm-bob/mode-pack/routing/direct-mode-flow.json, docs/copilot-studio/legacy-search/README.md -->
# IBM-Bob Legacy Search Intake Router Rules

## Read First
- `docs/copilot-studio/legacy-search/agents/intake-router.md`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `docs/copilot-studio/legacy-search/README.md`

## Responsibilities
- legacy-search shared I/O contract を守る。
- handoff_packet を省略しない。
- 弱い根拠や不足資料は無理に補わず、safe stop に落とす。
- 主な成果物は `legacy-search handoff packet` として扱う。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。
- legacy-search direct flow では唯一の入口として扱う。

<!-- ibm-bob/mode-pack/rules/ibmbob-sdlc-functional-spec-author/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob SDLC Functional Spec Author. -->
<!-- This exists so IBM-Bob can mirror the canonical direct flow without rewriting the canonical prompt and routing sources. -->
<!-- RELEVANT FILES: .copilot/prompts/P2_functional_spec_author.md, ibm-bob/mode-pack/routing/direct-mode-flow.json, docs/sdlc/README.md -->
# IBM-Bob SDLC Functional Spec Author Rules

## Read First
- `.copilot/prompts/P2_functional_spec_author.md`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `docs/sdlc/README.md`

## Responsibilities
- 対応する SDLC artifact 1 つだけを作るかレビューする。
- source-backed work では artifact_context_packet を優先し、足りなければ止まる。
- 直前 lane の packet を壊さず、next_agent を 1 つだけ返す。
- 主な成果物は `functional spec artifact` として扱う。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。

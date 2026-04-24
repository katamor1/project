<!-- ibm-bob/mode-pack/rules/ibmbob-summary-tree-builder/01-core.md -->
<!-- Defines the direct-mode rule for IBM-Bob Summary Tree Builder. -->
<!-- This exists so IBM-Bob can mirror the canonical direct flow without rewriting the canonical prompt and routing sources. -->
<!-- RELEVANT FILES: .copilot/prompts/runtime/ibmbob-summary-tree-builder.md, ibm-bob/mode-pack/routing/direct-mode-flow.json, docs/external-runtime/legacy-kb/README.md -->
# IBM-Bob Summary Tree Builder Rules

## Read First
- `.copilot/prompts/runtime/ibmbob-summary-tree-builder.md`
- `ibm-bob/mode-pack/routing/direct-mode-flow.json`
- `docs/external-runtime/legacy-kb/README.md`

## Responsibilities
- legacy knowledge runtime の処理だけを扱う。
- confidence, ACL, authority conflict の gate を保つ。
- artifact_context_packet や diff_report を作る時も、根拠の弱い claim を足さない。
- 主な成果物は `summary tree bundle` として扱う。

## Output Discipline
- canonical prompt の JSON contract を優先する。
- Bob 向け alias を説明で使ってよいが、返却 shape は canonical 名に合わせる。
- handoff がある時は `next_agent`, `handoff_packet`, `checkpoint` を落とさない。

## Safety
- direct flow の担当範囲を越える判断はしない。
- 根拠が弱い時は先へ進めず、abstain か human checkpoint に落とす。


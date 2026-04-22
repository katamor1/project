<!-- docs/claude-code/runtime-bridge/identity-scope.md -->
<!-- Defines how session scope is represented at the Claude Code bridge boundary. -->
<!-- This exists so unsafe dispatch is blocked before the external runtime or SDLC lanes start. -->
<!-- RELEVANT FILES: .copilot/schemas/session-scope-context.schema.json, docs/claude-code/runtime-bridge/pilot-runbook.md, docs/external-runtime/legacy-kb/security-acl.md -->
# Session Scope

## Required Scope Fields
- `user_principal`
- `workspace_roots`
- `repo_refs`
- `connector_scopes`
- `execution_surface`
- `allowed_targets`

## Default Allowed Targets
- `legacy_kb.query`
- `legacy_kb.diff`
- `sdlc.basic_design`
- `sdlc.detail_design`
- `sdlc.functional_spec`
- `sdlc.integration_spec`
- `sdlc.implementation`
- `sdlc.test_pack`
- `sdlc.review_record`
- `sdlc.eval`

## Guard Rules
- `user_principal` が欠ける時は block
- `workspace_roots` か `repo_refs` が空の時は block
- requested lane に必要な `connector_scopes` が無い時は block
- requested lane が `allowed_targets` に無い時は block
- scope は広げて推測しない

## Output
- OK の時は `session_scope_context`
- NG の時は `entry_human_checkpoint`

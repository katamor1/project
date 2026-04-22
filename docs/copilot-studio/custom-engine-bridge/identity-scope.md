<!-- docs/copilot-studio/custom-engine-bridge/identity-scope.md -->
<!-- Defines how user identity, tenant, and allowed scopes are represented at the Copilot bridge boundary. -->
<!-- This exists so unsafe dispatch is blocked before the external runtime or SDLC lanes start. -->
<!-- RELEVANT FILES: .copilot/schemas/bridge-identity-context.schema.json, docs/copilot-studio/custom-engine-bridge/pilot-runbook.md, docs/external-runtime/legacy-kb/security-acl.md -->
# Identity And Scope

## Required Identity Fields
- `user_principal`
- `groups`
- `tenant`
- `copilot_surface`
- `allowed_scopes`

## Default Allowed Scopes
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
- tenant が expected tenant と違う時は block
- requested lane に必要な scope が無い時は block
- scope は広げて推測しない

## Output
- OK の時は `bridge_identity_context`
- NG の時は `bridge_human_checkpoint`

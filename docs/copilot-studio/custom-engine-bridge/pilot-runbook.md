<!-- docs/copilot-studio/custom-engine-bridge/pilot-runbook.md -->
<!-- Provides the pilot steps and checkpoints for validating the Copilot custom engine bridge behavior. -->
<!-- This exists so the team can verify routing, abstain behavior, and user-facing response quality before implementation. -->
<!-- RELEVANT FILES: docs/copilot-studio/custom-engine-bridge/samples/email-notification-entry-dry-run.md, .copilot/prompts/C5_bridge_eval_monitor.md, .copilot/routing/custom-engine-bridge-matrix.yaml -->
# Pilot Runbook

## Pilot Cases
1. design request
2. legacy diff request
3. review / eval request
4. low confidence abstain
5. identity scope checkpoint

## Observation Points
- route が期待どおりか
- `artifact_context_packet` がある時だけ `P*` が始まるか
- Copilot 応答が短い日本語か
- abstain に次 action があるか
- checkpoint に required action があるか

## Block Conditions
- `identity missing`
- `tenant mismatch`
- `scope missing`
- `ACL mismatch`
- `authoritative conflict`
- `confidence < 0.65`

## Pilot Exit
- route ambiguity = 0
- abstain misfire = 0
- checkpoint reason が具体的
- bridge trace から request と runtime run が追える

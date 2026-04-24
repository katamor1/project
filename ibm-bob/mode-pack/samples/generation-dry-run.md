<!-- ibm-bob/mode-pack/samples/generation-dry-run.md -->
<!-- Shows a Bob dry run for source-backed design generation through entry, runtime, and SDLC modes. -->
<!-- This exists so the team can test the main generation path end-to-end without changing the canonical assets. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing-map.md, .bob/custom_modes.yaml, docs/sdlc/README.md -->
# Generation Dry Run

## User Intent

「通知設定変更の基本設計を、旧仕様の根拠付きで作りたい」

## Suggested Mode Path

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> k5-retrieval-planner -> k7-artifact-context-packer -> k8-evidence-reviewer -> p2-basic-design-author -> c4-entry-response-shaper`

## What To Check

- `c1` が `bridge_request_packet` を作れること
- `k7` が `artifact_context_packet` を作れること
- `k8` が `p2-basic-design-author` へ通せること
- `c4` が短い日本語で返せること

## Expected Bob Reply Shape

1. 旧仕様の根拠を確保できた
2. まだ未解決の点があれば明示する
3. 次に基本設計の下書きへ進むことを示す

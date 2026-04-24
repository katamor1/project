<!-- ibm-bob/mode-pack/routing-map.md -->
<!-- Summarizes how the IBM Bob custom modes map the existing entry, runtime, SDLC, and legacy-search flows. -->
<!-- This exists so the team can operate Bob with the same routing logic as the canonical prompt registry. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, .copilot/routing, ibm-bob/mode-pack/pilot-runbook.md -->
# Routing Map

## Entry Bridge

標準フローは次です。

`c0-entry-router -> c1-intent-packet-builder -> c2-identity-scope-guard -> c3-dispatch-packet-author -> k0-runtime-orchestrator -> c4-entry-response-shaper -> c5-entry-eval-monitor`

### Diff

`c3 -> k0 -> k5 -> k6 -> k8 -> c4`

### Source-Backed Generation

`c3 -> k0 -> k5 -> k7 -> k8 -> p* -> c4`

### Entry Gate

- `identity_missing`
- `tenant_mismatch`
- `scope_missing`

この 3 つは `c2` で止めます。

## Legacy Runtime

### Ingest

`k0 -> k1 -> k2 -> k3 -> k4 -> k9`

### Query / Diff

`k0 -> k5 -> k7 or k6 -> k8 -> p* / p8 / p9`

### Runtime Gate

- `confidence < 0.65`
- `acl_mismatch`
- `conflicting_authoritative_sources`

この 3 つは `k0` へ戻して human checkpoint にします。

- `0.65-0.78`

この帯は `k5` の second pass に戻します。

## SDLC

### Upper Design

`p0 -> p1 -> p2-basic -> p3 -> p2-detail -> p3 -> p8 -> p9`

### Test Spec

`p3 -> p2-functional or p2-integration -> p3 -> p8 -> p9`

### Implementation

`p3(detail pass) -> p4 -> p5 -> p6 -> p7 -> p8 -> p9`

### Execution Pack

`p7(pass) or repo_bridge_ready -> p10 -> p11 -> p12 -> p13 -> p14 -> p8 -> p9`

### Integration

`p14(pass) or execution_bundle_ready -> p15 -> p16 -> p17 -> p18 -> p19 -> p20 -> p21 -> p8 -> p9`

## Legacy Search

### Normal Answer

`ls-01 -> ls-02 -> ls-03 -> ls-04 -> ls-05`

### Renewal View

`ls-01 -> ls-02 -> ls-03 -> ls-04 -> ls-07`

### Change Diff

`ls-01 -> ls-02 -> ls-03 -> ls-04 -> ls-08`

### Safe Stop

`ls-01 -> ls-06`

または

`ls-03 / ls-04 -> ls-06`

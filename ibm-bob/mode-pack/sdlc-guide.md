<!-- ibm-bob/mode-pack/sdlc-guide.md -->
<!-- Explains how the old copied-workspace family and the new direct SDLC family coexist inside the Bob mode pack. -->
<!-- This exists so the team can choose the right Bob family for source-backed authoring, review, and evaluation work. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/routing/stage-flow.json, ibm-bob/mode-pack/routing/direct-mode-flow.json, docs/sdlc/README.md -->
# SDLC Guide

## Two Ways To Work

### Old family

`ibmbob-orchestrator` family は copied workspace を前提にした固定 run です。

実 build / test を含む run はこちらです。

### New family

`ibmbob-sdlc-*` family は direct authoring overlay です。

設計資産、review、eval、source-backed generation を直接扱います。

## Start Points

- 非 source-backed: `ibmbob-sdlc-request-orchestrator`
- source-backed: `ibmbob-entry-router`

## Main Lanes

### Upper Design

`ibmbob-sdlc-request-orchestrator -> ibmbob-sdlc-scope -> ibmbob-sdlc-basic-design-author -> ibmbob-sdlc-spec-reviewer -> ibmbob-sdlc-detail-design-author -> ibmbob-sdlc-spec-reviewer -> ibmbob-sdlc-review-record -> ibmbob-sdlc-eval-monitor`

### Test Spec

`ibmbob-sdlc-spec-reviewer -> ibmbob-sdlc-functional-spec-author or ibmbob-sdlc-integration-spec-author -> ibmbob-sdlc-spec-reviewer`

### Implementation

`ibmbob-sdlc-implementation-planner -> ibmbob-sdlc-fullstack-slice-author -> ibmbob-sdlc-test-author -> ibmbob-sdlc-code-reviewer`

### Execution Pack

`ibmbob-sdlc-execution-bundle-planner -> ibmbob-sdlc-frontend-codepack-author -> ibmbob-sdlc-backend-codepack-author -> ibmbob-sdlc-test-codepack-author -> ibmbob-sdlc-execution-reviewer`

### Integration

`ibmbob-sdlc-integration-bundle-planner -> ibmbob-sdlc-frontend-draft-author -> ibmbob-sdlc-backend-draft-author -> ibmbob-sdlc-sqlite-draft-author -> ibmbob-sdlc-test-draft-author -> ibmbob-sdlc-integration-reviewer -> ibmbob-sdlc-approval-pack-author`

## Source-Backed Rule

`artifact_context_packet` が無い時は次を開始しません。

- `ibmbob-sdlc-basic-design-author`
- `ibmbob-sdlc-implementation-planner`
- `ibmbob-sdlc-test-author`

この時は entry/runtime 側へ戻して evidence を先にそろえます。


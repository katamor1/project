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



## IBM-Bob Structured Specification Artifacts

For design and test-spec lanes, IBM-Bob treats each specification as a JSON/DOCX pair.

- Author modes create the intermediate JSON first.
- The JSON is validated against the artifact-kind schema.
- The DOCX is generated or filled from the artifact-kind Word template.
- Reviewer modes use the artifact-kind checklist and review JSON/DOCX consistency before human review.

Artifact kinds:

| Lane | artifact_kind | Author | Reviewer |
|---|---|---|---|
| Basic design | `basic_design` | `ibmbob-sdlc-basic-design-author` | `ibmbob-sdlc-spec-reviewer` |
| Detail design | `detail_design` | `ibmbob-sdlc-detail-design-author` / old `ibmbob-detail-design` | `ibmbob-sdlc-spec-reviewer` / old `ibmbob-review-gate` |
| Function test design | `function_test_design` | `ibmbob-sdlc-functional-spec-author` / old `ibmbob-functional-test-author` | `ibmbob-sdlc-spec-reviewer` / old `ibmbob-review-gate` |

Installed templates live in `.bob/ibm-bob/references/sdlc/structured-templates/`.

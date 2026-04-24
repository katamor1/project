# ibm-bob/mode-pack/scripts/reference_manifest.py
# Defines the explicit source-to-install reference mapping for IBM-Bob direct-mode assets.
# This exists so generated `.bob/ibm-bob/references/` stays curated and never mirrors source trees.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/scripts/validate_mode_pack.py, ibm-bob/mode-pack/modes/custom_modes.source.yaml
from __future__ import annotations

from pathlib import Path

ENTRY_PROMPTS = [
    "ibmbob-entry-router.md",
    "ibmbob-intent-packet-builder.md",
    "ibmbob-identity-scope-guard.md",
    "ibmbob-dispatch-packet-author.md",
    "ibmbob-entry-response-shaper.md",
    "ibmbob-entry-eval-monitor.md",
]

RUNTIME_PROMPTS = [
    "ibmbob-runtime-orchestrator.md",
    "ibmbob-intake-normalizer.md",
    "ibmbob-taxonomy-mapper.md",
    "ibmbob-summary-tree-builder.md",
    "ibmbob-relation-extractor.md",
    "ibmbob-retrieval-planner.md",
    "ibmbob-legacy-diff-checker.md",
    "ibmbob-artifact-context-packer.md",
    "ibmbob-evidence-reviewer.md",
    "ibmbob-runtime-eval-monitor.md",
]

SDLC_PROMPTS = [
    "ibmbob-sdlc-request-orchestrator.md",
    "ibmbob-sdlc-scope.md",
    "ibmbob-sdlc-basic-design-author.md",
    "ibmbob-sdlc-detail-design-author.md",
    "ibmbob-sdlc-functional-spec-author.md",
    "ibmbob-sdlc-integration-spec-author.md",
    "ibmbob-sdlc-spec-reviewer.md",
    "ibmbob-sdlc-implementation-planner.md",
    "ibmbob-sdlc-fullstack-slice-author.md",
    "ibmbob-sdlc-test-author.md",
    "ibmbob-sdlc-code-reviewer.md",
    "ibmbob-sdlc-review-record.md",
    "ibmbob-sdlc-eval-monitor.md",
    "ibmbob-sdlc-execution-bundle-planner.md",
    "ibmbob-sdlc-frontend-codepack-author.md",
    "ibmbob-sdlc-backend-codepack-author.md",
    "ibmbob-sdlc-test-codepack-author.md",
    "ibmbob-sdlc-execution-reviewer.md",
    "ibmbob-sdlc-integration-bundle-planner.md",
    "ibmbob-sdlc-frontend-draft-author.md",
    "ibmbob-sdlc-backend-draft-author.md",
    "ibmbob-sdlc-sqlite-draft-author.md",
    "ibmbob-sdlc-test-draft-author.md",
    "ibmbob-sdlc-integration-reviewer.md",
    "ibmbob-sdlc-approval-pack-author.md",
]

LEGACY_SEARCH_PROMPTS = [
    "ibmbob-legacy-search-intake-router.md",
    "ibmbob-legacy-search-glossary-normalizer.md",
    "ibmbob-legacy-search-document-locator.md",
    "ibmbob-legacy-search-evidence-verifier.md",
    "ibmbob-legacy-search-grounded-answerer.md",
    "ibmbob-legacy-search-gap-reporter.md",
    "ibmbob-legacy-search-renewal-impact-mapper.md",
    "ibmbob-legacy-search-change-diff-scout.md",
]

ENTRY_CONTRACTS = [
    "entry-request-packet.schema.json",
    "session-scope-context.schema.json",
    "entry-dispatch-packet.schema.json",
    "entry-response.schema.json",
    "entry-human-checkpoint.schema.json",
    "entry-trace.schema.json",
]

RUNTIME_CONTRACTS = [
    "document-record.schema.json",
    "section-node.schema.json",
    "chunk-record.schema.json",
    "relation-edge.schema.json",
    "acl-principal-set.schema.json",
    "ingest-job.schema.json",
    "retrieval-plan.schema.json",
    "diff-report.schema.json",
    "run-trace.schema.json",
]

SDLC_CONTRACTS = [
    "request-packet.schema.json",
    "scope-packet.schema.json",
    "work-packet.schema.json",
    "review-findings.schema.json",
    "review-record.schema.json",
    "artifact-manifest.schema.json",
    "implementation-packet.schema.json",
    "fullstack-slice.schema.json",
    "test-pack.schema.json",
    "code-review-findings.schema.json",
    "repo-bridge.schema.json",
    "api-contract.schema.json",
    "db-change.schema.json",
    "execution-bundle-packet.schema.json",
    "target-repo-map.schema.json",
    "frontend-codepack.schema.json",
    "backend-codepack.schema.json",
    "sqlite-change-pack.schema.json",
    "test-codepack.schema.json",
    "execution-review-findings.schema.json",
    "code-draft-packet.schema.json",
    "draft-file.schema.json",
    "frontend-draft-bundle.schema.json",
    "backend-draft-bundle.schema.json",
    "sqlite-draft-bundle.schema.json",
    "test-draft-bundle.schema.json",
    "integration-review-findings.schema.json",
    "approval-pack.schema.json",
]

SHARED_CONTRACTS = [
    "artifact-context-packet.schema.json",
    "evidence-bundle.schema.json",
]

ENTRY_ROUTING = ["ibmbob-entry-flow.yaml"]
RUNTIME_ROUTING = ["ibmbob-runtime-ingest-flow.yaml", "ibmbob-runtime-query-flow.yaml"]
SDLC_ROUTING = [
    "ibmbob-design-flow.yaml",
    "ibmbob-test-spec-flow.yaml",
    "ibmbob-implementation-flow.yaml",
    "ibmbob-execution-flow.yaml",
    "ibmbob-integration-flow.yaml",
]
LEGACY_SEARCH_ROUTING = ["ibmbob-legacy-search-flow.yaml"]

SHARED_GUIDES = [
    "ibm-bob/mode-pack/handoff-contracts.md",
    "ibm-bob/mode-pack/routing/direct-mode-flow.json",
]


def build_reference_manifest() -> dict[str, str]:
    manifest: dict[str, str] = {
        "ibm-bob/mode-pack/routing/stage-flow.json": ".bob/ibm-bob/stage-flow.json",
    }

    for name in ENTRY_PROMPTS:
        manifest[f".copilot/prompts/entry/{name}"] = f".bob/ibm-bob/references/entry/prompts/{name}"
    for name in RUNTIME_PROMPTS:
        manifest[f".copilot/prompts/runtime/{name}"] = f".bob/ibm-bob/references/runtime/prompts/{name}"
    for name in SDLC_PROMPTS:
        manifest[f".copilot/prompts/sdlc/{name}"] = f".bob/ibm-bob/references/sdlc/prompts/{name}"
    for name in LEGACY_SEARCH_PROMPTS:
        manifest[f".copilot/prompts/legacy-search/{name}"] = f".bob/ibm-bob/references/legacy-search/prompts/{name}"

    for name in ENTRY_CONTRACTS:
        manifest[f".copilot/schemas/{name}"] = f".bob/ibm-bob/references/entry/contracts/{name}"
    for name in RUNTIME_CONTRACTS:
        manifest[f".copilot/schemas/{name}"] = f".bob/ibm-bob/references/runtime/contracts/{name}"
    for name in SDLC_CONTRACTS:
        manifest[f".copilot/schemas/{name}"] = f".bob/ibm-bob/references/sdlc/contracts/{name}"
    for name in SHARED_CONTRACTS:
        manifest[f".copilot/schemas/{name}"] = f".bob/ibm-bob/references/shared/contracts/{name}"

    manifest["docs/copilot-studio/legacy-search/shared-io-contract.yaml"] = (
        ".bob/ibm-bob/references/legacy-search/contracts/shared-io-contract.yaml"
    )

    for name in ENTRY_ROUTING:
        manifest[f".copilot/routing/entry/{name}"] = f".bob/ibm-bob/references/entry/routing/{name}"
    for name in RUNTIME_ROUTING:
        manifest[f".copilot/routing/runtime/{name}"] = f".bob/ibm-bob/references/runtime/routing/{name}"
    for name in SDLC_ROUTING:
        manifest[f".copilot/routing/sdlc/{name}"] = f".bob/ibm-bob/references/sdlc/routing/{name}"
    for name in LEGACY_SEARCH_ROUTING:
        manifest[f".copilot/routing/legacy-search/{name}"] = f".bob/ibm-bob/references/legacy-search/routing/{name}"

    for source_path in SHARED_GUIDES:
        manifest[source_path] = f".bob/ibm-bob/references/shared/guides/{Path(source_path).name}"

    return manifest


REFERENCE_MANIFEST = build_reference_manifest()

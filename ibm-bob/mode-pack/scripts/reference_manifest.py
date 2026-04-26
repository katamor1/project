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

SDLC_TEMPLATE_GUIDES = [
    "scope.md",
    "basic-design.md",
    "detail-design.md",
    "functional-spec.md",
    "integration-spec.md",
    "implementation-plan.md",
    "fullstack-slice.md",
    "api-contract.md",
    "db-change.md",
    "test-spec.md",
    "repo-bridge.md",
    "execution-bundle-plan.md",
    "target-repo-map.md",
    "frontend-codepack.md",
    "backend-codepack.md",
    "sqlite-change-pack.md",
    "test-codepack.md",
    "integration-bundle-plan.md",
    "frontend-draft-bundle.md",
    "backend-draft-bundle.md",
    "sqlite-draft-bundle.md",
    "test-draft-bundle.md",
    "approval-pack.md",
    "review-summary.md",
    "review-record.md",
    "eval-report.md",
]

SDLC_SAMPLE_GUIDES = [
    "email-notification-dry-run.md",
    "email-notification-test-spec-dry-run.md",
    "email-notification-implementation-dry-run.md",
    "email-notification-execution-dry-run.md",
    "email-notification-integration-dry-run.md",
]

SDLC_EVAL_DATASET_GUIDES = [
    "README.md",
    "test-spec-lane.md",
    "implementation-lane.md",
    "execution-lane.md",
    "integration-lane.md",
]

SDLC_EVAL_REPORT_GUIDES = [
    "README.md",
    "test-spec-lane.md",
    "implementation-lane.md",
    "execution-lane.md",
    "integration-lane.md",
]

SDLC_SUPPORT_GUIDES = {
    "reviews/integration/README.md": "reviews-integration-readme.md",
    "artifacts/implementation/README.md": "artifacts-implementation-readme.md",
}

SHARED_GUIDES = {
    "ibm-bob/mode-pack/handoff-contracts.md": "handoff-contracts.md",
    "ibm-bob/mode-pack/routing/direct-mode-flow.json": "direct-mode-flow.json",
    "docs/copilot-studio/custom-engine-bridge/README.md": "entry-bridge-readme.md",
    "docs/copilot-studio/custom-engine-bridge/entry-flow.md": "entry-flow.md",
    "docs/copilot-studio/custom-engine-bridge/identity-scope.md": "identity-scope.md",
    "docs/copilot-studio/custom-engine-bridge/dispatch-contract.md": "dispatch-contract.md",
    "docs/copilot-studio/custom-engine-bridge/response-shaping.md": "response-shaping.md",
    "docs/copilot-studio/custom-engine-bridge/pilot-runbook.md": "entry-bridge-pilot-runbook.md",
    "docs/copilot-studio/custom-engine-bridge/samples/email-notification-entry-dry-run.md": (
        "entry-bridge-email-notification-entry-dry-run.md"
    ),
    "docs/copilot-studio/custom-engine-bridge/samples/email-notification-diff-dry-run.md": (
        "entry-bridge-email-notification-diff-dry-run.md"
    ),
    "docs/copilot-studio/custom-engine-bridge/samples/email-notification-abstain-dry-run.md": (
        "entry-bridge-email-notification-abstain-dry-run.md"
    ),
    "docs/copilot-studio/legacy-search/README.md": "legacy-search-readme.md",
    "docs/copilot-studio/legacy-search/routing-matrix.md": "legacy-search-routing-matrix.md",
    "docs/copilot-studio/legacy-search/implementation-guide.md": "legacy-search-implementation-guide.md",
    "docs/copilot-studio/legacy-search/pilot-runbook.md": "legacy-search-pilot-runbook.md",
    "docs/copilot-studio/legacy-search/readiness/knowledge-onboarding.md": "legacy-search-knowledge-onboarding.md",
    "docs/copilot-studio/legacy-search/readiness/knowledge-source-catalog.yaml": (
        "legacy-search-knowledge-source-catalog.yaml"
    ),
    "docs/copilot-studio/legacy-search/readiness/pilot-scorecard.yaml": "legacy-search-pilot-scorecard.yaml",
    "docs/copilot-studio/legacy-search/readiness/triage-playbook.md": "legacy-search-triage-playbook.md",
    "docs/copilot-studio/legacy-search/readiness/go-no-go-checklist.md": "legacy-search-go-no-go-checklist.md",
    "docs/copilot-studio/legacy-search/tests/evaluation-set.yaml": "legacy-search-evaluation-set.yaml",
    "docs/external-runtime/legacy-kb/README.md": "runtime-blueprint.md",
    "docs/external-runtime/legacy-kb/architecture.md": "runtime-architecture.md",
    "docs/external-runtime/legacy-kb/azure-topology.md": "runtime-topology.md",
    "docs/external-runtime/legacy-kb/api-spec.md": "runtime-api-spec.md",
    "docs/external-runtime/legacy-kb/data-pipeline.md": "runtime-data-pipeline.md",
    "docs/external-runtime/legacy-kb/query-diff-flow.md": "runtime-query-diff-flow.md",
    "docs/external-runtime/legacy-kb/security-acl.md": "runtime-security-acl.md",
    "docs/external-runtime/legacy-kb/eval-ops.md": "runtime-eval-ops.md",
    "docs/external-runtime/legacy-kb/runbook.md": "runtime-runbook.md",
    "docs/external-runtime/legacy-kb/samples/email-notification-ingest-dry-run.md": (
        "runtime-email-notification-ingest-dry-run.md"
    ),
    "docs/external-runtime/legacy-kb/samples/email-notification-query-dry-run.md": (
        "runtime-email-notification-query-dry-run.md"
    ),
    "docs/external-runtime/legacy-kb/samples/email-notification-diff-dry-run.md": (
        "runtime-email-notification-diff-dry-run.md"
    ),
    "docs/sdlc/README.md": "sdlc-readme.md",
}

REFERENCE_ALIASES = {
    ".copilot/prompts/": ".bob/ibm-bob/references/",
    ".copilot/schemas/": ".bob/ibm-bob/references/",
    ".copilot/routing/": ".bob/ibm-bob/references/",
    "docs/copilot-studio/custom-engine-bridge/": ".bob/ibm-bob/references/shared/guides/entry-bridge-readme.md",
    "docs/copilot-studio/custom-engine-bridge": ".bob/ibm-bob/references/shared/guides/entry-bridge-readme.md",
    "docs/copilot-studio/legacy-search/": ".bob/ibm-bob/references/shared/guides/legacy-search-readme.md",
    "docs/copilot-studio/legacy-search": ".bob/ibm-bob/references/shared/guides/legacy-search-readme.md",
    "docs/external-runtime/legacy-kb/": ".bob/ibm-bob/references/shared/guides/runtime-blueprint.md",
    "docs/external-runtime/legacy-kb": ".bob/ibm-bob/references/shared/guides/runtime-blueprint.md",
    "docs/sdlc/templates/": ".bob/ibm-bob/references/sdlc/templates/",
    "ibm-bob/mode-pack/structured-templates/": ".bob/ibm-bob/references/sdlc/structured-templates/",
    "structured-templates/": ".bob/ibm-bob/references/sdlc/structured-templates/",
    "docs/sdlc/samples/": ".bob/ibm-bob/references/sdlc/samples/",
    "docs/sdlc/": ".bob/ibm-bob/references/shared/guides/sdlc-readme.md",
    "docs/sdlc": ".bob/ibm-bob/references/shared/guides/sdlc-readme.md",
    "reviews/integration/": ".bob/ibm-bob/references/sdlc/guides/",
    "artifacts/implementation/": ".bob/ibm-bob/references/sdlc/guides/",
}


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

    for name in SDLC_TEMPLATE_GUIDES:
        manifest[f"docs/sdlc/templates/{name}"] = f".bob/ibm-bob/references/sdlc/templates/{name}"
    for name in SDLC_SAMPLE_GUIDES:
        manifest[f"docs/sdlc/samples/{name}"] = f".bob/ibm-bob/references/sdlc/samples/{name}"
    for name in SDLC_EVAL_DATASET_GUIDES:
        manifest[f"evals/datasets/{name}"] = f".bob/ibm-bob/references/sdlc/evals/datasets/{name}"
    for name in SDLC_EVAL_REPORT_GUIDES:
        manifest[f"evals/reports/{name}"] = f".bob/ibm-bob/references/sdlc/evals/reports/{name}"
    for source_path, install_name in SDLC_SUPPORT_GUIDES.items():
        manifest[source_path] = f".bob/ibm-bob/references/sdlc/guides/{install_name}"
    for source_path, install_name in SHARED_GUIDES.items():
        manifest[source_path] = f".bob/ibm-bob/references/shared/guides/{install_name}"

    return manifest


REFERENCE_MANIFEST = build_reference_manifest()
REFERENCE_TARGETS = {**REFERENCE_MANIFEST, **REFERENCE_ALIASES}

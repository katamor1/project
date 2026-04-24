# ibm-bob/mode-pack/scripts/validate_mode_pack.py
# Runs static validation for the IBM-Bob mode-pack source, generated Bob config, routing, and simulated packets.
# This exists so the repo can catch broken mode metadata or packet contracts before Bob uses them.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/scripts/evaluate_mode_pack.py, ibm-bob/mode-pack/modes/custom_modes.source.yaml
from __future__ import annotations

import tempfile
from pathlib import Path

import yaml

from evaluate_mode_pack import run_case
from install_mode_pack import install_to_workspace
from runtime_common import DIRECT_ROUTING_SOURCE, MODES_SOURCE, SCHEMA_ROOT, SAMPLES_ROOT, hash_tree, load_jsonc, load_yaml_file

OLD_EXPECTED_SLUGS = [
    "ibmbob-orchestrator",
    "ibmbob-intake",
    "ibmbob-context-scout",
    "ibmbob-detail-design",
    "ibmbob-review-gate",
    "ibmbob-code-author",
    "ibmbob-unit-test-author",
    "ibmbob-unit-test-executor",
    "ibmbob-functional-test-author",
    "ibmbob-run-summary",
]
DIRECT_EXPECTED_SLUGS = [
    "ibmbob-entry-router",
    "ibmbob-intent-packet-builder",
    "ibmbob-identity-scope-guard",
    "ibmbob-dispatch-packet-author",
    "ibmbob-entry-response-shaper",
    "ibmbob-entry-eval-monitor",
    "ibmbob-runtime-orchestrator",
    "ibmbob-intake-normalizer",
    "ibmbob-taxonomy-mapper",
    "ibmbob-summary-tree-builder",
    "ibmbob-relation-extractor",
    "ibmbob-retrieval-planner",
    "ibmbob-legacy-diff-checker",
    "ibmbob-artifact-context-packer",
    "ibmbob-evidence-reviewer",
    "ibmbob-runtime-eval-monitor",
    "ibmbob-sdlc-request-orchestrator",
    "ibmbob-sdlc-scope",
    "ibmbob-sdlc-basic-design-author",
    "ibmbob-sdlc-detail-design-author",
    "ibmbob-sdlc-functional-spec-author",
    "ibmbob-sdlc-integration-spec-author",
    "ibmbob-sdlc-spec-reviewer",
    "ibmbob-sdlc-implementation-planner",
    "ibmbob-sdlc-fullstack-slice-author",
    "ibmbob-sdlc-test-author",
    "ibmbob-sdlc-code-reviewer",
    "ibmbob-sdlc-review-record",
    "ibmbob-sdlc-eval-monitor",
    "ibmbob-sdlc-execution-bundle-planner",
    "ibmbob-sdlc-frontend-codepack-author",
    "ibmbob-sdlc-backend-codepack-author",
    "ibmbob-sdlc-test-codepack-author",
    "ibmbob-sdlc-execution-reviewer",
    "ibmbob-sdlc-integration-bundle-planner",
    "ibmbob-sdlc-frontend-draft-author",
    "ibmbob-sdlc-backend-draft-author",
    "ibmbob-sdlc-sqlite-draft-author",
    "ibmbob-sdlc-test-draft-author",
    "ibmbob-sdlc-integration-reviewer",
    "ibmbob-sdlc-approval-pack-author",
    "ibmbob-legacy-search-intake-router",
    "ibmbob-legacy-search-glossary-normalizer",
    "ibmbob-legacy-search-document-locator",
    "ibmbob-legacy-search-evidence-verifier",
    "ibmbob-legacy-search-grounded-answerer",
    "ibmbob-legacy-search-gap-reporter",
    "ibmbob-legacy-search-renewal-impact-mapper",
    "ibmbob-legacy-search-change-diff-scout",
]


def main() -> int:
    source = load_yaml_file(MODES_SOURCE)
    slugs = [mode["slug"] for mode in source["sourceModes"]]
    assert slugs[: len(OLD_EXPECTED_SLUGS)] == OLD_EXPECTED_SLUGS, slugs
    assert slugs[len(OLD_EXPECTED_SLUGS) :] == DIRECT_EXPECTED_SLUGS, slugs
    for schema_path in sorted(SCHEMA_ROOT.glob("*.json")):
        load_jsonc(schema_path)
    with tempfile.TemporaryDirectory() as temp_dir:
        workspace = Path(temp_dir) / "workspace"
        workspace.mkdir()
        install_to_workspace(workspace, force=True)
        generated = yaml.safe_load((workspace / ".bob" / "custom_modes.yaml").read_text(encoding="utf-8"))
        generated_slugs = [mode["slug"] for mode in generated["customModes"]]
        assert generated_slugs == OLD_EXPECTED_SLUGS + DIRECT_EXPECTED_SLUGS, generated_slugs
        for slug in OLD_EXPECTED_SLUGS + DIRECT_EXPECTED_SLUGS:
            assert (workspace / ".bob" / f"rules-{slug}" / "01-core.md").exists()
        assert not (workspace / ".bob" / "rules-ibmbob-orchestrator" / "10-response-style.md").exists()
        assert (workspace / ".bob" / "rules-ibmbob-entry-router" / "10-response-style.md").exists()
    routes = load_jsonc(Path(__file__).resolve().parent.parent / "routing" / "stage-flow.json")["routes"]
    assert routes["unit_test_run"]["on_revise"] == {
        "code": "ibmbob-code-author",
        "unit_test": "ibmbob-unit-test-author",
        "intake": "ibmbob-intake",
    }
    assert routes["functional_test_pack"]["on_pass"] == "ibmbob-run-summary"
    direct_routes = load_jsonc(DIRECT_ROUTING_SOURCE)
    direct_slugs = set()
    for family_slugs in direct_routes["families"].values():
        direct_slugs.update(family_slugs)
    for flow in direct_routes["manual_pilot_flows"].values():
        direct_slugs.update(flow)
    assert direct_slugs <= set(DIRECT_EXPECTED_SLUGS), direct_slugs
    original_hash = hash_tree(SAMPLES_ROOT / "base")
    summary = run_case(next((SAMPLES_ROOT / "評価用基本設計書").glob("*.docx")), "simulate")
    assert summary["status"] == "completed"
    assert hash_tree(SAMPLES_ROOT / "base") == original_hash
    real_run = run_case(next((SAMPLES_ROOT / "評価用基本設計書").glob("*.docx")), "real")
    assert real_run["status"] in {"completed", "blocked"}
    print("mode pack validation passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

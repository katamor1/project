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
from runtime_common import MODES_SOURCE, SCHEMA_ROOT, SAMPLES_ROOT, hash_tree, load_jsonc, load_yaml_file

EXPECTED_SLUGS = [
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


def main() -> int:
    source = load_yaml_file(MODES_SOURCE)
    slugs = [mode["slug"] for mode in source["sourceModes"]]
    assert slugs == EXPECTED_SLUGS, slugs
    for schema_path in sorted(SCHEMA_ROOT.glob("*.json")):
        load_jsonc(schema_path)
    with tempfile.TemporaryDirectory() as temp_dir:
        workspace = Path(temp_dir) / "workspace"
        workspace.mkdir()
        install_to_workspace(workspace, force=True)
        generated = yaml.safe_load((workspace / ".bob" / "custom_modes.yaml").read_text(encoding="utf-8"))
        generated_slugs = [mode["slug"] for mode in generated["customModes"]]
        assert generated_slugs == EXPECTED_SLUGS, generated_slugs
        for slug in EXPECTED_SLUGS:
            assert (workspace / ".bob" / f"rules-{slug}" / "01-core.md").exists()
    routes = load_jsonc(Path(__file__).resolve().parent.parent / "routing" / "stage-flow.json")["routes"]
    assert routes["unit_test_run"]["on_revise"] == {
        "code": "ibmbob-code-author",
        "unit_test": "ibmbob-unit-test-author",
        "intake": "ibmbob-intake",
    }
    assert routes["functional_test_pack"]["on_pass"] == "ibmbob-run-summary"
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

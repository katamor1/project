# ibm-bob/mode-pack/scripts/install_mode_pack.py
# Generates `.bob/custom_modes.yaml` and `.bob/rules-{slug}/` from the IBM-Bob mode-pack source files.
# This exists so copied workspaces can load the same Bob custom modes in Shell and IDE.
# RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/rules/ibmbob-orchestrator/01-core.md, ibm-bob/mode-pack/README.md
from __future__ import annotations

import argparse
import shutil
from pathlib import Path

import yaml

from runtime_common import MODES_SOURCE, PROFILES_ROOT, ROUTING_SOURCE, RULES_ROOT, ensure_dir, load_yaml_file


def _render_groups(permissions: list) -> list:
    groups: list = []
    for permission in permissions:
        if isinstance(permission, str):
            groups.append(permission)
            continue
        if permission.get("type") == "edit":
            groups.append(
                [
                    "edit",
                    {
                        "fileRegex": permission["fileRegex"],
                        "description": permission["description"],
                    },
                ]
            )
    return groups


def build_custom_modes() -> dict:
    source = load_yaml_file(MODES_SOURCE)
    custom_modes = []
    for mode in source["sourceModes"]:
        custom_modes.append(
            {
                "slug": mode["slug"],
                "name": mode["name"],
                "description": mode["description"],
                "roleDefinition": mode["roleDefinition"],
                "whenToUse": mode["whenToUse"],
                "customInstructions": mode["customInstructions"],
                "groups": _render_groups(mode.get("permissions", [])),
            }
        )
    return {"customModes": custom_modes}


def install_to_workspace(workspace_root: Path, force: bool = False) -> dict:
    bob_root = ensure_dir(workspace_root / ".bob")
    bundle_root = ensure_dir(bob_root / "ibm-bob")
    mode_doc = build_custom_modes()
    (bob_root / "custom_modes.yaml").write_text(
        yaml.safe_dump(mode_doc, allow_unicode=True, sort_keys=False),
        encoding="utf-8",
    )
    source = load_yaml_file(MODES_SOURCE)
    for mode in source["sourceModes"]:
        src = RULES_ROOT / mode["ruleDir"]
        dst = bob_root / f"rules-{mode['slug']}"
        if dst.exists() and force:
            shutil.rmtree(dst)
        if dst.exists():
            continue
        shutil.copytree(src, dst)
    shutil.copy2(ROUTING_SOURCE, bundle_root / "stage-flow.json")
    profiles_root = ensure_dir(bundle_root / "profiles")
    for path in PROFILES_ROOT.glob("*.json"):
        shutil.copy2(path, profiles_root / path.name)
    return {
        "workspace_root": str(workspace_root),
        "custom_modes_path": str(bob_root / "custom_modes.yaml"),
        "rules_root": str(bob_root),
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--workspace", required=True)
    parser.add_argument("--force", action="store_true")
    args = parser.parse_args()
    result = install_to_workspace(Path(args.workspace).resolve(), force=args.force)
    print(f"installed: {result['custom_modes_path']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

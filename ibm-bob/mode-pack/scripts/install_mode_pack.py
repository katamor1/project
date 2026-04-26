# ibm-bob/mode-pack/scripts/install_mode_pack.py
# Generates `.bob/custom_modes.yaml` and `.bob/rules-{slug}/` from the IBM-Bob mode-pack source files.
# This exists so copied workspaces can load one generated Bob config without mirroring repo source trees.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/reference_manifest.py, ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/README.md
from __future__ import annotations

import argparse
import shutil
from pathlib import Path

import yaml

from reference_manifest import REFERENCE_MANIFEST, REFERENCE_TARGETS
from runtime_common import MODES_SOURCE, PACK_ROOT, PROFILES_ROOT, REPO_ROOT, ROUTING_SOURCE, RULES_ROOT, ensure_dir, load_yaml_file

OLD_FAMILY_SLUGS = {
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
}
SHARED_RULES_ROOT = RULES_ROOT / "shared"
TEXT_SUFFIXES = {".md", ".json", ".yaml", ".yml"}


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


def _installed_path_for_reference(repo_relative_path: str) -> str:
    normalized = repo_relative_path.replace("\\", "/")
    return REFERENCE_TARGETS.get(normalized, normalized)


def _reference_variants(repo_relative_path: str) -> list[str]:
    normalized = repo_relative_path.replace("\\", "/")
    absolute_posix = (REPO_ROOT / Path(normalized)).resolve().as_posix()
    absolute_windows = str((REPO_ROOT / Path(normalized)).resolve())
    return [
        f"/{absolute_posix}",
        absolute_posix,
        absolute_windows,
        normalized,
    ]


def _rewrite_text_for_workspace(text: str, reference_paths: set[str]) -> str:
    rewritten = text
    for normalized in sorted(REFERENCE_TARGETS, key=len, reverse=True):
        installed_path = _installed_path_for_reference(normalized)
        replaced = False
        for source_variant in _reference_variants(normalized):
            if source_variant in rewritten:
                rewritten = rewritten.replace(source_variant, installed_path)
                replaced = True
        if replaced and normalized in REFERENCE_MANIFEST:
            reference_paths.add(normalized)
    return rewritten


def _copy_reference_files(reference_paths: set[str], workspace_root: Path) -> None:
    pending = set(reference_paths)
    copied: set[str] = set()
    while pending:
        repo_relative_path = sorted(pending)[0]
        pending.remove(repo_relative_path)
        if repo_relative_path in copied:
            continue
        source_path = REPO_ROOT / Path(repo_relative_path)
        if not source_path.exists():
            # Standalone mode-pack zips may not include the canonical repo references.
            # Keep install usable and rely on curated in-pack structured templates when present.
            print(f"warning: reference source missing, skipped: {repo_relative_path}")
            continue
        destination_path = workspace_root / Path(_installed_path_for_reference(repo_relative_path))
        ensure_dir(destination_path.parent)
        shutil.copy2(source_path, destination_path)
        copied.add(repo_relative_path)
        if destination_path.suffix in TEXT_SUFFIXES:
            nested_paths: set[str] = set()
            rewritten = _rewrite_text_for_workspace(destination_path.read_text(encoding="utf-8"), nested_paths)
            destination_path.write_text(rewritten, encoding="utf-8")
            pending.update(nested_paths - copied)


def _rewrite_rule_bundle(rule_root: Path, reference_paths: set[str]) -> None:
    for markdown_file in sorted(rule_root.glob("*.md")):
        rewritten = _rewrite_text_for_workspace(markdown_file.read_text(encoding="utf-8"), reference_paths)
        markdown_file.write_text(rewritten, encoding="utf-8")



def _copy_structured_templates(workspace_root: Path) -> None:
    source_root = PACK_ROOT / "structured-templates"
    if not source_root.exists():
        return
    destination_root = workspace_root / ".bob" / "ibm-bob" / "references" / "sdlc" / "structured-templates"
    if destination_root.exists():
        shutil.rmtree(destination_root)
    shutil.copytree(source_root, destination_root)


def build_custom_modes(reference_paths: set[str] | None = None) -> dict:
    tracked_paths = reference_paths if reference_paths is not None else set()
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
                "customInstructions": _rewrite_text_for_workspace(mode["customInstructions"], tracked_paths),
                "groups": _render_groups(mode.get("permissions", [])),
            }
        )
    return {"customModes": custom_modes}


def install_to_workspace(workspace_root: Path, force: bool = False) -> dict:
    bob_root = ensure_dir(workspace_root / ".bob")
    bundle_root = ensure_dir(bob_root / "ibm-bob")

    if force and (bundle_root / "references").exists():
        shutil.rmtree(bundle_root / "references")
    if force and (bundle_root / "direct-mode-flow.json").exists():
        (bundle_root / "direct-mode-flow.json").unlink()

    reference_paths: set[str] = {
        source_path
        for source_path, install_path in REFERENCE_MANIFEST.items()
        if "/references/" in install_path
    }
    mode_doc = build_custom_modes(reference_paths)
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
        if mode["slug"] not in OLD_FAMILY_SLUGS and SHARED_RULES_ROOT.exists():
            for shared_rule in sorted(SHARED_RULES_ROOT.glob("*.md")):
                shutil.copy2(shared_rule, dst / shared_rule.name)
        _rewrite_rule_bundle(dst, reference_paths)

    shutil.copy2(ROUTING_SOURCE, bundle_root / "stage-flow.json")
    _copy_reference_files(reference_paths, workspace_root)
    _copy_structured_templates(workspace_root)

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

# ibm-bob/mode-pack/scripts/workspace_run.py
# Prepares copied IBM-Bob workspaces, installs the mode pack, selects workspace profiles, and runs unit-test commands.
# This exists so evaluation and validation can exercise the mode pack without touching the sample originals.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/scripts/stage_packets.py, ibm-bob/mode-pack/profiles/ibm-bob-vc2026.workspace-profile.json
from __future__ import annotations

import re
import shutil
from pathlib import Path

from install_mode_pack import install_to_workspace
from runtime_common import ARTIFACTS_ROOT, PROFILES_ROOT, SAMPLES_ROOT, command_available, copy_tree, ensure_dir, load_jsonc, run_command, timestamp_run_id, write_json

VS_DEV_SHELL = Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1")
KNOWN_CLANG_CL_PATHS = [
    Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\bin\clang-cl.exe"),
    Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe"),
    Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\ARM64\bin\clang-cl.exe"),
]


def _eval_case_id(input_doc: Path) -> str:
    match = re.search(r"_(\d+)$", input_doc.stem)
    return f"case-{match.group(1)}" if match else input_doc.stem


def known_clang_cl_exists() -> bool:
    return any(path.exists() for path in KNOWN_CLANG_CL_PATHS)


def select_workspace_profile() -> tuple[Path, list[str]]:
    if command_available("cl.exe"):
        return PROFILES_ROOT / "ibm-bob-vc2026.workspace-profile.json", []
    if command_available("clang-cl.exe") or known_clang_cl_exists():
        return PROFILES_ROOT / "ibm-bob-clang-win.workspace-profile.json", []
    if VS_DEV_SHELL.exists():
        return PROFILES_ROOT / "ibm-bob-vc2026.workspace-profile.json", []
    return PROFILES_ROOT / "ibm-bob-vc2026.workspace-profile.json", ["toolchain not available for configured profiles"]


def prepare_run(input_doc: Path, execution_mode: str) -> tuple[dict, dict, list[str]]:
    eval_case_id = _eval_case_id(input_doc)
    run_id = timestamp_run_id(eval_case_id)
    run_root = ensure_dir(ARTIFACTS_ROOT / "runs" / run_id)
    workspace_root = ARTIFACTS_ROOT / "workspaces" / run_id / "project"
    copy_tree(SAMPLES_ROOT / "base", workspace_root)
    copied_input = ensure_dir(run_root / "input") / input_doc.name
    shutil.copy2(input_doc, copied_input)
    profile_path, issues = select_workspace_profile()
    profile = load_jsonc(profile_path)
    run_request = {
        "run_id": run_id,
        "project_root": str(workspace_root),
        "input_basic_design_doc": str(copied_input),
        "workspace_profile_ref": str(run_root / "workspace_profile.json"),
        "run_policy": {"execution_mode": execution_mode, "write_markdown": True, "stop_on_block": True},
        "locale": "ja",
        "eval_case_id": eval_case_id,
        "workspace_copy_root": str(workspace_root),
        "artifacts_root": str(run_root),
    }
    write_json(run_root / "run-request.json", run_request)
    write_json(run_root / "workspace_profile.json", profile)
    install_to_workspace(workspace_root, force=True)
    write_json(workspace_root / ".bob" / "ibm-bob" / "current_run.json", run_request)
    write_json(workspace_root / ".bob" / "ibm-bob" / "workspace_profile.json", profile)
    return run_request, profile, issues


def execute_unit_tests(run_request: dict, workspace_profile: dict) -> dict:
    run_root = Path(run_request["artifacts_root"])
    logs_root = ensure_dir(run_root / "logs")
    if run_request["run_policy"]["execution_mode"] == "simulate":
        return {
            "run_id": run_request["run_id"],
            "build_command": workspace_profile["build_command"],
            "test_command": workspace_profile["unit_test_command"],
            "exit_code": 0,
            "status": "pass",
            "log_refs": ["SIMULATED"],
            "failed_cases": [],
            "reroute_target": "none",
            "next_agent": "ibmbob-review-gate",
        }
    build = run_command(workspace_profile["build_command"], Path(run_request["project_root"]), workspace_profile["command_timeout_sec"])
    build_log = logs_root / "build.log"
    build_log.write_text(build["stdout"] + "\n" + build["stderr"], encoding="utf-8")
    if build["exit_code"] != 0:
        combined = build["stdout"] + "\n" + build["stderr"]
        intake_markers = [
            "not found in PATH",
            "Launch-VsDevShell",
            "Developer Command Prompt",
            "Developer PowerShell",
            "DevShellModuleLoad",
            "Could not start Developer PowerShell",
        ]
        reroute = "intake" if any(marker in combined for marker in intake_markers) else "code"
        return {
            "run_id": run_request["run_id"],
            "build_command": workspace_profile["build_command"],
            "test_command": workspace_profile["unit_test_command"],
            "exit_code": build["exit_code"],
            "status": "fail",
            "log_refs": [str(build_log)],
            "failed_cases": ["build"],
            "reroute_target": reroute,
            "next_agent": "ibmbob-review-gate",
        }
    test = run_command(workspace_profile["unit_test_command"], Path(run_request["project_root"]), workspace_profile["command_timeout_sec"])
    test_log = logs_root / "unit-test.log"
    test_log.write_text(test["stdout"] + "\n" + test["stderr"], encoding="utf-8")
    combined = test["stdout"] + "\n" + test["stderr"]
    reroute = "none" if test["exit_code"] == 0 else ("intake" if "not found in PATH" in combined else "code")
    return {
        "run_id": run_request["run_id"],
        "build_command": workspace_profile["build_command"],
        "test_command": workspace_profile["unit_test_command"],
        "exit_code": test["exit_code"],
        "status": "pass" if test["exit_code"] == 0 else "fail",
        "log_refs": [str(build_log), str(test_log)],
        "failed_cases": [] if test["exit_code"] == 0 else ["unit-test"],
        "reroute_target": reroute,
        "next_agent": "ibmbob-review-gate",
    }

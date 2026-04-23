# ibm-bob/mode-pack/scripts/runtime_common.py
# Provides shared path, file, hashing, YAML, and command helpers for the IBM-Bob mode-pack scripts.
# This exists so installer, evaluator, and validators share one small runtime toolkit.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/scripts/workspace_run.py, ibm-bob/mode-pack/scripts/json_schema_lite.py
from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
from datetime import datetime
from pathlib import Path

import yaml

SCRIPT_DIR = Path(__file__).resolve().parent
PACK_ROOT = SCRIPT_DIR.parent
REPO_ROOT = PACK_ROOT.parents[1]
IBM_BOB_ROOT = PACK_ROOT.parent
SAMPLES_ROOT = IBM_BOB_ROOT / "samples"
ARTIFACTS_ROOT = REPO_ROOT / "artifacts" / "ibm-bob"
MODES_SOURCE = PACK_ROOT / "modes" / "custom_modes.source.yaml"
RULES_ROOT = PACK_ROOT / "rules"
SCHEMA_ROOT = PACK_ROOT / "schemas"
PROFILES_ROOT = PACK_ROOT / "profiles"
ROUTING_SOURCE = PACK_ROOT / "routing" / "stage-flow.json"


def ensure_dir(path: Path) -> Path:
    path.mkdir(parents=True, exist_ok=True)
    return path


def strip_jsonc(text: str) -> str:
    return "\n".join(line for line in text.splitlines() if not line.lstrip().startswith("//"))


def load_jsonc(path: Path) -> dict:
    return json.loads(strip_jsonc(path.read_text(encoding="utf-8")))


def load_yaml_file(path: Path) -> dict:
    return yaml.safe_load(path.read_text(encoding="utf-8"))


def write_json(path: Path, data: dict) -> None:
    ensure_dir(path.parent)
    path.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def write_text(path: Path, text: str) -> None:
    ensure_dir(path.parent)
    path.write_text(text, encoding="utf-8")


def copy_tree(src: Path, dst: Path) -> None:
    if dst.exists():
        shutil.rmtree(dst)
    shutil.copytree(src, dst)


def hash_tree(path: Path) -> str:
    digest = hashlib.sha256()
    for item in sorted(path.rglob("*")):
        if item.is_file():
            digest.update(str(item.relative_to(path)).encode("utf-8"))
            digest.update(item.read_bytes())
    return digest.hexdigest()


def relative_to_repo(path: Path) -> str:
    try:
        return str(path.resolve().relative_to(REPO_ROOT))
    except ValueError:
        return str(path.resolve())


def timestamp_run_id(eval_case_id: str) -> str:
    stamp = datetime.now().strftime("%Y%m%d-%H%M%S-%f")
    return f"ibmbob-{eval_case_id}-{stamp}"


def run_command(command: str, cwd: Path, timeout_sec: int) -> dict:
    def _decode(data: bytes) -> str:
        for encoding in ("utf-8", "cp932"):
            try:
                return data.decode(encoding)
            except UnicodeDecodeError:
                continue
        return data.decode("utf-8", errors="replace")

    result = subprocess.run(
        command,
        cwd=str(cwd),
        shell=True,
        capture_output=True,
        text=False,
        timeout=timeout_sec,
        check=False,
    )
    return {
        "command": command,
        "cwd": str(cwd),
        "exit_code": result.returncode,
        "stdout": _decode(result.stdout),
        "stderr": _decode(result.stderr),
    }


def command_available(command_name: str) -> bool:
    probe = subprocess.run(
        f"where {command_name}",
        shell=True,
        capture_output=True,
        text=True,
        check=False,
    )
    return probe.returncode == 0

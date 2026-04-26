# .agents/skills/legacy-basic-design-agent/scripts/kb_profile.py
# Loads, merges, infers, and applies KB profiles for legacy document ingestion and query.
# This exists so project-specific terms, headings, and document roles stay out of code.
# RELEVANT FILES: .agents/skills/legacy-basic-design-agent/scripts/build_kb.py, .agents/skills/legacy-basic-design-agent/scripts/query_kb.py, .agents/skills/legacy-basic-design-agent/profiles/default.yaml
from __future__ import annotations

import fnmatch
import json
import re
from copy import deepcopy
from pathlib import Path

import yaml

DEFAULT_PROFILE = {
    "version": 1,
    "document_rules": [],
    "heading_rules": {
        "style_prefixes": ["Heading", "見出し"],
        "number_patterns": [r"^\d+(\.\d+)*[.)]?\s+", r"^第.+[章節]\s*"],
        "heading_aliases": [],
        "max_heading_length": 80,
        "sentence_endings": ["。", "."],
    },
    "entity_rules": {
        "patterns": [r"\b[A-Z]{1,4}[-_]?\d+(?:\.\d+)?\b"],
        "terms": [],
        "auto_from_titles": True,
        "auto_token_pattern": r"[\w\u3040-\u30ff\u4e00-\u9fff]{2,24}",
        "max_entities": 8,
    },
    "ranking": {
        "role_weights": {},
        "base_term_weight": 3.0,
        "expanded_term_weight": 0.15,
        "title_match_bonus": 1.5,
        "unusable_text_penalty": 1.0,
        "minimum_confidence": 0.65,
        "confidence_bonuses": [],
    },
    "output": {
        "task_id": "LEGACY-KB-QUERY",
        "artifact_type": "basic_design",
        "target_agent": "legacy-basic-design-agent",
        "context_packer": "artifact-context-packer",
        "next_agent": "design-author",
    },
    "defaults": {
        "type": "general",
        "role": "support",
        "authority_score": 0.5,
        "approval_status": "unknown",
        "interference_reason": "This document may contain related legacy behavior or constraints.",
    },
}


def deep_merge(base: dict, override: dict) -> dict:
    merged = deepcopy(base)
    for key, value in (override or {}).items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = deep_merge(merged[key], value)
        else:
            merged[key] = value
    return merged


def load_profile(path: str | Path | None = None) -> dict:
    if not path:
        return deepcopy(DEFAULT_PROFILE)
    data = yaml.safe_load(Path(path).read_text(encoding="utf-8")) or {}
    return deep_merge(DEFAULT_PROFILE, data)


def write_profile(path: Path, profile: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    text = yaml.safe_dump(profile, allow_unicode=True, sort_keys=False)
    path.write_text(text, encoding="utf-8")


def infer_profile(docs_root: Path, profile: dict) -> dict:
    inferred = deepcopy(profile)
    existing = {rule.get("path") for rule in inferred.get("document_rules", [])}
    top_dirs = sorted({item.relative_to(docs_root).parts[0] for item in docs_root.rglob("*") if item.is_file() and len(item.relative_to(docs_root).parts) > 1})
    for name in top_dirs:
        pattern = f"{name}/**"
        if pattern in existing:
            continue
        inferred.setdefault("document_rules", []).append(infer_document_rule(name, pattern))
    return inferred


def infer_document_rule(name: str, pattern: str) -> dict:
    lower = name.lower()
    role = "support"
    score = 0.5
    reason = "This document may contain related legacy behavior or constraints."
    if any(token in lower for token in ("require", "spec")) or "要件" in name:
        role, score = "requirement", 0.75
        reason = "This document may constrain accepted behavior, terminology, or external requirements."
    elif "detail" in lower or "詳細" in name:
        role, score = "detail_design", 0.75
        reason = "This document may contain downstream constraints that should not be contradicted."
    elif "basic" in lower or "基本" in name:
        role, score = "primary_design", 0.9
        reason = "This document may define existing behavior, parameters, or constraints."
    elif "template" in lower or "テンプレ" in name:
        role, score = "template", 0.4
        reason = "This document may constrain artifact structure or required sections."
    return {
        "path": pattern,
        "type": name,
        "role": role,
        "authority_score": score,
        "approval_status": "approved" if role in {"primary_design", "requirement", "detail_design"} else "unknown",
        "interference_reason": reason,
    }


def classify_document(rel_path: str, suffix: str, profile: dict) -> dict:
    defaults = deepcopy(profile.get("defaults", {}))
    for rule in profile.get("document_rules", []):
        if fnmatch.fnmatch(rel_path, rule.get("path", "")):
            return {**defaults, **rule, "extension": suffix.lower()}
    name = rel_path.split("/", 1)[0] if "/" in rel_path else Path(rel_path).stem
    return {**defaults, **infer_document_rule(name, f"{name}/**"), "extension": suffix.lower()}


def is_heading_candidate(text: str, style: str, index: int, profile: dict) -> bool:
    if index == 0:
        return True
    rules = profile.get("heading_rules", {})
    if style and any(style.startswith(prefix) for prefix in rules.get("style_prefixes", [])):
        return True
    max_length = int(rules.get("max_heading_length", 80))
    endings = tuple(rules.get("sentence_endings", []))
    if len(text) > max_length or (endings and text.endswith(endings)):
        return False
    if text in set(rules.get("heading_aliases", [])):
        return True
    return any(re.search(pattern, text) for pattern in rules.get("number_patterns", []))


def extract_entities(text: str, seeds: str, profile: dict) -> list[str]:
    rules = profile.get("entity_rules", {})
    entities = set()
    for pattern in rules.get("patterns", []):
        entities.update(match.group(0).upper() for match in re.finditer(pattern, text, re.IGNORECASE))
    entities.update(term for term in rules.get("terms", []) if term and term in text)
    if rules.get("auto_from_titles", True):
        token_pattern = rules.get("auto_token_pattern", DEFAULT_PROFILE["entity_rules"]["auto_token_pattern"])
        entities.update(match.group(0) for match in re.finditer(token_pattern, seeds))
    return sorted(entities)[: int(rules.get("max_entities", 12))]


def profile_snapshot(profile: dict) -> str:
    return json.dumps(profile, ensure_ascii=False, sort_keys=True)


def profile_from_db(conn) -> dict:
    row = conn.execute("SELECT value FROM kb_metadata WHERE key = ?", ("profile_snapshot",)).fetchone()
    if not row:
        return deepcopy(DEFAULT_PROFILE)
    return json.loads(row["value"] if hasattr(row, "keys") else row[0])

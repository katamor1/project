# ibm-bob/mode-pack/scripts/context_builder.py
# Builds the IBM-Bob project context packet from copied documents and source files.
# This exists so detailed design and coding stages start from explicit evidence about the current system.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/docx_text.py, ibm-bob/mode-pack/schemas/project-context-packet.schema.json, ibm-bob/mode-pack/scripts/stage_packets.py
from __future__ import annotations

import re
from pathlib import Path

from docx_text import read_docx_paragraphs, summarize_docx
from runtime_common import relative_to_repo

FUNCTION_RE = re.compile(r"\b([A-Za-z_]\w+)\s*\(")

IMPACT_MAP = [
    (
        "異常履歴",
        [
            ("alarm_history", "異常履歴と CSV 退避が必要", ["source/inc/system_shared.h", "source/src/rt_control.c", "source/src/ts_service.c", "source/src/control_api.c"]),
        ],
        ["RT 側で継続異常を重複記録しないこと", "リングバッファ wraparound を壊さないこと"],
    ),
    (
        "原点復帰",
        [
            ("homing_retry", "状態遷移と再試行カウンタが必要", ["source/inc/system_shared.h", "source/src/rt_control.c", "source/src/ts_service.c"]),
        ],
        ["RT の状態遷移肥大化", "ERROR 復帰条件の取り違え"],
    ),
    (
        "パージ",
        [
            ("purge_sequence", "順序依存の段階制御を追加する", ["source/inc/system_shared.h", "source/src/rt_control.c", "source/src/ts_service.c"]),
        ],
        ["安全停止順序の誤り", "中断条件と完了条件の競合"],
    ),
    (
        "先読み補正",
        [
            ("prefetch_extension", "補正要求と結果の受け渡しを広げる", ["source/inc/system_shared.h", "source/src/rt_control.c", "source/src/ts_service.c"]),
        ],
        ["世代番号の取り違え", "未完了時のフォールバック欠落"],
    ),
    (
        "運転モード",
        [
            ("run_mode_api", "外部 API と RT 確定処理の分離が必要", ["source/inc/control_api.h", "source/src/control_api.c", "source/inc/system_shared.h", "source/src/rt_control.c"]),
        ],
        ["外部要求と確定値の混同", "安全条件チェックの欠落"],
    ),
]


def _summarize_source(path: Path) -> tuple[str, list[str]]:
    lines = path.read_text(encoding="utf-8").splitlines()
    summary = " / ".join(line.strip() for line in lines[:8] if line.strip())
    exports: list[str] = []
    for line in lines:
        match = FUNCTION_RE.search(line)
        if match and match.group(1) not in exports:
            exports.append(match.group(1))
        if len(exports) >= 6:
            break
    return summary, exports


def _find_impacts(input_doc: Path) -> tuple[list[dict], list[str]]:
    text = " ".join(read_docx_paragraphs(input_doc))
    modules: list[dict] = []
    risks: list[str] = []
    for keyword, module_defs, risk_defs in IMPACT_MAP:
        if keyword in text:
            for module, reason, files in module_defs:
                modules.append({"module": module, "reason": reason, "candidate_files": files})
            risks.extend(risk_defs)
    if modules:
        return modules, risks
    return (
        [
            {
                "module": "general_change",
                "reason": "基本設計書から個別キーワードを特定できなかったため、全体影響を確認する",
                "candidate_files": ["source/inc/system_shared.h", "source/src/rt_control.c", "source/src/ts_service.c"],
            }
        ],
        ["影響範囲が広いため追加レビューが必要"],
    )


def build_project_context(run_request: dict, workspace_profile: dict) -> dict:
    project_root = Path(run_request["project_root"])
    input_doc = Path(run_request["input_basic_design_doc"])
    existing_docs = []
    for doc_path in sorted((project_root / "docs").glob("*.docx")):
        existing_docs.append(
            {
                "path": relative_to_repo(doc_path),
                "summary": summarize_docx(doc_path, line_limit=6),
            }
        )
    existing_code_findings = []
    for pattern in workspace_profile["source_globs"]:
        for path in sorted(project_root.glob(pattern)):
            if path.suffix not in {".h", ".c"}:
                continue
            summary, exports = _summarize_source(path)
            existing_code_findings.append(
                {
                    "path": relative_to_repo(path),
                    "summary": summary,
                    "exports": exports,
                }
            )
    impacted_modules, impact_risks = _find_impacts(input_doc)
    traceability = [
        {
            "source_ref": relative_to_repo(input_doc),
            "target_ref": item["module"],
        }
        for item in impacted_modules
    ]
    return {
        "run_id": run_request["run_id"],
        "existing_docs": existing_docs,
        "existing_code_findings": existing_code_findings,
        "impacted_modules": impacted_modules,
        "impact_risks": impact_risks,
        "traceability": traceability,
        "next_agent": "ibmbob-detail-design",
    }

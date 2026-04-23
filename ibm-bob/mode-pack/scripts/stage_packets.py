# ibm-bob/mode-pack/scripts/stage_packets.py
# Builds IBM-Bob stage packets and shared review decisions from the copied workspace evidence.
# This exists so local evaluation can produce the same packet shapes that the Bob modes are expected to author.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/context_builder.py, ibm-bob/mode-pack/schemas/review-decision-packet.schema.json, ibm-bob/mode-pack/routing/stage-flow.json
from __future__ import annotations

from pathlib import Path

from docx_text import read_docx_paragraphs


def make_review_packet(run_id: str, stage: str, decision: str, findings: list[dict], revise_to: str, next_agent: str, human_checkpoint: bool) -> dict:
    return {
        "run_id": run_id,
        "stage": stage,
        "decision": decision,
        "findings": findings,
        "revise_to": revise_to,
        "human_checkpoint": human_checkpoint,
        "next_agent": next_agent,
    }


def build_detail_design_packet(run_request: dict, context_packet: dict) -> dict:
    paragraphs = read_docx_paragraphs(Path(run_request["input_basic_design_doc"]))
    summary = " / ".join(paragraphs[1:6])
    return {
        "run_id": run_request["run_id"],
        "eval_case_id": run_request["eval_case_id"],
        "design_summary": summary,
        "api_changes": [{"name": item["module"], "change": item["reason"]} for item in context_packet["impacted_modules"]],
        "shared_memory_changes": [{"name": item["module"], "change": "共有構造と可視化項目を追加する"} for item in context_packet["impacted_modules"]],
        "state_changes": [{"name": item["module"], "change": "基本設計書の状態やイベントに沿って状態管理を追加する"} for item in context_packet["impacted_modules"]],
        "threading_rules": [
            "RT 側は状態遷移と即時制御だけを担当する",
            "TS 側はファイル I/O、ログ整形、補助計算、UI 向け整形を担当する",
        ],
        "error_rules": [
            "異常時は安全側出力を優先する",
            "条件未成立時は current state を維持し、応答コードや状態値だけを更新する",
        ],
        "traceability": context_packet["traceability"],
        "next_agent": "ibmbob-review-gate",
    }


def build_code_change_packet(run_request: dict, context_packet: dict, detail_design: dict) -> dict:
    touched_files = []
    for item in context_packet["impacted_modules"]:
        for path in item["candidate_files"]:
            touched_files.append({"path": path, "reason": item["reason"]})
    unique_files = {item["path"]: item for item in touched_files}
    return {
        "run_id": run_request["run_id"],
        "touched_files": list(unique_files.values()),
        "change_summary": f"{run_request['eval_case_id']} の詳細設計に沿って copied workspace の影響ファイルを更新する",
        "design_refs": [trace["target_ref"] for trace in detail_design["traceability"]],
        "risk_flags": context_packet["impact_risks"],
        "review_targets": ["RT 制約を守れているか", "共有構造と API 契約がずれていないか", "TS 側へ寄せる処理が混ざっていないか"],
        "next_agent": "ibmbob-review-gate",
    }


def build_unit_test_pack(run_request: dict, workspace_profile: dict) -> dict:
    case_id = run_request["eval_case_id"].lower().replace("_", "-")
    return {
        "run_id": run_request["run_id"],
        "test_files": [f"source/tests/test_{case_id}.c"],
        "test_cases": [
            {"case_id": f"{case_id}-01", "title": "要求が受理される", "expected_result": "応答コードや状態値が想定どおり更新される"},
            {"case_id": f"{case_id}-02", "title": "異常条件で安全側へ遷移する", "expected_result": "安全値と異常状態が維持される"},
            {"case_id": f"{case_id}-03", "title": "境界条件で重複処理しない", "expected_result": "重複更新や再記録を起こさない"},
        ],
        "fixtures": ["初期共有メモリ", "異常入力", "正常入力"],
        "coverage_focus": ["RT 側状態遷移", "共有構造更新", "TS 側の後処理"],
        "commands_ref": workspace_profile.get("profile_id", "workspace-profile"),
        "next_agent": "ibmbob-review-gate",
    }


def build_functional_test_pack(run_request: dict, detail_design: dict) -> dict:
    title = detail_design["design_summary"].split(" / ")[0]
    return {
        "run_id": run_request["run_id"],
        "scenarios": [
            {"scenario_id": "FT-01", "title": f"{title} の正常系シナリオ"},
            {"scenario_id": "FT-02", "title": f"{title} の異常系シナリオ"},
        ],
        "preconditions": ["保守または運転条件が設計どおり成立している", "対象要求が共有メモリから参照できる"],
        "steps": ["外部要求を投入する", "RT 側の更新を待つ", "TS 側の副作用を確認する"],
        "expected_results": ["共有メモリの状態値が更新される", "必要なログや応答コードが反映される"],
        "negative_cases": ["条件未成立のまま要求した時に拒否される", "異常発生時に安全側へ停止する"],
        "next_agent": "ibmbob-review-gate",
    }


def build_run_summary(run_request: dict, status: str, stage_results: list[dict], review_refs: list[str], open_issues: list[str], next_action: str) -> dict:
    return {
        "run_id": run_request["run_id"],
        "status": status,
        "stage_results": stage_results,
        "review_refs": review_refs,
        "workspace_copy_root": run_request["workspace_copy_root"],
        "artifacts_root": run_request["artifacts_root"],
        "open_issues": open_issues,
        "next_action": next_action,
        "next_agent": "none",
    }


def review_stage(run_request: dict, stage: str, packet: dict, routes: dict) -> dict:
    if stage == "unit_test_run":
        if packet["status"] == "pass":
            return make_review_packet(run_request["run_id"], stage, "pass", [], "none", routes[stage]["on_pass"], False)
        reroute = routes[stage]["on_revise"][packet["reroute_target"]]
        finding = {"severity": "high", "title": "unit test execution failed", "detail": f"reroute_target={packet['reroute_target']}"}
        return make_review_packet(run_request["run_id"], stage, "revise", [finding], reroute, reroute, False)

    required_lists = {
        "detail_design": ["api_changes", "shared_memory_changes", "state_changes", "threading_rules", "error_rules", "traceability"],
        "code_change": ["touched_files", "design_refs", "risk_flags", "review_targets"],
        "unit_test_pack": ["test_files", "test_cases", "fixtures", "coverage_focus"],
        "functional_test_pack": ["scenarios", "preconditions", "steps", "expected_results", "negative_cases"],
    }
    missing = [field for field in required_lists[stage] if not packet.get(field)]
    if missing:
        finding = {"severity": "high", "title": "required fields missing", "detail": ", ".join(missing)}
        reroute = routes[stage]["on_revise"]
        return make_review_packet(run_request["run_id"], stage, "revise", [finding], reroute, reroute, False)
    return make_review_packet(run_request["run_id"], stage, "pass", [], "none", routes[stage]["on_pass"], False)

# ibm-bob/mode-pack/scripts/evaluate_mode_pack.py
# Runs local IBM-Bob mode-pack evaluations by preparing copied workspaces and writing packet artifacts.
# This exists so the team can smoke-test the stage flow and packet contracts before running Bob in the IDE.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/workspace_run.py, ibm-bob/mode-pack/scripts/stage_packets.py, ibm-bob/mode-pack/scripts/validate_mode_pack.py
from __future__ import annotations

import argparse
from pathlib import Path

from context_builder import build_project_context
from json_schema_lite import LiteSchemaValidator
from packet_renderers import render_packet
from runtime_common import SCHEMA_ROOT, SAMPLES_ROOT, load_jsonc, write_json, write_text
from stage_packets import build_code_change_packet, build_detail_design_packet, build_functional_test_pack, build_run_summary, build_unit_test_pack, make_review_packet, review_stage
from workspace_run import execute_unit_tests, prepare_run

VALIDATOR = LiteSchemaValidator()
ROUTES = load_jsonc((Path(__file__).resolve().parent.parent / "routing" / "stage-flow.json"))["routes"]
SCHEMA_MAP = {
    "run-request": "run-request.schema.json",
    "project-context-packet": "project-context-packet.schema.json",
    "detail-design-packet": "detail-design-packet.schema.json",
    "code-change-packet": "code-change-packet.schema.json",
    "unit-test-pack": "unit-test-pack.schema.json",
    "unit-test-run-packet": "unit-test-run-packet.schema.json",
    "functional-test-pack": "functional-test-pack.schema.json",
    "review-decision-packet": "review-decision-packet.schema.json",
    "run-summary-packet": "run-summary-packet.schema.json",
}


def _write_packet(run_root: Path, name: str, packet_type: str, data: dict) -> str:
    json_path = run_root / f"{name}.json"
    md_path = run_root / f"{name}.md"
    errors = VALIDATOR.validate_file(data, SCHEMA_ROOT / SCHEMA_MAP[packet_type])
    if errors:
        raise ValueError("; ".join(errors))
    write_json(json_path, data)
    write_text(md_path, render_packet(packet_type, data, md_path))
    return str(json_path)


def run_case(input_doc: Path, execution_mode: str) -> dict:
    run_request, workspace_profile, issues = prepare_run(input_doc, execution_mode)
    run_root = Path(run_request["artifacts_root"])
    review_refs: list[str] = []
    stage_results = [{"stage": "run_request", "status": "saved", "artifact_ref": _write_packet(run_root, "00-run-request", "run-request", run_request)}]
    if issues and execution_mode == "real":
        intake_review = make_review_packet(run_request["run_id"], "intake", "block", [{"severity": "high", "title": "toolchain missing", "detail": issues[0]}], "human_checkpoint", "human_checkpoint", True)
        review_refs.append(_write_packet(run_root, "01-review-intake", "review-decision-packet", intake_review))
        stage_results.append({"stage": "intake", "status": "blocked", "artifact_ref": review_refs[-1]})
        summary = build_run_summary(run_request, "blocked", stage_results, review_refs, issues, "toolchain を用意して intake から再開する")
        stage_results.append({"stage": "run_summary", "status": "saved", "artifact_ref": _write_packet(run_root, "99-run-summary", "run-summary-packet", summary)})
        return summary

    context_packet = build_project_context(run_request, workspace_profile)
    stage_results.append({"stage": "context_scout", "status": "saved", "artifact_ref": _write_packet(run_root, "01-project-context", "project-context-packet", context_packet)})
    detail_design = build_detail_design_packet(run_request, context_packet)
    stage_results.append({"stage": "detail_design", "status": "saved", "artifact_ref": _write_packet(run_root, "02-detail-design", "detail-design-packet", detail_design)})
    review = review_stage(run_request, "detail_design", detail_design, ROUTES)
    review_refs.append(_write_packet(run_root, "03-review-detail-design", "review-decision-packet", review))
    code_change = build_code_change_packet(run_request, context_packet, detail_design)
    stage_results.append({"stage": "code_change", "status": "saved", "artifact_ref": _write_packet(run_root, "04-code-change", "code-change-packet", code_change)})
    review = review_stage(run_request, "code_change", code_change, ROUTES)
    review_refs.append(_write_packet(run_root, "05-review-code-change", "review-decision-packet", review))
    unit_test_pack = build_unit_test_pack(run_request, workspace_profile)
    stage_results.append({"stage": "unit_test_pack", "status": "saved", "artifact_ref": _write_packet(run_root, "06-unit-test-pack", "unit-test-pack", unit_test_pack)})
    review = review_stage(run_request, "unit_test_pack", unit_test_pack, ROUTES)
    review_refs.append(_write_packet(run_root, "07-review-unit-test-pack", "review-decision-packet", review))
    unit_test_run = execute_unit_tests(run_request, workspace_profile)
    stage_results.append({"stage": "unit_test_run", "status": unit_test_run["status"], "artifact_ref": _write_packet(run_root, "08-unit-test-run", "unit-test-run-packet", unit_test_run)})
    review = review_stage(run_request, "unit_test_run", unit_test_run, ROUTES)
    review_refs.append(_write_packet(run_root, "09-review-unit-test-run", "review-decision-packet", review))
    if review["decision"] != "pass":
        summary = build_run_summary(run_request, "blocked", stage_results, review_refs, [f"unit_test_run rerouted to {review['next_agent']}"], "review packet の差し戻し先から再開する")
        stage_results.append({"stage": "run_summary", "status": "saved", "artifact_ref": _write_packet(run_root, "99-run-summary", "run-summary-packet", summary)})
        return summary
    functional_test_pack = build_functional_test_pack(run_request, detail_design)
    stage_results.append({"stage": "functional_test_pack", "status": "saved", "artifact_ref": _write_packet(run_root, "10-functional-test-pack", "functional-test-pack", functional_test_pack)})
    review = review_stage(run_request, "functional_test_pack", functional_test_pack, ROUTES)
    review_refs.append(_write_packet(run_root, "11-review-functional-test-pack", "review-decision-packet", review))
    summary = build_run_summary(run_request, "completed", stage_results, review_refs, [], "Bob IDE または Shell で copied workspace を開いて実作業を継続する")
    stage_results.append({"stage": "run_summary", "status": "saved", "artifact_ref": _write_packet(run_root, "99-run-summary", "run-summary-packet", summary)})
    return summary


def _docs_for_case(case_arg: str) -> list[Path]:
    docs = sorted((SAMPLES_ROOT / "評価用基本設計書").glob("*.docx"))
    if case_arg == "all":
        return docs
    return [next(path for path in docs if path.stem.endswith(f"_{case_arg}"))]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--case", default="all")
    parser.add_argument("--execution-mode", choices=["simulate", "real"], default="simulate")
    args = parser.parse_args()
    for doc in _docs_for_case(args.case):
        summary = run_case(doc, args.execution_mode)
        print(f"{doc.name}: {summary['status']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

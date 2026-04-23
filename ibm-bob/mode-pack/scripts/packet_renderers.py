# ibm-bob/mode-pack/scripts/packet_renderers.py
# Renders IBM-Bob packet JSON into simple Markdown views for human review.
# This exists so each run leaves both machine-readable JSON and easy-to-scan Markdown artifacts.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/evaluate_mode_pack.py, ibm-bob/mode-pack/scripts/stage_packets.py, ibm-bob/mode-pack/README.md
from __future__ import annotations

from pathlib import Path

from runtime_common import relative_to_repo


def _header(output_path: Path, packet_type: str) -> list[str]:
    return [
        f"<!-- {relative_to_repo(output_path)} -->",
        f"<!-- Generated Markdown view for {packet_type}. -->",
        "<!-- This exists so IBM-Bob packet reviewers can read the saved JSON quickly. -->",
        "<!-- RELEVANT FILES: .bob/ibm-bob/current_run.json, .bob/ibm-bob/stage-flow.json -->",
    ]


def _bullets(items: list[str]) -> list[str]:
    return [f"- {item}" for item in items] if items else ["- none"]


def render_packet(packet_type: str, data: dict, output_path: Path) -> str:
    lines = _header(output_path, packet_type)
    lines.append(f"# {packet_type}")

    if packet_type == "run-request":
        lines += [
            "",
            f"- run_id: `{data.get('run_id', '')}`",
            f"- project_root: `{data['project_root']}`",
            f"- input_basic_design_doc: `{data['input_basic_design_doc']}`",
            f"- workspace_profile_ref: `{data['workspace_profile_ref']}`",
            f"- execution_mode: `{data['run_policy']['execution_mode']}`",
        ]
    elif packet_type == "project-context-packet":
        lines += ["", "## Docs", ""]
        lines += [f"- `{item['path']}`: {item['summary']}" for item in data["existing_docs"]]
        lines += ["", "## Code", ""]
        lines += [f"- `{item['path']}`: {item['summary']}" for item in data["existing_code_findings"]]
        lines += ["", "## Impact", ""]
        lines += [
            f"- `{item['module']}`: {item['reason']} ({', '.join(item['candidate_files'])})"
            for item in data["impacted_modules"]
        ]
    elif packet_type == "detail-design-packet":
        lines += ["", "## Summary", "", data["design_summary"], "", "## Threading Rules", ""]
        lines += _bullets(data["threading_rules"])
        lines += ["", "## Error Rules", ""]
        lines += _bullets(data["error_rules"])
    elif packet_type == "code-change-packet":
        lines += ["", data["change_summary"], "", "## Touched Files", ""]
        lines += [f"- `{item['path']}`: {item['reason']}" for item in data["touched_files"]]
        lines += ["", "## Risks", ""]
        lines += _bullets(data["risk_flags"])
    elif packet_type == "unit-test-pack":
        lines += ["", "## Test Files", ""]
        lines += _bullets(data["test_files"])
        lines += ["", "## Coverage Focus", ""]
        lines += _bullets(data["coverage_focus"])
    elif packet_type == "unit-test-run-packet":
        lines += [
            "",
            f"- status: `{data['status']}`",
            f"- exit_code: `{data['exit_code']}`",
            f"- reroute_target: `{data['reroute_target']}`",
            "",
            "## Logs",
            "",
        ]
        lines += _bullets(data["log_refs"])
    elif packet_type == "functional-test-pack":
        lines += ["", "## Scenarios", ""]
        lines += [f"- `{item['scenario_id']}`: {item['title']}" for item in data["scenarios"]]
        lines += ["", "## Negative Cases", ""]
        lines += _bullets(data["negative_cases"])
    elif packet_type == "review-decision-packet":
        lines += [
            "",
            f"- stage: `{data['stage']}`",
            f"- decision: `{data['decision']}`",
            f"- revise_to: `{data['revise_to']}`",
            f"- next_agent: `{data['next_agent']}`",
            "",
            "## Findings",
            "",
        ]
        lines += [f"- [{item['severity']}] {item['title']}: {item['detail']}" for item in data["findings"]]
        if not data["findings"]:
            lines.append("- none")
    elif packet_type == "run-summary-packet":
        lines += [
            "",
            f"- status: `{data['status']}`",
            f"- workspace_copy_root: `{data['workspace_copy_root']}`",
            "",
            "## Stage Results",
            "",
        ]
        lines += [f"- `{item['stage']}`: {item['status']}" for item in data["stage_results"]]
        lines += ["", "## Open Issues", ""]
        lines += _bullets(data["open_issues"])
        lines += ["", "## Next Action", "", data["next_action"]]
    return "\n".join(lines) + "\n"

---
name: speca-review-reporting
description: Use when reviewing SPECA audit findings, filtering false positives, calibrating severity, generating PoCs, or drafting bug-bounty/audit reports.
---

# SPECA Review and Reporting

Use this skill for Phase `04` review and manual phases `05`, `06`, and `06b`.

## Phase 04 recall-safe gates

Apply exactly these gates in order:

1. Dead Code Gate.
2. Trust Boundary Gate.
3. Scope Gate.

Only these gates may produce `DISPUTED_FP`. Otherwise classify as `CONFIRMED_VULNERABILITY`, `CONFIRMED_POTENTIAL`, `DOWNGRADED`, `NEEDS_MANUAL_REVIEW`, or `PASS_THROUGH`.

## Commands

```bash
uv run python3 scripts/run_phase.py --phase 04 --workers 2 --max-concurrent 8
```

Manual reporting uses prompts directly:

- `references/prompts/05_poc.md`
- `references/prompts/06_report.md`
- `references/prompts/06b_audit_report.md`

## Reporting standard

Every report must include property provenance, affected code, reproduction or proof sketch, impact, assumptions, scope status, and remediation guidance. Avoid exploit operationalization beyond the user’s authorized audit purpose.

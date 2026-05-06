---
name: speca-spec-discovery
description: Use when discovering protocol specifications, whitepapers, architecture docs, EIPs, RFC-style documents, or technical docs from seed URLs.
---

# SPECA Specification Discovery

Phase `01a` finds technical specification documents from seed URLs.

## Inputs

- One or more seed URLs via `SPEC_URLS`.
- Optional keywords via `KEYWORDS`.

## Procedure

1. Fetch the seed page and extract links whose titles or paths indicate specifications, architecture, protocol docs, standards, whitepapers, EIPs, RFCs, yellow papers, or developer documentation.
2. Recurse 2-3 levels only when links remain relevant.
3. Deduplicate canonicalized URLs.
4. Record title, URL, source seed, and visited URLs.
5. Write JSON to the expected phase output path or run the existing phase runner.

## Command

```bash
SPEC_URLS="https://example.org/specs" KEYWORDS="protocol,specification,architecture"   uv run python3 scripts/run_phase.py --phase 01a --workers 1
```

## Reference prompt

Use `references/prompts/01a_crawl.md` for the exact worker behavior. The original Claude Code skill is preserved at `references/original-claude/skills/spec-discovery/SKILL.md`.

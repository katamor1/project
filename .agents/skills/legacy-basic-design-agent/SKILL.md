---
name: legacy-basic-design-agent
description: Build and query a local SQLite knowledge base from legacy requirements, basic-design docs, templates, DOCX files, and PDF manuals. Use when Codex needs to investigate existing design impact, interference, source-backed evidence, or related legacy specs before drafting a basic design for a feature addition.
---

<!-- .agents/skills/legacy-basic-design-agent/SKILL.md -->
<!-- Explains how to build and query a local legacy design KB before basic design work. -->
<!-- This exists so agents read maps and bounded evidence instead of loading all source docs. -->
<!-- RELEVANT FILES: .agents/skills/legacy-basic-design-agent/scripts/build_kb.py, .agents/skills/legacy-basic-design-agent/scripts/query_kb.py, .copilot/prompts/sdlc/ibmbob-sdlc-basic-design-author.md -->
# Legacy Basic Design Agent

## Overview

Use this skill before drafting a basic design for a feature addition.

It builds a small SQLite KB from existing design docs, then returns only the relevant evidence.

Project-specific document types, headings, terms, and ranking weights belong in a YAML profile.

If no profile is supplied, the builder infers a starter profile from folder names, extensions, DOCX styles, and heading-like lines.

## Workflow

1. Build the KB if it does not exist or source docs changed.

```powershell
py -3 .agents/skills/legacy-basic-design-agent/scripts/build_kb.py `
  --docs-root C:\path\to\docs `
  --out-db C:\path\to\legacy-basic-design.legacy-kb.sqlite
```

Use `--profile .agents/skills/legacy-basic-design-agent/profiles/cnc-sample.yaml` for the CNC sample docs.

Use `--write-profile C:\path\to\starter.yaml` to write the inferred profile for human adjustment.

2. Query the KB with the new feature description.

```powershell
py -3 .agents/skills/legacy-basic-design-agent/scripts/query_kb.py `
  --db C:\path\to\legacy-basic-design.legacy-kb.sqlite `
  --query "G71 roughing cycle existing basic design impact" `
  --domain cnc
```

`query_kb.py` uses the DB profile snapshot by default.

Pass `--profile C:\path\to\profile.yaml` only when you want to override it.

3. Read the returned JSON in this order:

- `retrieval_plan`: why these sources were selected.
- `impact_candidates`: likely existing design interference.
- `evidence_bundle.retrieved_items`: bounded source evidence.
- `artifact_context_packet`: allowed assertions and open questions for basic design.

## Rules

- Do not load all raw documents into context.
- Start from the KB map, then candidate sections, then leaf chunks.
- Use `role`, `authority_score`, and `interference_reason` from the KB instead of hard-coded document type assumptions.
- Keep sample-specific words, headings, and folder rules in `profiles/*.yaml`, not in Python code.
- Stop and report `open_questions` when confidence is low.

## PDF And OCR

`build_kb.py` uses `pdftotext` first.

If extracted text quality is low, it tries OCR with `pdftoppm` and `tesseract`.

OCR requires `tesseract` plus `jpn+eng` language data.

If OCR tools are missing, the document is still recorded with an error status.

The same error is written to the generated `.legacy-kb.log.jsonl` file.

## Output Contract

The query output intentionally mirrors the existing IBM Bob source-backed flow:

- `retrieval_plan`
- `evidence_bundle`
- `artifact_context_packet`
- `impact_candidates`

Use those fields as the handoff into basic design work.

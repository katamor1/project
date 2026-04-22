<!-- docs/external-runtime/legacy-kb/security-acl.md -->
<!-- Describes the ACL and security checks for ingest, retrieval, and evidence handoff. -->
<!-- This exists so the runtime can block unsafe evidence before it reaches generation or review lanes. -->
<!-- RELEVANT FILES: docs/external-runtime/legacy-kb/azure-topology.md, .copilot/schemas/acl-principal-set.schema.json, .copilot/prompts/K8_evidence_reviewer.md -->
# Security And ACL

## Core Rules
- ACL は `document_record` と `chunk_record` の両方に保持する
- query 時点で filter する
- `evidence_bundle` 作成前に再確認する
- downstream handoff 前に `K8` が最終確認する

## Query-Time Trim
- principal set は user / group / tenant で受ける
- Azure AI Search には security filter を渡す
- trim 後の doc だけ rerank する

## Evidence-Time Recheck
- retrieved item ごとに ACL principal を再照合する
- authority が弱い source は単独採用しない
- conflict があれば `risk_flags` に残す

## Mandatory Blocks
- principal mismatch
- sensitivity label conflict
- conflicting authoritative source
- missing approval status on high-risk source

## Audit Notes
- trace には `retrieved_doc_ids` と decision flag を残す
- 本文全文は長期保存しない
- incident 時は `run_id` から query plan と ACL verdict を引けるようにする

<!-- docs/copilot-studio/legacy-search/readiness/triage-playbook.md -->
<!-- This file defines the pilot triage playbook for recurring problem types. -->
<!-- This file exists to help the team identify the first file to fix when pilot issues appear. -->
<!-- RELEVANT FILES: docs/copilot-studio/legacy-search/routing-matrix.md, docs/copilot-studio/legacy-search/shared-io-contract.yaml, docs/copilot-studio/legacy-search/tests/evaluation-set.yaml -->
# Triage Playbook

## 誤ルーティング

まず確認する原因:

- `LS-01` の意図分類が広すぎる
- route 定義と評価ケースがずれている

最初に直すファイル:

- `routing-matrix.md`
- `agents/intake-router.md`

再確認する評価ケース:

- `Q11`, `Q12`, `Q13`, `Q14`, `O05`

## 用語正規化不足

まず確認する原因:

- 別名や旧画面名が `entities` に乗っていない
- 曖昧語の扱いが弱い

最初に直すファイル:

- `agents/glossary-normalizer.md`
- `shared-io-contract.yaml`

再確認する評価ケース:

- `Q03`, `Q04`

## 資料候補不足

まず確認する原因:

- `include_paths` が狭すぎる
- locator の候補抽出基準が弱い

最初に直すファイル:

- `readiness/knowledge-source-catalog.yaml`
- `agents/document-locator.md`

再確認する評価ケース:

- `Q09`, `Q10`, `Q19`, `Q20`

## 根拠検証不足

まず確認する原因:

- 競合資料を落としている
- 承認状態や鮮度の見方が弱い

最初に直すファイル:

- `agents/evidence-verifier.md`
- `shared-io-contract.yaml`

再確認する評価ケース:

- `Q05`, `Q06`, `O02`, `O03`, `O04`

## 転記テンプレ不備

まず確認する原因:

- `chat_response` が冗長
- `pm_copy_template` が設計メモへ貼りにくい

最初に直すファイル:

- `agents/grounded-answerer.md`
- `agents/gap-reporter.md`
- `agents/renewal-impact-mapper.md`
- `agents/change-diff-scout.md`

再確認する評価ケース:

- `Q01`, `Q15`, `Q17`, `O01`

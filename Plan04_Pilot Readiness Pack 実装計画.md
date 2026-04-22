# Pilot Readiness Pack 実装計画

## Summary
- 次フェーズでは、`docs/copilot-studio/legacy-search/readiness/` を新設し、`Pilot をすぐ始めるための最小運用台帳` をまとめて追加する。
- 今回は `Template first` で進める。実 URL、実担当者、実サイト名は埋めず、すぐ記入できる雛形に固定する。
- 既存の `implementation-guide.md`, `pilot-runbook.md`, `tests/evaluation-set.yaml` は土台として使い、新しい readiness 資料から参照する形にする。既存ルーティングや prompt 本体はこのフェーズでは変えない。

## Key Changes
- `readiness/knowledge-onboarding.md` を追加する。
- 目的は `1 系統 knowledge` を決める前の確認を標準化すること。
- 内容は `対象ドメイン`, `対象 SharePoint / OneDrive`, `含める範囲`, `除外範囲`, `承認済み資料の見分け方`, `旧版資料の扱い`, `更新責任者`, `公開前 block 条件` に固定する。
- 記入形式は Markdown の短いチェックリスト + 記入欄にする。
- `readiness/knowledge-source-catalog.yaml` を追加する。
- 目的は knowledge source の実体台帳を 1 つに集約すること。
- `sources[]` 配列を持つ YAML にする。
- 各 source の必須キーは `domain`, `source_name`, `source_type`, `base_url`, `include_paths`, `exclude_paths`, `owner_role`, `approval_signal`, `freshness_rule`, `risk_notes`, `description_template` に固定する。
- `source_name` は既存ルールどおり `<domain>-legacy-search-main` 形式に固定する。
- `source_type` は `sharepoint` または `onedrive` のみ許可する。
- `description_template` は 6 行固定にする。
  `対象業務`, `含む文書`, `含まない文書`, `主な用語`, `更新責任者`, `鮮度の目安`
- `readiness/pilot-scorecard.yaml` を追加する。
- 目的は 25 ケースの実行結果を機械的に記録すること。
- `runs[]` 配列を持つ YAML にする。
- 25 ケース分をあらかじめ空欄で持たせる。
- 各 run の必須キーは `case_id`, `case_type`, `pilot_user`, `used_route`, `expected_route_or_behavior`, `matched_expectation`, `abstained_as_expected`, `speculative_answer`, `conflict_missed`, `handoff_packet_ok`, `final_output_ok`, `template_usable`, `notes` に固定する。
- `case_type` は `acceptance` または `operational` に固定する。
- `matched_expectation`, `abstained_as_expected`, `speculative_answer`, `conflict_missed`, `handoff_packet_ok`, `final_output_ok`, `template_usable` は `yes | no | n/a` の 3 値に固定する。
- `readiness/triage-playbook.md` を追加する。
- 目的は Pilot で出た不具合の直し先を迷わず決めること。
- 不具合分類は 5 つに固定する。
  `誤ルーティング`, `用語正規化不足`, `資料候補不足`, `根拠検証不足`, `転記テンプレ不備`
- 各分類に対して `まず確認する原因`, `最初に直すファイル`, `再確認する評価ケース` を 1 セットで書く。
- 修正先は既存ファイルへ明示的に紐づける。
  `routing-matrix.md`, `agents/glossary-normalizer.md`, `agents/document-locator.md`, `agents/evidence-verifier.md`, `shared-io-contract.yaml` と最終系 agent prompt を主対象にする。
- `readiness/go-no-go-checklist.md` を追加する。
- 目的は Pilot 後の公開判断を 1 枚で残すこと。
- 内容は `前提確認`, `評価結果`, `重大論点`, `差戻し理由`, `判定`, `確認者`, `日付`, `次アクション` に固定する。
- 判定値は `go`, `conditional go`, `no-go` の 3 つに固定する。
- 既存ドキュメントとの接続を追加する。
- `README.md` に `readiness/` フォルダへの導線を 1 か所追加する。
- `implementation-guide.md` から `knowledge-source-catalog.yaml` と `knowledge-onboarding.md` を参照する。
- `pilot-runbook.md` から `pilot-scorecard.yaml`, `triage-playbook.md`, `go-no-go-checklist.md` を参照する。
- `evaluation-set.yaml` 自体はケース内容を変えず、scorecard がその 25 ケースを記録対象として使う前提にする。

## Test Plan
- 静的確認を行う。
- `readiness/` に 5 ファイルが追加されていることを確認する。
- 全ファイルが 300 行未満であることを確認する。
- YAML 2 本に required key 一覧がそろっていることを確認する。
- 運用整合を確認する。
- `knowledge-source-catalog.yaml` の `source_name` が `<domain>-legacy-search-main` 形式で書ける構造になっていることを確認する。
- `pilot-scorecard.yaml` が `Q01-Q20` と `O01-O05` を記録できる形になっていることを確認する。
- `triage-playbook.md` の各不具合分類に、修正先ファイルと再確認ケースがあることを確認する。
- `go-no-go-checklist.md` に `go`, `conditional go`, `no-go` の判定欄があることを確認する。
- 受入条件は次で固定する。
- readiness 資料だけで Pilot 前準備の担当分担、knowledge 範囲整理、評価記録、問題分類、公開判定まで一巡できる。
- scorecard だけを見て 25 ケースの実施結果を欠落なく記録できる。
- triage playbook だけを見て、どの問題をどのファイルから直すか判断できる。

## Assumptions
- 今回は `Readiness Pack` に限定し、prompt 本体や routing の仕様変更は行わない。
- `Template first` を守り、実 URL、実担当者、実サイト名、実権限グループは埋め込まない。
- readiness 資料はすべて `docs/copilot-studio/legacy-search/readiness/` に集約する。
- structured な台帳は YAML、説明資料は Markdown に統一する。
- Pilot は既存前提どおり `5 人以内`, `25 ケース`, `LS-01 Intake Router 起点` のまま変えない。

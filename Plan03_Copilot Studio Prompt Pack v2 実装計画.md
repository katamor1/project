# Copilot Studio Prompt Pack v2 実装計画

## Summary
- 既存の `docs/claude-code/legacy-search/` を `運用できる Prompt Pack v2` に引き上げる。
- 変更対象は `README`, `shared-io-contract.yaml`, `routing-matrix.md`, `agents/` 配下の 8 prompt, `tests/evaluation-set.yaml`。新規追加は `implementation-guide.md` と `pilot-runbook.md` の 2 本に固定する。
- v2 の主変更は `handoff_packet` の全 agent 必須化、`LS-01` から `LS-08` の実運用名への統一、最終系 4 agent の `chat_response + pm_copy_template` 対応、Pilot team 向け運用手順の文書化。
- すべての Markdown / YAML ファイルは既存ルールに合わせて 4 行のヘッダーコメントを持たせ、1 ファイル 300 行未満を維持する。

## Key Changes
- `shared-io-contract.yaml` を `version: 2` に更新する。
- `next_agent_values` は論理名ではなく実運用名に固定する。
- 許可値は `LS-01 Intake Router`, `LS-02 Glossary Normalizer`, `LS-03 Document Locator`, `LS-04 Evidence Verifier`, `LS-05 Grounded Answerer`, `LS-06 Gap Reporter`, `LS-07 Renewal Impact Mapper`, `LS-08 Change Diff Scout`, `human_follow_up`, `complete`。
- 新しい共通項目 `handoff_packet` を追加する。
- `handoff_packet` の必須キーは `question`, `goal`, `entities`, `source_scope`, `known_constraints`, `candidate_sources`, `unknowns`, `next_agent` に固定する。
- `candidate_sources` は空配列を許可する。Router と Glossary でも必ず返す。
- 検索系出力は `candidate_sources`, `confidence`, `unknowns`, `next_agent`, `handoff_packet` を必須にする。
- 最終系出力は `chat_response`, `pm_copy_template`, `evidence`, `confidence`, `unknowns`, `next_agent`, `handoff_packet` を必須にする。
- `chat_response` は Markdown 文字列で返す。
- 見出しは必ず `## 結論`, `## 根拠`, `## 不明点`, `## 次アクション` の 4 つ。
- `pm_copy_template` は Markdown 文字列で返す。
- 行ラベルは必ず `確認事項:`, `確認結果:`, `根拠資料:`, `未解決:`, `次に見る agent:` の 5 行。
- `README.md` は v2 前提に更新する。
- Prompt Pack 名はそのまま維持しつつ、実運用名の一覧を追加する。
- `Router 起点`, `Pilot team 限定`, `1 系統 knowledge`, `chat + template copy`, `人手で handoff_packet を貼り替える` を明記する。
- `routing-matrix.md` は既存ルートを維持しつつ運用ルールを足す。
- `LS-01 Intake Router` だけが新規会話の入口であることを明記する。
- `LS-02` 以降を直接開いた場合は、正常処理せず `LS-01 Intake Router` へ戻す方針を明記する。
- `complete` は最終系 agent のみ、`human_follow_up` は `LS-06 Gap Reporter` のみを既定にする。
- 8 本の prompt を以下の方針で更新する。
- `LS-01 Intake Router`: `handoff_packet` を生成する。`candidate_sources` は空配列。`next_agent` は `LS-02`, `LS-03`, `LS-06`, `LS-07`, `LS-08` のいずれか。
- `LS-02 Glossary Normalizer`: 正規化結果を `handoff_packet.entities` に反映する。曖昧なら `LS-06`。
- `LS-03 Document Locator`: 3〜5 件を目安に `candidate_sources[]` を返し、同じ内容を `handoff_packet.candidate_sources` にコピーする。
- `LS-04 Evidence Verifier`: `candidate_sources` を絞り直し、採用候補だけを `handoff_packet.candidate_sources` に残す。競合や根拠不足は `LS-06`。
- `LS-05 Grounded Answerer`: `answer` は廃止し、`chat_response` と `pm_copy_template` に置き換える。`next_agent` は既定で `complete`。
- `LS-06 Gap Reporter`: `answer` は廃止し、`chat_response` と `pm_copy_template` に置き換える。`needed_sources` は維持する。`next_agent` は既定で `human_follow_up`。
- `LS-07 Renewal Impact Mapper`: `answer` は廃止し、`impact_areas` を残しつつ `chat_response` と `pm_copy_template` を返す。`next_agent` は既定で `complete`。
- `LS-08 Change Diff Scout`: `answer` は廃止し、`diff_items` を残しつつ `chat_response` と `pm_copy_template` を返す。`next_agent` は既定で `complete`。
- `LS-02` 以降の 7 agent には共通のガードを入れる。
- `handoff_packet` がない、または必須入力が欠ける場合は処理を進めず、低信頼で `next_agent: LS-01 Intake Router` を返す。
- `implementation-guide.md` を新規作成する。
- 内容は `8 agent の配置`, `knowledge source 命名規則`, `description テンプレート`, `agent-level 設定`, `Web Search/general knowledge の ON/OFF 方針`, `公開手順`, `Pilot 公開条件`, `block 条件` に固定する。
- knowledge source 名は `<domain>-legacy-search-main` 形式に固定する。
- knowledge source 説明文テンプレートは `対象業務`, `含む文書`, `含まない文書`, `主な用語`, `更新責任者`, `鮮度の目安` の 6 行を必須にする。
- 設定方針は検索精度優先で統一する。
- 全 8 agent で `Web Search` を無効化する。
- 全 8 agent で `Use general knowledge` を無効化する。
- 知識探索の差を作らないため、knowledge source は全 agent 共通で `agent-level` に載せる。
- `pilot-runbook.md` を新規作成する。
- `Maker`, `Knowledge owner`, `Pilot user` の役割を定義する。
- Pilot 実施順は `LS-01 から開始`, `handoff_packet を次 agent に貼る`, `結果を runbook 内の記録表へ転記`, `誤推測 / 誤ルーティング / 競合見逃し / 転記しにくさ` を記録、の順に固定する。
- Pilot は 5 人以内、各 5 問で開始する。
- Pilot 公開前の block 条件は `Restricted SharePoint Search が有効`, `guest user 前提`, `knowledge source が Ready でない`, `権限検証に失敗` の 4 つに固定する。
- `tests/evaluation-set.yaml` を拡張する。
- 既存 20 問は維持する。
- 追加で `operational_checks` セクションを作り、5 ケースを追加する。
- ケースは `初回接続で権限確認が必要`, `source が Ready 前`, `旧版と新版が混在`, `sensitivity label / 保護で回答不可`, `途中 agent を直接開いた` に固定する。
- `pass_criteria` に 3 項目を追加する。
- `All agents return handoff_packet with no missing required keys`
- `Final agents always return chat_response and pm_copy_template`
- `Non-router direct entry is redirected to LS-01 Intake Router without speculative answer`

## Test Plan
- 静的確認を先に行う。
- 8 prompt すべてに既存の 8 セクションが残っていることを確認する。
- 8 prompt すべての JSON 例に `handoff_packet` が含まれていることを確認する。
- 最終系 4 prompt の JSON 例に `chat_response` と `pm_copy_template` が含まれていることを確認する。
- `shared-io-contract.yaml` の `next_agent_values` が `LS-01` から `LS-08` に更新されていることを確認する。
- 評価セットは 25 ケースで実施する。
- 20 問の既存受入ケースでルーティングと abstain を確認する。
- 5 問の運用ケースで direct entry, permission, Ready 状態, 競合, 保護文書の停止動作を確認する。
- 合格条件は固定する。
- 25 ケース中 23 ケース以上で `expected_route` または `expected_behavior` と一致する。
- abstain 対象は 100% で推測回答なし。
- 最終系回答は 100% で `chat_response`, `pm_copy_template`, `evidence or needed_sources`, `handoff_packet` を持つ。
- 直接起動された非 Router agent は 100% で `LS-01 Intake Router` へ戻す。
- runbook に記録された Pilot 5 人分の結果から、重大論点が `誤推測 0`, `競合見逃し 0` を満たした時だけ Pilot team 公開へ進める。

## Assumptions
- 既存ファイル名は維持し、運用名だけを `LS-01` から `LS-08` にする。
- JSON キーは英語のまま維持し、ユーザー向けの表示だけ日本語にする。
- `chat_response` と `pm_copy_template` はオブジェクトではなく Markdown 文字列で返す。
- v1 は 1 系統の SharePoint / OneDrive knowledge を共通利用し、部門別や複数 knowledge への分岐は入れない。
- v1 は `Copilot Only` を守り、外部オーケストレータ、自動ログ基盤、自動転記、コードリポジトリ連携は追加しない。
- 非 Router agent の direct entry は UI 上では起こり得る前提で扱い、拒否ではなく `LS-01 Intake Router` へ戻す誘導で統一する。

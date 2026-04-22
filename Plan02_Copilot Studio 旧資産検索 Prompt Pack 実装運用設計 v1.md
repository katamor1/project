# Copilot Studio 旧資産検索 Prompt Pack 実装運用設計 v1

## Summary
- 次フェーズは、既存の Prompt Pack を `Copilot Studio` 上で実運用できる形に落とす `運用設計書` を作る。
- 構成は `独立した 8 agent`。`Pilot team` 限定で公開し、主な knowledge は `1 系統の SharePoint / OneDrive` を全 agent に共通で持たせる。
- 利用フローは `Intake Router` を唯一の入口に固定する。hidden handoff は使わず、各 agent が返す `handoff_packet` を人が次 agent に貼り替える。
- v1 の出力は `chat + template copy`。最終系 agent は短い結論に加えて、PM がそのまま設計メモへ転記できる定型ブロックを返す。

## Key Changes
- `8 agent` を独立作成する。
- 推奨名は `LS-01 Intake Router`, `LS-02 Glossary Normalizer`, `LS-03 Document Locator`, `LS-04 Evidence Verifier`, `LS-05 Grounded Answerer`, `LS-06 Gap Reporter`, `LS-07 Renewal Impact Mapper`, `LS-08 Change Diff Scout` に固定する。
- `Intake Router` だけを新規会話の入口にする。
- Pilot 利用者には「まず Router から始める」を運用ルールとして徹底する。
- 全 agent に同じ knowledge を `agent-level` で設定する。
- 対象は 1 つの主要 SharePoint サイト / OneDrive フォルダ群に固定する。
- knowledge source 名は一意にし、説明文は詳細に書く。
- 説明文テンプレートは `対象業務`, `含む文書`, `含まない文書`, `主な用語`, `更新責任者`, `鮮度の目安` を含める。
- 検索精度を優先する agent では `Web Search` を無効化し、agent-level の `Use general knowledge` も無効化する。
- `Grounded Answerer`, `Gap Reporter`, `Renewal Impact Mapper`, `Change Diff Scout` は一般知識補完を許可しない。
- 共通 I/O 契約を `shared-io-contract v2` として拡張する。
- 入力は既存どおり `question`, `goal`, `entities`, `source_scope`, `known_constraints` を維持する。
- 全 agent は出力で `next_agent` を必須にする。
- 全 agent は `handoff_packet` を必須にする。
- `handoff_packet` の固定項目は `question`, `goal`, `entities`, `source_scope`, `known_constraints`, `candidate_sources`, `unknowns`, `next_agent` とする。
- 検索系 agent は `candidate_sources[]` を返し、各要素は `title`, `location`, `section_or_page`, `why_relevant`, `authority`, `freshness`, `conflict_flag` を持つ。
- 最終系 agent は `chat_response` と `pm_copy_template` を返す。
- `chat_response` は `結論`, `根拠`, `不明点`, `次アクション` の 4 区画に固定する。
- `pm_copy_template` は `確認事項`, `確認結果`, `根拠資料`, `未解決`, `次に見る agent` の 5 行テンプレートに固定する。
- ルーティングは現在の matrix を維持する。
- 通常検索は `Router -> Glossary -> Locator -> Verifier -> Answerer`。
- 根拠不足や資料競合は `Gap Reporter` で停止する。
- 刷新観点は `Renewal Impact Mapper`、新要求差分は `Change Diff Scout` に送る。
- Pilot 運用では 3 役を明確にする。
- `Maker` は agent 設定と publish を担当する。
- `Knowledge owner` は SharePoint / OneDrive の範囲、名称、説明文、権限を管理する。
- `Pilot user` は Router 起点で利用し、評価シートに結果を残す。
- 実装順は固定する。
- 1. 共通知識設定と Router を作る。
- 2. Glossary, Locator, Verifier, Answerer, Gap を作る。
- 3. Renewal と Diff を作る。
- 4. 20 問評価を通した後に Pilot team へ公開する。

## Test Plan
- 既存の 20 問評価セットを正式な受入セットにする。
- `通常検索`, `用語揺れ`, `資料競合`, `根拠不足`, `Excel/PDF`, `広すぎる質問`, `対象外`, `刷新観点`, `差分確認`, `資料特定` をすべて実施する。
- 追加の運用確認を 5 本入れる。
- 初回接続で権限確認が必要なケース。
- SharePoint / OneDrive source が `Ready` になる前のケース。
- 古い資料と新しい資料が並ぶケース。
- sensitivity label や保護で実質回答できないケース。
- Router を飛ばして途中 agent を開いたケース。
- 合格条件は固定する。
- 20 問中 18 問以上で `expected route` と一致する。
- abstain 対象は 100% で推測回答を出さない。
- 最終系回答は 100% で `根拠資料` か `needed_sources` を含む。
- `handoff_packet` は 20 問すべてで欠落項目なしにする。
- `pm_copy_template` は 10 件の代表ケースで、そのまま設計メモに転記できる粒度を満たす。
- Pilot は 5 人以内、各 5 問で始める。
- Pilot 中は `誤推測`, `誤ルーティング`, `競合見逃し`, `転記しにくさ` を主な改善論点として記録する。

## Assumptions
- v1 は `Copilot Only` を守り、外部オーケストレータ、独自ログ基盤、コードリポジトリ連携は入れない。
- v1 の knowledge は 1 系統の SharePoint / OneDrive に限定し、部門横断の複数 knowledge には広げない。
- v1 の保存先は chat と人の設計メモであり、自動記録や自動転記は後段に回す。
- 現行の Copilot Studio 実装前提として、knowledge は agent-level / topic-level に追加可能で、source 名と詳細な説明文が generative orchestration を助けること、SharePoint / OneDrive source は同期とユーザー権限検証を前提に扱えること、検索絞り込み時は `Web Search` と general knowledge を無効化することを前提にする。
  参考: [Add knowledge to an existing agent](https://learn.microsoft.com/en-us/microsoft-copilot-studio/knowledge-add-existing-copilot), [Add unstructured data as a knowledge source](https://learn.microsoft.com/en-us/microsoft-copilot-studio/knowledge-add-unstructured-data), [Add SharePoint as a knowledge source](https://learn.microsoft.com/en-us/microsoft-copilot-studio/knowledge-add-sharepoint)
- `Restricted SharePoint Search` が有効な tenant や guest user 利用は v1 の対象外とし、該当する場合は Pilot 前の環境確認で block とする。

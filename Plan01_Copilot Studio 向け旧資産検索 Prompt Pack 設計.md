# Copilot Studio 向け旧資産検索 Prompt Pack 設計

## Summary
- `Microsoft Copilot Studio` を前提に、`現行サービスの旧資産検索` を最優先にした `Multi Agents` 構成の Prompt Pack を設計する。
- 主利用者は `PM/設計者`。主なナレッジソースは `SharePoint / OneDrive`。回答は `日本語`、`根拠付き`、`根拠不足時は abstain` を徹底する。
- v1 の成功条件は、現行仕様に関する質問に対して、`関連資料を特定し、根拠付きで要約回答する` か、`不足資料を明示して止まる` こと。

## Key Changes
- Prompt Pack は 8 エージェントで設計する。
- `Intake Router`: 質問の意図を分類し、次に使うエージェントと検索スコープを決める。
- `Glossary Normalizer`: 現行業務の用語揺れ、旧画面名、機能別名を検索語へ正規化する。
- `Document Locator`: SharePoint / OneDrive から候補資料を絞り込む。
- `Evidence Verifier`: 候補資料が本当に質問の答えになっているか、承認状態・鮮度・競合有無を確認する。
- `Grounded Answerer`: 検証済み根拠だけを使って、日本語で簡潔に回答する。
- `Gap Reporter`: 根拠不足時に回答を止め、不足資料と追加確認事項を返す。
- `Renewal Impact Mapper`: 確認できた現行仕様から、刷新時に見直すべき観点を整理する。
- `Change Diff Scout`: 新要求と現行仕様の差分候補を `踏襲 / 変更 / 不明` で返す。
- 全エージェントで共通の I/O 契約を使う。
- 入力は `question`, `goal`, `entities`, `source_scope`, `known_constraints`。
- 検索系の出力は `candidate_sources[]`。各要素は `title`, `location`, `section/page`, `why_relevant`, `authority`, `freshness`, `conflict_flag` を持つ。
- 回答系の出力は `answer`, `evidence[]`, `confidence`, `unknowns[]`, `next_agent` を必須にする。
- すべての prompt は同じ骨格で揃える。
- `役割`, `対象範囲`, `対象外`, `許可ソース`, `根拠ルール`, `abstain 条件`, `出力形式`, `次エージェント案内` を固定セクションにする。
- Multi Agents でも hidden handoff には依存しない。
- 各エージェントは最後に `次に使うべきエージェント` を返し、実運用は明示ルーティングにする。
- あわせて `Routing Matrix` を作る。
- 例: `現行仕様を知りたい` は `Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Grounded Answerer`。
- 例: `刷新要求との差分を見たい` は `Intake Router -> Glossary Normalizer -> Document Locator -> Evidence Verifier -> Change Diff Scout`。

## Test Plan
- PM/設計者の実質問を使った評価セットを最低 20 問作る。
- `通常検索`, `用語揺れ`, `資料競合`, `根拠不足` を必須ケースに入れる。
- 合格条件は `正しい agent 選択`, `根拠付き回答`, `誤推測しないこと`, `競合資料を検知できること`, `日本語が簡潔であること`。
- Excel/PDF を含む資料、広すぎる質問、対象外質問も smoke test に入れる。

## Assumptions
- v1 では `SharePoint / OneDrive` を主ナレッジにし、コードリポジトリや外部検索は含めない。
- v1 の主目的は `現行仕様の理解支援` であり、設計書生成やコード生成は後段に回す。
- `Copilot Only` を守るため、外部オーケストレータや独自 MCP には依存しない。
- `Multi Agents` による運用負荷は受け入れ、代わりに各エージェントの責務を狭くし、プロンプトを短く固定する。
- 将来、差分分析や刷新設計の精度要求が Copilot 単体の上限を超えた場合は、別フェーズで外部実行基盤を追加検討する。

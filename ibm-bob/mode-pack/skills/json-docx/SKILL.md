---
name: ibm-bob-json-docx
version: 1.0.0
description: IBM-Bobの設計書JSONとWordテンプレートから、基本設計書・詳細設計書・機能テスト設計書などのDOCXを安定生成するためのSkill。LLMにDOCX内部構造を直接編集させず、JSON検証、テンプレート差し込み、DOCX生成、未置換チェック、レンダリング確認までを標準手順化する。
---

# IBM-Bob JSON → Word設計書生成 Skill

## 1. このSkillの目的

IBM-BobのAIエージェントが、以下の入力からWord設計書を安定して生成できるようにする。

- 設計書JSON
  - 基本設計書JSON
  - 詳細設計書JSON
  - 機能テスト設計書JSON
  - プレレビュー結果JSON
- DOCXテンプレート
  - `{{json.path}}` 形式の差し込み項目
  - `[[TABLE:json.path]]` 形式の表展開マーカー
- 出力DOCX
  - Wordで開ける `.docx`
  - 未置換プレースホルダなし
  - 章立て・表・箇条書きが崩れていない
  - レビューAIが項目抽出しやすい構造

重要: LLMはDOCXの中身を直接作り込まない。必ずこのSkillのスクリプトを使って、JSONからDOCXを生成する。

## 2. 適用タイミング

ユーザーが次のように依頼したら、このSkillを使う。

- 「JSONとテンプレートからWordファイルを生成して」
- 「基本設計書JSONから基本設計書DOCXを作って」
- 「詳細設計書テンプレートにJSONを差し込んで」
- 「機能テスト設計書をDOCX化して」
- 「プレレビュー結果をWordにして」
- 「IBM-Bobの設計書JSONをWord化して」

## 3. 入力ファイルの標準

### 3.1 JSON

JSONはUTF-8で保存する。トップレベルは必ずobjectとする。

推奨トップレベル項目:

```json
{
  "schema_version": "...",
  "document": {
    "document_id": "...",
    "document_name": "...",
    "document_type": "basic_design | detail_design | function_test_design",
    "version": "1.0",
    "created_date": "yyyy-mm-dd",
    "author": "IBM-Bob AI Agent",
    "status": "draft"
  }
}
```

JSON内の配列は、表または箇条書きに展開する。ネストが深い場合は、章単位で表現し、無理に1つの巨大表へ押し込まない。

### 3.2 DOCXテンプレート

テンプレートはWordで作成した `.docx` とし、次の書式を使う。

#### 単一値プレースホルダ

```text
{{document.document_name}}
{{document.version}}
{{target_system.system_name}}
```

#### JSON Path風プレースホルダ

```text
{{$.document.document_name}}
{{json.document.document_name}}
```

上記3形式は同じ意味として扱う。

#### 配列またはオブジェクトを本文へ展開

```text
{{design_policy.scope}}
{{impacted_assets}}
```

- scalar: そのまま文字列化
- list[str]: 箇条書き相当の改行テキスト
- list[object]: `key: value` 形式の複数行
- object: `key: value` 形式の複数行

#### 表の繰り返し展開

テンプレート内の表で、1行目または任意セルに次のマーカーを置く。

```text
[[TABLE:traceability.requirements]]
```

マーカーがある表では、マーカー行の次の行をテンプレート行として複製する。テンプレート行には以下のように `{{item.xxx}}` を置く。

```text
{{item.id}} | {{item.summary}} | {{item.design_ref}}
```

展開後、マーカー行とテンプレート行は削除し、配列件数分の行を追加する。

## 4. 標準ワークフロー

### 4.1 Golden Path

1. 入力JSONを読み込む。
2. JSON構文を検証する。
3. JSONトップレベルと `document` メタ情報の有無を確認する。
4. テンプレートDOCXの有無を確認する。
5. `scripts/generate_docx_from_json.py` でDOCXを生成する。
6. `scripts/validate_docx_output.py` で以下を検査する。
   - DOCXがzipとして開ける
   - `word/document.xml` が存在する
   - XMLが壊れていない
   - 未置換 `{{...}}` が残っていない
   - 未展開 `[[TABLE:...]]` が残っていない
   - 空文書ではない
7. 可能であればWord/LibreOfficeで開く、またはPDF/PNGレンダリング確認を行う。
8. 問題があればJSONまたはテンプレートを修正し、再生成する。

### 4.2 コマンド例

```bash
python scripts/generate_docx_from_json.py \
  --json input/design.json \
  --template input/template.docx \
  --out output/design.docx \
  --doc-type auto \
  --strict

python scripts/validate_docx_output.py \
  --docx output/design.docx \
  --fail-on-unresolved
```

テンプレートが使えない場合は、フォールバック生成を使う。

```bash
python scripts/generate_docx_from_json.py \
  --json input/design.json \
  --out output/design.docx \
  --doc-type auto
```

## 5. LLM向け禁止事項

以下は禁止する。

- `.docx` をテキストファイルとして直接作る
- `.txt` や `.html` を拡張子だけ `.docx` に変更する
- OOXMLを手書きで最小構成だけ作り、Wordで開ける保証をしない
- 未置換の `{{...}}` や `[[TABLE:...]]` を残したまま完了扱いにする
- テンプレート差し込みに失敗したのに「生成済み」と報告する
- RT/TS、共有メモリ、API、状態遷移、テスト観点などの重要章を勝手に削除する
- 文字化け確認なしにShift-JIS/CP932へ変換する

## 6. 失敗時の復旧方針

### 6.1 テンプレート差し込みに失敗した場合

1. JSONはそのまま保持する。
2. テンプレート差し込みをあきらめる前に、未解決プレースホルダ一覧を出す。
3. それでも失敗する場合、フォールバック生成で章立て付きDOCXを作る。
4. ユーザーには「テンプレート保持版ではなく、構造生成版で作成した」と明記する。

### 6.2 表展開に失敗した場合

1. `[[TABLE:path]]` のpathがJSON内に存在するか確認する。
2. pathの値が配列であるか確認する。
3. テンプレート行の `{{item.xxx}}` が配列要素のキーと一致するか確認する。
4. 失敗時は、表ではなく箇条書き・キー値表として出力する。

### 6.3 DOCX検証に失敗した場合

1. 出力DOCXを破棄する。
2. フォールバック生成に切り替える。
3. 再度 `validate_docx_output.py` を実行する。
4. 検証済みファイルだけを成果物として提示する。

## 7. 出力時に報告すること

最終回答では以下を簡潔に報告する。

- 生成したDOCXのリンク
- 使用したJSONのリンク
- テンプレート使用有無
- 検証結果
  - JSON構文OK
  - DOCX構造OK
  - 未置換プレースホルダなし
  - 表展開OK、またはフォールバック展開あり
- レンダリング確認ができなかった場合は、その事実

## 8. 推奨Skill実行コマンド

IBM-Bobのエージェントは、Word生成時に原則として以下を実行する。

```bash
python .ibm-bob/skills/json-docx/scripts/generate_docx_from_json.py --json <input.json> --template <template.docx> --out <output.docx> --strict
python .ibm-bob/skills/json-docx/scripts/validate_docx_output.py --docx <output.docx> --fail-on-unresolved
```

## 9. 品質ゲート

完了扱いにしてよい条件:

- JSONが読める
- DOCXがWord形式zipとして正常
- `word/document.xml` がXMLとして正常
- 文書本文に文字が存在する
- 未置換 `{{...}}` が存在しない
- 未展開 `[[TABLE:...]]` が存在しない
- 入力JSONの `document.document_name` 相当のタイトルが文書内に存在する

上記のどれかを満たさない場合は、成果物として出してはいけない。

<!-- docs/sdlc/samples/email-notification-test-spec-dry-run.md -->
<!-- Shows a minimal test spec lane dry run for the email notification setting example. -->
<!-- This exists so the team can verify how reviewed design artifacts become functional and integration specs. -->
<!-- RELEVANT FILES: .copilot/routing/test-spec-matrix.yaml, docs/sdlc/templates/functional-spec.md, docs/sdlc/templates/integration-spec.md -->
# Email Notification Test Spec Dry Run

この sample は、`detail_design` 通過後に `functional_spec` と `integration_spec` を作る最小構成です。

## functional_spec work_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "artifact_manifest": {
    "artifact_id": "FUNC-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "functional_spec",
    "status": "draft",
    "prompt_id": "P2_functional_spec_author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["DETAIL-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "stage": "functional_spec",
  "objective": "通知設定変更の機能試験仕様を作る",
  "next_agent": "P3_spec_reviewer"
}
```

## functional_spec excerpt
```markdown
# Functional Spec

## 対象シナリオ
- 認証済みユーザーが通知設定を ON から OFF に変更する
- 未認証ユーザーが更新を試みる

## 事前条件
- ユーザーが設定画面にアクセスできる
- 既存設定値が取得できる

## 操作と期待結果
- トグルを変更して保存すると成功メッセージが表示される
- 再表示時に最新設定が反映される

## 異常系
- 未認証時は 401 相当の結果を返す

## 受入条件トレーサビリティ
- AC-01 -> 正常更新シナリオ
- AC-02 -> 再表示確認シナリオ
```

## functional review_findings
```json
{
  "artifact_type": "functional_spec",
  "review_decision": "pass",
  "summary": "主要シナリオと受入条件の対応が明確",
  "findings": [],
  "revise_to": "P2_functional_spec_author",
  "human_checkpoint": false,
  "next_agent": "P8_review_record"
}
```

## integration_spec work_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "artifact_manifest": {
    "artifact_id": "INTSPEC-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "integration_spec",
    "status": "draft",
    "prompt_id": "P2_integration_spec_author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["DETAIL-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "stage": "integration_spec",
  "objective": "通知設定変更の結合試験仕様を作る",
  "next_agent": "P3_spec_reviewer"
}
```

## integration_spec excerpt
```markdown
# Integration Spec

## 対象境界
- PATCH /notification-settings
- user_settings テーブル

## 依存関係
- 認証ミドルウェア
- SQLite 接続

## データ整合
- 保存後に email_notifications_enabled が更新される

## 異常系と復旧
- 不正入力時は DB 更新しない
- 保存失敗時は retry 方針を確認する

## トレーサビリティ
- detail_design.API 契約 -> 対象境界
- detail_design.永続化 -> データ整合
```

## integration review_findings
```json
{
  "artifact_type": "integration_spec",
  "review_decision": "pass",
  "summary": "API と DB の境界観点がそろっている",
  "findings": [],
  "revise_to": "P2_integration_spec_author",
  "human_checkpoint": false,
  "next_agent": "P8_review_record"
}
```

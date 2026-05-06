# IBM-Bob Source Implementation Report v42_10

## 概要

`v41_10` の続きとして、ID `335〜344` の10機能を追加した。実装は `nc_feature_backlog_v42` として分離し、既存の backlog continuation 方式と同じく、固定長ステータス、固定長ループ、RT側ファイルI/Oなし、動的メモリ確保なしの制約を守る。

## 追加した10機能

| ID | 機能 | 方針 |
|---:|---|---|
| 335 | 主軸エアパージ圧低下ゲート | GATE |
| 336 | 軸リミットスイッチ貼り付きトレース | TRACE |
| 337 | オイルミスト供給低下HOLD | HOLD |
| 338 | ローダチャック開閉確認インターロック | INTERLOCK |
| 339 | 自動扉開閉タイムアウトリトライ | RETRY |
| 340 | テーブル着座センサ不一致HOLD | HOLD |
| 341 | 主軸回転中ドア解除禁止 | INHIBIT |
| 342 | 工具ID RFID読取失敗ゲート | GATE |
| 343 | 加工室照明異常トレース | TRACE |
| 344 | サーボ温度上昇段階減速 | SLOWDOWN |


## 追加ファイル

- `source/inc/nc_feature_backlog_v42.h`
- `source/src/nc_feature_backlog_v42.c`
- `source/tests/test_nc_feature_backlog_v42.c`
- `source/scripts/build_gcc_smoke_v42_10.sh`
- `source/scripts/test_gcc_smoke_v42_10.sh`
- `source/scripts/build_vc2026_v42_10.bat`
- `source/scripts/test_vc2026_v42_10.bat`
- `source/scripts/build_clang_v42_10.bat`
- `source/scripts/test_clang_v42_10.bat`

## 実装方針

- TS側 `NcFeatureBacklogV42_ApplyBlockTs()` で、実行ブロックに対する gate / hold / interlock / retry / inhibit / trace / slowdown の固定長マスク更新を行う。
- RT側 `NcFeatureBacklogV42_RunRtTick()` で、主軸エアパージ圧、軸リミットスイッチ、オイルミスト圧、ローダチャック、自動扉、テーブル着座、主軸回転中ドア解除、RFID、照明、サーボ温度を監視する。
- チャタリング・一時的欠落を吸収するため、各機能に固定tickの待ちカウンタを持たせる。
- 共有状態は `NC_FEATURE_BACKLOG_V42_STATUS` に集約し、APIからスナップショット取得できる。

## 検証

GCC smoke build/test を実行済み。

```text
implementation_backlog_v42 implemented=10 configured=10 ts=1 rt=8 hold=30 interlock=8 gate=15 slowdown=15 trace=16 retry=11 inhibit=17 cat_mask=0x00001000 last_id=344 last_policy=4 trace=0x4209 fault=0x000003ff axis=0x00000002 pressure=60 spindle_rpm=2400 temp=86 table_mismatch=0x00000002 tool_rfid=0 last_x=4200
PASS nc_feature_backlog_v42
```

## Windows確認

```bat
cd source
scripts\test_vc2026_v42_10.bat
scripts\test_clang_v42_10.bat
```

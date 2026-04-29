# IBM-Bob 追加機能実装レポート v7

## 目的
v6で追加した5軸/傾斜、ミラー、軸退避、経路制御の実行基盤に対し、運用時に必要となる診断・保全系の追加機能を実装した。

## 追加した主な機能

### 1. 軸過負荷予兆監視・段階減速要求
- 新規共有メモリ `NC_AXIS_LOAD_STATUS` / `g_ncAxisLoadStatus` を追加。
- RT周期で `g_ioIn.analog[1..AXIS_MAX]` を軸負荷入力として監視。
- 移動平均フィルタにより瞬間ノイズを抑制し、以下の段階を判定する。
  - Warning: `NC_AXIS_LOAD_WARN_THRESHOLD`
  - Limit: `NC_AXIS_LOAD_LIMIT_THRESHOLD`
  - Critical: `NC_AXIS_LOAD_CRITICAL_THRESHOLD`
- 判定結果として `warning_mask`、`limit_mask`、`critical_mask`、`staged_decel_level`、`decel_override_percent` を公開。
- RT側ではファイルI/O、動的メモリ、重い計算を行わず、固定長共有メモリ更新のみとした。

### 2. 工具寿命カウンタ・交換予告/寿命到達通知
- 新規共有メモリ `NC_TOOL_LIFE_STATUS` / `g_ncToolLifeStatus` を追加。
- `Api_SetNcToolLifeLimit()` で工具ごとの寿命上限を設定可能にした。
- `M06 Tn` およびアクティブ工具によるモーションブロック消費時に使用回数を更新。
- 80%到達で `warning_mask`、寿命到達で `expired_mask` を設定。
- サンプルでは `T2` の寿命上限を4に設定し、スモークテスト内で警告/寿命到達を確認する。

### 3. NC診断スナップショット
- 新規共有メモリ `NC_DIAGNOSTIC_SNAPSHOT` / `g_ncDiagnosticSnapshot` を追加。
- `Api_GetNcDiagnosticSnapshot()` を追加。
- RT周期の最後に、以下を一括スナップショット化する。
  - 実行状態、NC状態、アラーム、実行行、読込行、バッファ数
  - 運転開始前インターロックマスク
  - 軸負荷警告/制限/クリティカルマスク
  - 段階減速レベルと要求オーバライド
  - 工具寿命警告/寿命到達マスク
  - 補間完了数、固定サイクル完了数、MFIN待ち時間、I/Oトレースfreeze状態

### 4. API追加
- `Api_SetNcToolLifeLimit()`
- `Api_GetNcAxisLoadStatus()`
- `Api_GetNcToolLifeStatus()`
- `Api_GetNcDiagnosticSnapshot()`

### 5. ビルド/テストスクリプト更新
- `scripts/build_vc2026.bat` と `scripts/build_clang.bat` に、v4〜v7で追加した以下のソースを追加。
  - `nc_compensation.c`
  - `nc_kinematics.c`
  - `nc_lookahead.c`
  - `nc_diagnostics.c`
- `scripts/test_vc2026.bat` と `scripts/test_clang.bat` の期待出力をv7のスモークテストに更新。

## 主要な確認結果

`docs/verification_output_v7.txt` に実行結果を保存した。

主な確認値:

```text
nc_state=6 nc_error=0
path_arc=2 path_helical=1 path_arc_segments=8
kin_transformed=16 kin_mirrored=11 kin_retracts=1
diag_axis_warn=0x00000003 diag_axis_limit=0x00000001 diag_axis_critical=0x00000000 decel_level=2 decel_override=45
tool_active=2 tool2_uses=7 tool_warn=0x00000004 tool_expired=0x00000004
coord_error_checks=5
```

## 実装上の制約

- 軸負荷値は評価用として `g_ioIn.analog[1..]` を使用している。実機ではサーボアンプ/モーションコントローラからの負荷率入力へ接続する必要がある。
- 段階減速は現時点では「要求状態の公開」までであり、実際の加減速計画再生成や補間中速度制御への閉ループ反映は未実装。
- 工具寿命カウントはサンプル実装として、工具選択/交換とモーションブロック消費を基準にしている。実機では加工時間、切削距離、材質、主軸負荷などとの統合が必要。

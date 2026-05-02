# C言語 単体テスト作成結果

## 1. 対象関数分析

| 関数名 | 役割 | 入力 | 出力 | 依存するグローバル変数 | 外部依存 |
| --- | --- | --- | --- | --- | --- |
| `Api_RequestRunModeChange` | 運転モード変更要求をAPI経由で登録 | `requestedMode` | 戻り値、`g_uiRequest` | `g_uiRequest` | なし |
| `Api_GetStatusSnapshot` | 共有状態のスナップショット取得 | `STATUS_SNAPSHOT*` | 戻り値、出力構造体 | `g_machineCtx`, `g_ioIn`, `g_ioOut`, `g_uiRequest`, `g_prefetchReq` | なし |
| `Api_RequestNcProgramLoad` | NCプログラムロード要求登録 | `filePath` | 戻り値、`g_ncProgramRequest` | `g_machineCtx`, `g_ncProgramStatus`, `g_ncProgramRequest` | なし |
| `Api_RequestNcProgramStart/Hold/Stop/Resume` | NC運転状態遷移要求登録 | なし | 戻り値、要求フラグ | `g_ncProgramStatus`, `g_ncProgramRequest` | なし |
| `Api_Get*Status` 系 | 各状態構造体のコピー取得 | 出力ポインタ | 戻り値、出力構造体 | 各 `g_*Status` | なし |
| `NcProgram_ClearBuffer` | NC実行リングバッファ初期化 | なし | バッファ状態更新 | `g_ncProgramBuffer`, `g_ncProgramStatus` | なし |
| `NcBuffer_Count` | リングバッファ件数算出 | なし | 件数 | `g_ncProgramBuffer` | なし |
| `NcBuffer_IsFull` | リングバッファ満杯判定 | なし | 満杯フラグ | `g_ncProgramBuffer` | なし |
| `NcBuffer_CommitBlock` | 実行ブロック投入 | `NC_EXEC_BLOCK*` | 戻り値、バッファ更新 | `g_ncProgramBuffer`, `g_ncProgramStatus` | なし |
| `NcBuffer_PeekHead` | 先頭ブロック参照 | `NC_EXEC_BLOCK*` | 戻り値、出力ブロック | `g_ncProgramBuffer` | なし |
| `NcBuffer_ConsumeHead` | 先頭ブロック消費 | なし | バッファ更新 | `g_ncProgramBuffer`, `g_ncProgramStatus` | なし |
| `NcDiagnostics_UpdateAxisLoadRt` | 軸負荷監視、段階減速要求、ログ登録 | 共有メモリ入力 | 負荷状態、警告/制限/重大マスク | `g_ioIn`, `g_ncAxisLoadStatus`, `g_machineCtx`, `g_logQueue` | センサ入力は `stub_external_io` で模擬 |
| `NcDiagnostics_SetToolLifeLimit` | 工具寿命上限設定 | `toolNo`, `limit` | 戻り値、寿命上限 | `g_ncToolLifeStatus` | なし |
| `NcDiagnostics_OnBlockRt` | 実行ブロックに応じた工具使用回数更新 | `NC_EXEC_BLOCK*` | 工具寿命状態、警告/寿命到達マスク | `g_ncToolLifeStatus`, `g_logQueue` | NC実行ブロックはスタブ生成 |
| `NcDiagnostics_BuildSnapshot` | 診断スナップショット構築 | 共有状態 | `g_ncDiagnosticSnapshot` | 多数の `g_*Status` | なし |
| `NcMotionFilter_ResetRt` | モーションフィルタRT状態初期化 | なし | フィルタ状態 | `g_ncMotionFilterStatus` | なし |
| `NcMotionFilter_SetConfigRt` | フィルタ有効/方式/窓幅設定 | `enabled`, `secondStageMode`, `velocityWindow`, `accelWindow` | 戻り値、設定値 | `g_ncMotionFilterStatus` | なし |
| `NcMotionFilter_SetAxisLimitRt` | 軸別速度/加速度上限設定 | `axis`, `maxVelocityPerTick`, `maxAccelPerTick` | 戻り値、上限値 | `g_ncMotionFilterStatus` | なし |
| `NcMotionFilter_ApplyRt` | 速度・加速度平滑化、上限、終点補正 | `axisTarget`, `axisMask`, `forceEndpoint`, `remainingTicks` | 軸目標、状態、イベント | `g_ncMotionFilterStatus`, `g_ncEventRing`, `g_ncProgramStatus` | モーションコントローラ出力先は共有メモリ配列で模擬 |

---

## 2. 単体テスト観点一覧

| No | 観点 | 内容 | 優先度 |
| -- | -- | -- | --- |
| UT-001 | NULLポインタ | API取得系、バッファ参照、モーションフィルタ入力NULL | 高 |
| UT-002 | モード範囲外 | `RUN_MODE_MAINT` 超過時の `RESPONSE_PARAM_ERROR` | 高 |
| UT-003 | 状態遷移不正 | NC未READYでStart、未RUNNINGでHold、未HOLDでResume | 高 |
| UT-004 | 状態遷移正常 | READY→Start要求、RUNNING→Hold要求、HOLD→Resume要求 | 高 |
| UT-005 | NCロード境界 | NULL、空文字、長過ぎるパス、AUTO以外、BUSY状態、正常パス | 高 |
| UT-006 | リングバッファ境界 | 0件、1件、満杯、満杯時投入拒否、消費後ラップアラウンド | 高 |
| UT-007 | 軸負荷異常 | センサ値の絶対値化、警告/制限/重大しきい値、段階減速、ログ | 高 |
| UT-008 | 工具寿命境界 | 工具番号0、上限0、警告80%、寿命到達 | 高 |
| UT-009 | モーションフィルタ設定 | 不正方式、窓幅0、最大窓幅超過、軸番号範囲外 | 高 |
| UT-010 | フェイルセーフ出力 | 速度上限、加速度上限、終点強制一致、イベント記録 | 高 |
| UT-011 | 副作用確認 | `g_*Status`、ログキュー、イベントリング、次周期残留状態 | 高 |
| UT-012 | 未初期化相当 | 各テスト前に `SystemShared_Initialize` + フィルタリセット | 高 |
| UT-013 | 分岐網羅 | if/else、early return、エラー分岐、状態遷移分岐 | 中 |

---

## 3. テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
| ----- | ---- | ----- | ---- | ---- | -- |
| TC-API-001 | `Api_RequestRunModeChange` | 正常モード変更要求 | `RUN_MODE_MAINT` | 戻り値0、要求フラグON、ACCEPTED | 正常系 |
| TC-API-002 | `Api_RequestRunModeChange` | 範囲外モード拒否 | `RUN_MODE_MAINT+1` | 戻り値-1、PARAM_ERROR | 異常系/境界 |
| TC-API-003 | `Api_GetStatusSnapshot` | NULL拒否 | NULL | 戻り値-1 | 異常系 |
| TC-API-004 | `Api_GetStatusSnapshot` | スナップショットコピー | 状態・I/O・prefetch設定済み | 出力構造体に一致 | 正常系/副作用 |
| TC-API-005 | `Api_GetPrefetchStatus` | NULL組合せ拒否 | 片方NULL | 戻り値-1 | 異常系 |
| TC-API-006 | `Api_RequestNcProgramLoad` | NULL/空/長過ぎ/Manual/BUSY拒否 | 各入力 | 戻り値-1、REJECTEDまたはPARAM_ERROR | 異常系/境界 |
| TC-API-007 | `Api_RequestNcProgramLoad` | AUTO + IDLEでロード受理 | `sample.nc` | load_request ON、seq加算、パスコピー | 正常系 |
| TC-API-008 | `Api_RequestNcProgramStart/Hold/Resume/Stop` | 正常・不正状態遷移要求 | READY/RUNNING/HOLD/それ以外 | 要求フラグと戻り値を検証 | 状態遷移 |
| TC-API-009 | `Api_Get*Status` | 取得系NULL拒否 | NULL | 戻り値-1 | 異常系 |
| TC-BUF-001 | `NcProgram_ClearBuffer` | バッファ初期化 | 汚染済み共有状態 | head/tail/count=0 | 正常系/副作用 |
| TC-BUF-002 | `NcBuffer_CommitBlock/Peek/Consume` | 1件投入・参照・消費 | 有効ブロック | generation、valid、count更新 | 正常系 |
| TC-BUF-003 | `NcBuffer_CommitBlock/PeekHead` | NULL/空バッファ拒否 | NULL、空 | 戻り値-1 | 異常系 |
| TC-BUF-004 | `NcBuffer_IsFull` | 配列境界・満杯判定 | `NC_MAX_LOOKAHEAD_LINES-1` 件投入 | full=1、追加拒否 | 境界値 |
| TC-BUF-005 | `NcBuffer_ConsumeHead` | 空消費No-op | 空バッファ | count/generation不変 | 境界値 |
| TC-DIAG-001 | `NcDiagnostics_UpdateAxisLoadRt` | 正常負荷 | 低いanalog値 | 警告なし、override=100% | 正常系 |
| TC-DIAG-002 | `NcDiagnostics_UpdateAxisLoadRt` | 負値センサの絶対値化と重大異常 | analog=-4000 | warning/limit/critical、減速25%、ログ登録 | 異常系/フェイルセーフ |
| TC-DIAG-003 | `NcDiagnostics_UpdateAxisLoadRt` | イベント重複抑止 | 同一異常を連続入力 | event countは初回のみ | 副作用 |
| TC-DIAG-004 | `NcDiagnostics_SetToolLifeLimit` | 境界・不正値 | tool=0, 範囲外, limit=0 | 戻り値-1 | 異常系/境界 |
| TC-DIAG-005 | `NcDiagnostics_OnBlockRt` | M06工具使用と寿命警告/到達 | tool2, limit=3 | 2回目警告、3回目expired | 境界値 |
| TC-DIAG-006 | `NcDiagnostics_OnBlockRt` | active toolの切削ブロック加算 | M06後、LINEAR/DWELL | LINEAR加算、DWELL非加算 | 分岐 |
| TC-DIAG-007 | `NcDiagnostics_BuildSnapshot` | 診断スナップショットコピー | 各共有状態設定 | snapshotに一致 | 正常系/副作用 |
| TC-MF-001 | `NcMotionFilter_ResetRt` | デフォルト設定復元 | 初期化 | enabled/FIR/window/axis limit一致 | 正常系 |
| TC-MF-002 | `NcMotionFilter_SetConfigRt` | 不正方式拒否、窓幅クランプ | mode=99, window=0/超過 | 戻り値、設定値検証 | 異常系/境界 |
| TC-MF-003 | `NcMotionFilter_SetAxisLimitRt` | 軸範囲外、負値、0、最大超過 | axis=AXIS_MAX等 | 拒否/正規化/クランプ | 境界値 |
| TC-MF-004 | `NcMotionFilter_ApplyRt` | NULL/disabled No-op | NULL、有効OFF | 状態/target不変 | 異常系 |
| TC-MF-005 | `NcMotionFilter_ApplyRt` | 初回サンプル初期化 | target=0 | target不変、applied_samples加算 | 正常系 |
| TC-MF-006 | `NcMotionFilter_ApplyRt` | 軸速度上限とラグ | X=1000, 上限80 | 出力80、lag=920、イベント | フェイルセーフ/分岐 |
| TC-MF-007 | `NcMotionFilter_ApplyRt` | 終点強制一致 | forceEndpoint=1 | target=raw、endpoint_corrections加算 | 境界/安全 |
| TC-MF-008 | `NcMotionFilter_ApplyRt` | axisMask対象外 | mask=0 | filtered_axis_mask不変 | 分岐 |

---

## 4. テストコード

配置済みファイル:

```text
source/tests/test_common.h
source/tests/test_control_api.c
source/tests/test_nc_buffer.c
source/tests/test_nc_diagnostics.c
source/tests/test_nc_motion_filter.c
```

各テストは `main()` を持つ独立実行形式です。`RUN_TEST()` と `TEST_ASSERT_*()` により失敗箇所を標準エラーへ出力します。

---

## 5. スタブ・モックコード

配置済みファイル:

```text
source/stubs/stub_external_io.h
source/stubs/stub_external_io.c
source/stubs/stub_module_deps.c
```

- `stub_external_io.*`: PLC、センサ、モーションコントローラ相当の共有メモリ入力をテスト側から設定するヘルパです。
- `stub_module_deps.c`: `control_api.c` 単体テストで対象外モジュールへのリンク依存を切り離すための最小スタブです。

---

## 6. ビルド方法

Windows / Visual Studio / `cl.exe` 想定:

```bat
cd source
scripts\build_tests.bat
```

代表的なビルドコマンドは以下です。

```bat
cl /nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS ^
  /Iinc /Istubs /Itests ^
  tests\test_nc_motion_filter.c ^
  src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c ^
  stubs\stub_external_io.c ^
  /Fe:build\tests\test_nc_motion_filter.exe
```

---

## 7. 実行方法

```bat
cd source
scripts\run_tests.bat
```

実行結果は次に出力されます。

```text
build/test_results.txt
```

---

## 8. 期待結果

成功時の例:

```text
[run_tests] PASS. Results: build\test_results.txt
===== test_control_api =====
[  PASSED  ] test_control_api
===== test_nc_buffer =====
[  PASSED  ] test_nc_buffer
===== test_nc_diagnostics =====
[  PASSED  ] test_nc_diagnostics
===== test_nc_motion_filter =====
[  PASSED  ] test_nc_motion_filter
```

失敗時の例:

```text
ASSERT FAILED: tests/test_nc_motion_filter.c:123: expected 80 actual 1000: target[0]
[run_tests] FAIL. See build\test_results.txt.
```

この環境では、GCCで同等ビルド・実行確認を行い、全テストPASSの `build/test_results.txt` を同梱しています。

---

## 9. カバレッジ上の注意点

- 今回は `assert` ベースの単体テストコード作成が主目的であり、`gcov` / Visual Studio Coverage による行・分岐カバレッジ計測までは同梱していません。
- `control_api.c` は公開APIが多数あるため、今回の対象は中核API、取得系NULL、NC運転要求、NCロード要求です。補正、主軸、同期、工具管理などのAPI委譲先はスタブ化しています。
- NCパーサ、TS側ファイルI/O、実NCファイル読込、RT周期全体の `RtNcProgram_ProcessRequests` / `RtNcProgram_ConsumeBlocks` は結合テストまたは機能テスト側で別途確認が必要です。
- モーションフィルタは代表的な速度上限、終点補正、mask分岐を確認していますが、FIR係数全履歴、負方向速度、複数軸同時飽和、加速度上限のみ発生する条件は追加テスト候補です。
- 軸負荷診断は単一軸中心です。複数軸同時異常、警告→解除→再警告、analogチャネル範囲外相当は追加テスト候補です。
- 工具寿命は `M06` と通常モーション加算を確認しています。工具番号31、bit31、寿命上限1、寿命到達後のNC HOLD遷移は追加テスト候補です。

---

## 10. 仕様不明点

- NCロード要求において、`NC_READY` 状態を「再ロード不可」とする現在実装は妥当か。別ファイルの再選択を許可する仕様なら期待値が変わります。
- `Api_RequestNcProgramStop` は現在どの状態でも受理されます。STOPPED/IDLE中のStop要求を受理する仕様でよいか確認が必要です。
- 軸負荷フィルタのしきい値判定は `filtered_load >= threshold` です。しきい値一致時を異常扱いする仕様でよいか確認が必要です。
- 工具寿命カウントは `M06` と切削モーションの両方で増加します。工具交換回数、切削ブロック数、切削時間、切削距離のどれを寿命単位にするかは設備仕様確認が必要です。
- モーションフィルタの初回サンプルでは `applied_samples` は加算される一方、対象軸の `filtered_axis_mask` はまだ立ちません。これを「初期化のみ」と扱う仕様でよいか確認が必要です。
- 終点強制一致時に速度/加速度上限を超えてでもraw targetへ一致させます。実機サーボ制限優先か、終点一致優先かの安全仕様確認が必要です。

---

## 11. 単体テストと結合・機能テストの切り分け

| 区分 | 今回の単体テストで確認 | 結合・機能テストで確認すべき事項 |
| --- | --- | --- |
| API | 要求フラグ、戻り値、NULL、状態条件 | API→RT/TS処理完了までの非同期シーケンス |
| 共有メモリ | 構造体更新、初期化、副作用残りなし | 複数スレッドからの世代番号・読み書き整合性 |
| RT診断 | 軸負荷判定、工具寿命判定、ログ副作用 | 実センサ値、PLC入力、アラーム/HOLD遷移 |
| モーション出力 | 平滑化、上限、終点補正 | 実モーションコントローラ応答、サーボ追従、周期時間 |
| NCバッファ | リング境界、投入/参照/消費 | TS先読みとRT消費の同時動作、アンダーラン |

---

## 12. 最小限の変更案

既存 `src/` と `include/` は変更していません。今後、カバレッジを上げる場合の最小変更案は以下です。

- `static` 関数を直接テストしない方針は維持し、公開関数経由で分岐を到達させる。
- RT周期処理に対しては、周期入力を1tickずつ注入する `tests/test_rt_cycle.c` を追加する。
- `scripts/build_tests.bat` に `/analyze` または `/fsanitize=address` 相当のclang-cl版を追加する。
- Windows側でカバレッジを取る場合は Visual Studio Enterprise のコードカバレッジ、または clang/LLVM coverage を別スクリプトに分離する。

---

## 13. 追加作成した単体テスト

今回の追加依頼に対して、前回対象外だった以下の関数群を追加で単体テスト化しました。

### 13.1 追加対象関数分析

| 関数名 | 役割 | 入力 | 出力 | 依存するグローバル変数 | 外部依存 |
| --- | --- | --- | --- | --- | --- |
| `NcCodes_GetGCodeInfo` | Gコード定義テーブル参照 | `code`, `NC_G_CODE_INFO*` | 戻り値、Gコード情報 | なし | なし |
| `NcCodes_GetAddressInfo` | アドレス文字定義テーブル参照 | `address`, `NC_ADDRESS_INFO*` | 戻り値、アドレス情報 | なし | なし |
| `NcCodes_GetMCodeInfo` | Mコード定義テーブル参照 | `code`, `NC_M_CODE_INFO*` | 戻り値、Mコード情報 | なし | なし |
| `NcFeed_Reset` | 送り制御状態初期化 | なし | `g_ncFeedStatus` | `g_ncFeedStatus` | なし |
| `NcFeed_SetOverride` | 送りオーバライド設定 | `percent` | 戻り値、override状態 | `g_ncFeedStatus` | なし |
| `NcFeed_PrepareMotionTs` | TS側で補間tickと加減速プロファイルを設定 | `NC_EXEC_BLOCK*`, 経路長, error出力 | 戻り値、ブロック更新 | `g_ncFeedStatus` | なし |
| `NcFeed_PrepareDwellTs` | ドウェルtick設定 | `NC_EXEC_BLOCK*`, dwellTicks, error出力 | 戻り値、ブロック更新 | `g_ncFeedStatus` | なし |
| `NcFeed_BeginBlockRt` / `NcFeed_UpdateRt` | RT側送り状態開始・完了更新 | `NC_EXEC_BLOCK*`, tick, done | 戻り値、送り状態 | `g_ncFeedStatus` | なし |
| `NcFeed_ProfileRatioRt` | 台形加減速比率算出 | `NC_EXEC_BLOCK*`, tick | 比率 | なし | なし |
| `NcCoordinate_Reset` | 座標系状態初期化 | なし | `g_ncCoordinateState` | `g_ncCoordinateState` | なし |
| `NcCoordinate_SetWorkOffsetAxis` | ワーク座標オフセット設定 | work, axis, offset | 戻り値、座標状態 | `g_ncCoordinateState` | なし |
| `NcCoordinate_SetLocalShiftAxis` | ローカルシフト設定 | axis, shift | 戻り値、座標状態 | `g_ncCoordinateState` | なし |
| `NcCoordinate_SetProgramAxis` | プログラム座標指令を機械座標へ反映 | axis, programPosition, block | 戻り値、ブロック更新 | `g_ncCoordinateState` | なし |
| `NcCoordinate_SetMachineAxis` | 機械座標指令をプログラム座標へ逆算 | axis, machinePosition, block | 戻り値、ブロック更新 | `g_ncCoordinateState` | なし |
| `NcCoordinate_SetTemporaryAxis` | 一時絶対座標シフト設定 | axis, programPosition | 戻り値、座標状態 | `g_ncCoordinateState` | なし |
| `NcCoordinate_FillTargets` | 現在機械座標をブロック目標へコピー | `NC_EXEC_BLOCK*` | ブロック更新 | `g_ncCoordinateState` | なし |
| `NcCoordinateTransform_*` | 動的治具・取付誤差・ロータリ補正設定とブロック適用 | axis/offset/block/error | 戻り値、補正後ブロック | `g_ncCoordinateTransformStatus`, `g_ncCoordinateState` | なし |
| `NcAxisConfig_*` | 軸定義、経路軸、取り外し軸、直径モード設定とブロック適用 | axis/mask/block/error | 戻り値、軸状態、イベント | `g_ncAxisConfigStatus`, `g_ncEventRing` | なし |
| `NcRotaryMcc_*` | ロータリMCC半径設定、仮想周速、MCC出力制御 | axis/radius/block | 戻り値、状態、出力bit | `g_ncRotaryMccStatus`, `g_ioOut`, `g_machineCtx`, `g_ncProgramStatus` | モーション出力bitは共有メモリで模擬 |

### 13.2 追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
| ----- | ---- | ----- | ---- | ---- | -- |
| TC-CODE-001 | `NcCodes_GetGCodeInfo` | 通常Gコード、10倍表現Gコード、未知Gコード、NULL | G01, G76.7, G999, NULL | 正常コピーまたは-1 | 正常/異常/境界 |
| TC-CODE-002 | `NcCodes_GetAddressInfo` | 小文字正規化、軸/増分軸、未知文字、NULL | `x`, `w`, `#`, NULL | 大文字化、kind/axis一致、未知は-1 | 正常/異常 |
| TC-CODE-003 | `NcCodes_GetMCodeInfo` | M06、未登録999以下、1000超過、NULL | 6, 777, 1000, NULL | MFIN待ち、default対応、範囲外扱い | 正常/境界 |
| TC-FEED-001 | `NcFeed_Reset/SetOverride` | 初期化、不正上限、正常設定 | 201%, 150% | 拒否または状態更新 | 正常/境界 |
| TC-FEED-002 | `NcFeed_PrepareMotionTs` | NULL、feed=0、負経路長、G95で主軸0 | 各異常入力 | `NC_ERR_FEED` または-1 | 異常 |
| TC-FEED-003 | `NcFeed_PrepareMotionTs` | 通常送りとoverride=0 | feedあり、path=1.0 | tick設定、override=0時は最小tick | 正常/境界 |
| TC-FEED-004 | `NcFeed_PrepareDwellTs` | NULL、最大超過、0tick境界、完了更新 | dwell=0, max+1 | 0は1tick、最大超過は範囲異常 | 境界/異常 |
| TC-FEED-005 | `NcFeed_BeginBlockRt/ProfileRatioRt` | override=0 HOLD、比率端点 | tick=0/中間/終端 | HOLD、比率0付近/中間/1.0 | 状態/境界 |
| TC-COORD-001 | `NcCoordinate_Reset/FillTargets` | 初期状態、NULL No-op | reset, NULL | MM/絶対/座標0 | 正常/異常 |
| TC-COORD-002 | `NcCoordinate_SetWorkOffsetAxis/SetLocalShiftAxis/SetProgramAxis` | ワーク+ローカルシフト反映 | offset=100, shift=20, X=500 | machine X=620、mask更新 | 正常/副作用 |
| TC-COORD-003 | `NcCoordinate_SetProgramAxis` | 増分座標 | Y=100後にY=-25 | program Y=75 | 境界/状態 |
| TC-COORD-004 | `NcCoordinate_SetMachineAxis/SetTemporaryAxis/SelectWork` | 機械座標逆算、一時座標、ワーク切替 | Z=1000等 | program逆算、temporary/localクリア | 正常/副作用 |
| TC-XFORM-001 | `NcCoordinateTransform_Reset/SetEnabled` | reset、ON/OFF、軸範囲外 | axis=AXIS_MAX | rejected加算 | 正常/異常 |
| TC-XFORM-002 | `NcCoordinateTransform_Set*` | 補正値設定とactive mask | dynamic/work/rotary補正 | active mask更新 | 正常/副作用 |
| TC-XFORM-003 | `NcCoordinateTransform_ApplyBlockTs` | NULL、非motion、補正ON/OFF | LINEAR/AUX_ONLY | target/start補正、非motion No-op | 分岐/異常 |
| TC-XFORM-004 | `NcCoordinateTransform_ApplyBlockTs` | 円弧中心補正 | ARC_CW + offset | arc_centerも補正 | 分岐 |
| TC-AXIS-001 | `NcAxisConfig_SetAxisDefinition` | 軸定義、名前正規化、不正値 | axis範囲外、`c` | Cへ正規化、mask更新 | 正常/異常 |
| TC-AXIS-002 | `NcAxisConfig_SetPathAxisMask/SetDetachedMask/SetDiameterMode` | 0mask、範囲外mask、直径mode | 各mask/axis | 拒否または状態更新 | 境界/異常 |
| TC-AXIS-003 | `NcAxisConfig_ApplyBlockTs` | 経路外軸除去、取り外し軸除去、直径→半径変換 | X/Y/Z軸ブロック | X半分、Y/Z除去、イベント | 分岐/副作用 |
| TC-AXIS-004 | `NcAxisConfig_ApplyBlockTs` | 非motion No-op | AUX_ONLY | block不変 | 分岐 |
| TC-RMCC-001 | `NcRotaryMcc_Reset/SetAxisRadius/SetMccOutput` | reset、半径境界、出力OFF | axis範囲外、min未満 | 拒否、出力bit OFF | 正常/異常/安全 |
| TC-RMCC-002 | `NcRotaryMcc_ApplyBlockTs` | モードON/OFF、仮想周速算出 | G07.1/G13.1, A軸360度 | radius相当のvirtual delta | 正常/分岐 |
| TC-RMCC-003 | `NcRotaryMcc_OnBlockRt/ServiceRt` | 実行ブロック反映、RUNNING時出力ON、ALARM時OFF | machine RUNNING/ALARM | command bit ON/OFF | フェイルセーフ |

### 13.3 追加ファイル

```text
source/tests/test_nc_codes.c
source/tests/test_nc_feed.c
source/tests/test_nc_coordinate.c
source/tests/test_nc_coordinate_transform.c
source/tests/test_nc_axis_config.c
source/tests/test_nc_rotary_mcc.c
source/scripts/build_tests_clang.sh
source/scripts/run_tests_clang.sh
```

`build_tests.bat` と `run_tests.bat` も、上記6本の追加テストを含めるよう更新済みです。

### 13.4 追加後の確認結果

この実行環境では Windows `cl.exe` は実行できないため、Linux上の `clang + lld` でビルド・実行確認しました。追加後の `build/test_results.txt` には、既存4本 + 追加6本、合計10本のテスト実行結果を保存しています。

成功対象:

```text
test_control_api
test_nc_buffer
test_nc_diagnostics
test_nc_motion_filter
test_nc_codes
test_nc_feed
test_nc_coordinate
test_nc_coordinate_transform
test_nc_axis_config
test_nc_rotary_mcc
```

### 13.5 追加後も残るカバレッジ注意点

- `nc_parser.c` / `nc_parser_tokens.c` / `nc_parser_modal.c` / `nc_parser_feature.c` は、入力文字列パターンが多いため別途パーサ専用テストを作るのが望ましいです。
- `nc_interpolation.c`、`nc_cycle.c`、`rt_control.c` はRT周期シミュレーション付きで、複数tickを進めるテストが必要です。
- `nc_spindle.c`、`nc_tool_management.c`、`nc_synchronization.c`、`nc_precision.c` は状態機械とイベント出力が中心のため、次回の追加候補です。
- 今回 `NcFeed_PrepareMotionTs` など一部ヘッダ未公開の内部TS関数は `extern` 宣言で直接テストしています。製品コード側で正式に単体テスト対象とするなら、テスト用公開ヘッダまたは内部ヘッダへ宣言を集約する案があります。


---

## 14. 追加単体テスト v2：対象関数拡張

### 14.1 追加対象関数分析

| 関数名 | 役割 | 入力 | 出力 | 依存するグローバル変数 | 外部依存 |
| --- | --- | --- | --- | --- | --- |
| `NcCapability_Reset` / `NcCapability_OnParsedBlockTs` / `NcCapability_RecordUnsupported` | G/M/アドレス/機能フラグの対応可否診断、未対応コード検出 | `NC_EXEC_BLOCK*`, コード種別 | 診断状態、エラーコード、イベント | `g_ncCapabilityStatus`, `g_ncEventRing` | なし |
| `NcInterference_Reset` / `NcInterference_SetEnabled` / `NcInterference_CheckBlockTs` | 軸位置・工具半径・禁止領域による干渉チェック | 実行ブロック、禁止領域設定 | エラー、警告、干渉イベント | `g_ncInterferenceStatus`, `g_ncEventRing` | センサ/実機形状はテスト値で代替 |
| `NcSafetyMotion_Reset` / `NcSafetyMotion_UpdateRt` / `NcSafetyMotion_ApplyBlockRt` | 非常停止、減速停止、オーバトラベル時の安全側出力 | `g_ioIn`, `g_machineCtx`, 実行ブロック | 安全状態、停止要求、出力マスク | `g_ncSafetyMotionStatus`, `g_ioOut`, `g_machineCtx` | 外部入力は共有メモリ初期値でスタブ化 |
| `NcToolManagement_Reset` / `NcToolManagement_SetPocket` / `NcToolManagement_RequestPrepare` / `NcToolManagement_OnParsedBlockTs` / `NcToolManagement_ServiceRt` | 工具ポケット設定、工具準備、M06/MFIN待ち、二重登録検出 | 工具番号、ポケット、Mコード、センサ入力 | 工具状態、待ち状態、イベント | `g_ncToolManagementStatus`, `g_ioIn`, `g_ioOut`, `g_ncEventRing` | PLC/MFINは共有メモリで模擬 |
| `NcSpindle_Reset` / `NcSpindle_SetSpeedLimit` / `NcSpindle_SetOrientWindow` / `NcSpindle_OnParsedBlockTs` / `NcSpindle_ServiceRt` | 主軸速度制限、CSS、停止、オリエント完了/タイムアウト | S値、Mコード、主軸角度 | 主軸状態、出力指令、タイムアウト | `g_ncSpindleStatus`, `g_ioIn`, `g_ioOut`, `g_ncEventRing` | 主軸フィードバックは共有メモリで模擬 |
| `NcPrecision_Reset` / `NcPrecision_SetLearningControl` / `NcPrecision_SetVibrationControl` / `NcPrecision_SetPreviewControl` / `NcPrecision_OnParsedBlockTs` / `NcPrecision_ServiceRt` | 高精度モード、学習制御、振動抑制、プレビュー制御 | Gコード、制御パラメータ、軸負荷 | 状態、補正量、警告 | `g_ncPrecisionStatus`, `g_ioIn`, `g_ioOut` | 負荷/振動入力は共有メモリで模擬 |
| `NcSynchronization_Reset` / `NcSynchronization_SetMasterSlave` / `NcSynchronization_SetOverlayAxis` / `NcSynchronization_SetCompoundPathMask` / `NcSynchronization_ApplyBlockRt` | 同期、重ね合わせ、複合制御、双テーブル制御 | 軸設定、マスク、実行ブロック | 軸目標値、同期状態、イベント | `g_ncSynchronizationStatus`, `g_ncEventRing` | なし |
| `NcCompensation_Reset` / `NcCompensation_SetToolLengthOffset` / `NcCompensation_SetCutterRadiusOffset` / `NcCompensation_ApplyBlockTs` | 工具長補正、工具径補正、キャンセル、異常オフセット | H/D番号、実行ブロック | 軸目標値、補正状態、エラー | `g_ncCompensationStatus`, `g_ncEventRing` | なし |
| `NcCycle_ResetParserModal` / `NcCycle_InitParserBlock` / `NcCycle_ApplyCycleGCode` / `NcCycle_FinalizeParserBlock` / `NcCycle_BuildSegmentPlanRt` | 固定サイクルモーダル、R点/feed未指定検出、G81セグメント生成 | Gコード、Q/P/R/F相当値、ブロック | セグメント計画、エラー | `g_ncCycleStatus` | なし |
| `NcReference_Reset` / `NcReference_SetAxisConfig` / `NcReference_SetOneDirectionApproach` / `NcReference_OnParsedBlockTs` / `NcReference_ServiceRt` | レファレンス点復帰、原点マーカ検出、1方向復帰 | 軸設定、Gコード、センサ入力 | 復帰状態、軸位置、イベント | `g_ncReferenceStatus`, `g_ioIn`, `g_ioOut`, `g_ncEventRing` | 原点マーカ入力は共有メモリで模擬 |
| `NcLatheCycle_Reset` / `NcLatheCycle_SetDiameterMode` / `NcLatheCycle_OnParsedBlockTs` / `NcLatheCycle_OnBlockRt` | 旋削/研削/ねじ切りサイクルの計画状態とイベント | Gコード、X/Z値、実行ブロック | サイクル状態、イベント | `g_ncLatheCycleStatus`, `g_ncEventRing` | なし |
| `NcKinematics_Reset` / `NcKinematics_SetAxisAssignment` / `NcKinematics_SetMirrorMask` / `NcKinematics_ApplyBlockTs` | 軸割付、ミラー、退避、傾斜面、工具軸特異点警告 | 軸設定、実行ブロック | 軸目標値、警告、状態 | `g_ncKinematicsStatus` | なし |
| `NcInterp_PlaneAxes` / `NcInterp_SignedSweep` / `NcInterp_PrepareArcTs` | 平面選択、円弧方向、IJK/R指定妥当性 | 平面、円弧ブロック | 円弧計画、エラー | なし | なし |

### 14.2 追加単体テスト観点一覧

| No | 観点 | 内容 | 優先度 |
| -- | -- | -- | --- |
| UT-ADD2-01 | 安全側出力 | 非常停止、減速停止、オーバトラベル時に出力が安全側へ倒れること | 高 |
| UT-ADD2-02 | 状態機械 | 工具交換、主軸オリエント、原点復帰など複数周期状態が正しく進むこと | 高 |
| UT-ADD2-03 | 設定値境界 | 軸番号、工具番号、速度、窓幅、サイクル繰返し回数の範囲外を拒否すること | 高 |
| UT-ADD2-04 | 補正副作用 | 工具長/工具径/軸割付/ミラーにより実行ブロックが意図どおり更新されること | 高 |
| UT-ADD2-05 | イベント登録 | 異常・完了・警告イベントがリングに登録されること | 中 |
| UT-ADD2-06 | NULL/未初期化相当 | NULLブロック、未設定パラメータ、未設定サイクルを異常扱いにすること | 高 |
| UT-ADD2-07 | RT禁止事項 | 追加テスト対象のRT関数がテスト内でファイルI/Oや動的確保に依存しないこと | 高 |

### 14.3 追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
| ----- | ---- | ----- | ---- | ---- | -- |
| TC-ADD2-001 | `NcCapability_*` | 対応G/M/アドレスと未対応コードを判定 | 対応コード、不正コード、未対応機能フラグ | カウンタ・エラー・イベント更新 | 正常/異常 |
| TC-ADD2-002 | `NcInterference_*` | 禁止領域内外、工具半径膨張、無効時No-op | 禁止領域、軸目標、工具半径 | 干渉時のみエラー | 正常/異常/境界 |
| TC-ADD2-003 | `NcSafetyMotion_*` | 非常停止、減速停止、オーバトラベル | 入力ビット、機械状態、軸目標 | ALARM/安全側出力/停止要求 | 異常/安全 |
| TC-ADD2-004 | `NcToolManagement_*` | ポケット重複、M06即完了、MFIN待ち完了 | T/Mコード、MFIN入力 | 状態遷移・出力・イベント更新 | 正常/異常 |
| TC-ADD2-005 | `NcSpindle_*` | 速度制限、CSS、M05、オリエント完了/タイムアウト | S値、Mコード、角度FB | clamp/停止/完了/タイムアウト | 正常/異常/境界 |
| TC-ADD2-006 | `NcPrecision_*` | G08等のモード有効化、学習/振動補正 | Gコード、負荷、補正パラメータ | 状態・補正出力・警告更新 | 正常/境界 |
| TC-ADD2-007 | `NcSynchronization_*` | 同期軸、重ね合わせ、複合制御 | 軸設定、マスク、目標値 | slave軸/overlay/compound状態更新 | 正常/境界 |
| TC-ADD2-008 | `NcCompensation_*` | H/D境界、G43/G49、G41補正、NULL/不正オフセット | H/D番号、XY/Z移動 | 軸目標補正またはエラー | 正常/異常/境界 |
| TC-ADD2-009 | `NcCycle_*` | 固定サイクルの未指定異常、モーダル再利用、G81セグメント | R/F/Q/P相当値 | エラーまたはセグメント生成 | 正常/異常 |
| TC-ADD2-010 | `NcReference_*` | 原点復帰、原点マーカ検出、1方向復帰 | G28/G30相当、マーカ入力 | 完了状態・軸位置更新 | 正常/境界 |
| TC-ADD2-011 | `NcLatheCycle_*` | 直径モード、旋削/研削/ねじ切り計画イベント | Gコード、X/Z、MOTION | 状態・イベント更新 | 正常 |
| TC-ADD2-012 | `NcKinematics_*` | 軸入替、符号反転、ミラー、退避、特異点警告 | 軸設定、角度 | 軸目標・警告更新 | 正常/異常/境界 |
| TC-ADD2-013 | `NcInterp_*` | 平面選択、R/IJK競合、幾何不足、IJK円弧計算 | 円弧指定 | エラーまたは円弧計画生成 | 正常/異常/境界 |

### 14.4 追加したテストコード

以下のファイルを追加しました。

```text
source/tests/test_nc_capability.c
source/tests/test_nc_interference.c
source/tests/test_nc_safety_motion.c
source/tests/test_nc_tool_management.c
source/tests/test_nc_spindle.c
source/tests/test_nc_precision.c
source/tests/test_nc_synchronization.c
source/tests/test_nc_compensation.c
source/tests/test_nc_cycle.c
source/tests/test_nc_reference.c
source/tests/test_nc_lathe_cycle.c
source/tests/test_nc_kinematics.c
source/tests/test_nc_interp_math.c
```

既存10本と合わせ、テスト実行ファイルは合計23本です。

### 14.5 ビルド・実行方法

Windows / Visual Studio / `cl.exe` 用スクリプトは以下です。

```bat
cd source
scripts\build_tests.bat
scripts\run_tests.bat
```

結果は以下へ出力します。

```text
source\build\test_results.txt
```

GCC/clang確認用の補助スクリプトも同梱しています。

```sh
cd source
scripts/run_tests_gcc.sh
scripts/run_tests_clang.sh
```

### 14.6 追加後の確認結果

この実行環境では Windows `cl.exe` は実行できないため、Linux上の `gcc` でビルド・実行確認しました。`build/test_results.txt` には、既存10本 + 追加13本、合計23本の実行結果を保存しています。

成功対象:

```text
test_control_api
test_nc_buffer
test_nc_diagnostics
test_nc_motion_filter
test_nc_codes
test_nc_feed
test_nc_coordinate
test_nc_coordinate_transform
test_nc_axis_config
test_nc_rotary_mcc
test_nc_capability
test_nc_interference
test_nc_safety_motion
test_nc_tool_management
test_nc_spindle
test_nc_precision
test_nc_synchronization
test_nc_compensation
test_nc_cycle
test_nc_reference
test_nc_lathe_cycle
test_nc_kinematics
test_nc_interp_math
```

### 14.7 追加後も残るカバレッジ注意点

- `nc_parser.c` / `nc_parser_tokens.c` / `nc_parser_modal.c` / `nc_parser_feature.c` は、NC文字列入力パターンが多いため、字句解析・構文解析専用テストとして別途拡充が必要です。
- `rt_control.c` / `ts_service.c` は実周期・TS周期・共有メモリ競合を含むため、単体テストだけでなく周期シミュレーション付きの結合テストが必要です。
- `main.c` は起動・終了・割込待ちを含むため、単体テストよりも起動シナリオテストの対象です。
- 干渉チェック、工具径補正、5軸幾何、主軸CSSは実機仕様・機械寸法・サーボ仕様に依存するため、現在のテストはソフトウェア分岐と副作用確認に限定しています。
- RT処理時間の上限確認は、Windows実機または計測用タイマを使った性能テストで別途確認してください。

### 14.8 追加で確認すべき仕様不明点

- 工具ポケット重複時に警告継続とするか、即ALARMにするか。
- 主軸CSS時の最大回転数clamp、最低回転数、停止判定の正式なしきい値。
- 干渉チェックの禁止領域を機械座標で扱うか、ワーク座標補正後で扱うか。
- 工具径補正G41/G42のアプローチ/キャンセル時の正式な幾何仕様。
- 原点復帰の1方向復帰時、マーカ検出後の戻し量・低速再進入量。
- 同期/重ね合わせ/複合制御の軸競合時の優先順位。
- 円弧R指定で大円弧/小円弧をどの符号規約で選択するか。

---

## 15. 今回追加した単体テスト v4

前回までの23本に加えて、共有メモリ初期化、TS_SERVICE、RT_CONTROL、先読み補正、補間実行、NCパーサを対象に6本の単体テストを追加しました。合計テスト実行単位は29本です。

### 15.1 追加対象関数分析

| 関数名 | 役割 | 入力 | 出力 | 依存するグローバル変数 | 外部依存 |
| --- | --- | --- | --- | --- | --- |
| `SystemShared_Initialize` | 共有メモリ/グローバル状態の安全初期化 | なし | 各共有メモリの初期値 | `g_ioIn`, `g_ioOut`, `g_machineCtx`, `g_uiRequest`, `g_logQueue`, `g_prefetchResult` ほか | なし |
| `LogQueue_Push` | 固定長ログキュー登録 | level, code, arg | push成功/失敗、write_index/count | `g_logQueue` | なし |
| `NcEvent_Push` | NCイベントリング登録 | code, a0, a1 | event count/lost count | `g_ncEventRing` | なし |
| `TsService_ExecutePrefetch` | TS側先読み要求処理 | `g_uiRequest.prefetch_request` | `g_prefetchResult` 更新 | `g_uiRequest`, `g_prefetchResult` | TS依存関数はスタブ化 |
| `TsService_HandleLogFlush` | ログフラッシュ要求処理 | `g_uiRequest.log_flush_request` | 要求クリア、ログ登録 | `g_uiRequest`, `g_logQueue` | ファイルI/Oなし |
| `TsService_BuildUiSnapshot` | UI向けスナップショット生成 | 共有メモリ状態 | `g_uiSnapshot` 更新 | `g_machineCtx`, `g_uiSnapshot` | なし |
| `RtControl_UpdatePrestartInterlock` | 運転開始前インタロック集約 | 入力信号/状態 | interlock mask/status | `g_ioIn`, `g_machineCtx` | なし |
| `RtControl_UpdateMachineState` | RT主状態遷移 | モード/状態/要求 | `g_machineCtx.state` | `g_machineCtx` | なし |
| `RtControl_UpdateAxisTargets` | RT出力軸指令更新 | target position | `g_ioOut.axis_cmd[]` | `g_machineCtx`, `g_ioOut` | モーション出力は共有メモリのみ |
| `RtControl_ProcessModeRequest` | API要求のRT側確定 | `g_uiRequest.run_mode_request` | 応答コード、モード更新 | `g_uiRequest`, `g_machineCtx` | なし |
| `RtControl_UpdateInterlock` | インタロック発生時の安全側遷移 | 入力異常/状態 | ALARM遷移、安全出力 | `g_ioIn`, `g_ioOut`, `g_machineCtx` | なし |
| `RtControl_CaptureIoTrace` | I/Oトレースリング更新 | 現在I/O | trace count/freeze | `g_ioIn`, `g_ioOut`, `g_ioTraceRing` | なし |
| `NcLookahead_ApplyBlockTs` | TS側先読み補正/速度調整 | `NC_EXEC_BLOCK` | feature flags/feed更新 | `g_ncLookaheadStatus` | なし |
| `NcInterpolation_PrepareBlockTs` | 補間ブロック準備 | `NC_EXEC_BLOCK` | 補間状態 | `g_ncInterpolationStatus` | なし |
| `NcInterpolation_BeginBlockRt` | RT補間開始 | prepared block | active状態 | `g_ncInterpolationStatus` | なし |
| `NcInterpolation_StepRt` | RT補間1tick実行 | sensor input/補間状態 | axis command/完了状態 | `g_ioIn`, `g_ioOut`, `g_ncInterpolationStatus` | センサ入力はスタブ/共有メモリで設定 |
| `NcInterpolation_Cancel` | 補間キャンセル | なし | active解除 | `g_ncInterpolationStatus` | なし |
| `NcParse_LineToBlock` | NC文字列1行を固定長ブロックへ変換 | NC文字列、行番号 | `NC_EXEC_BLOCK`, error code | parser modal状態 | なし |

### 15.2 追加テスト観点一覧

| No | 観点 | 内容 | 優先度 |
| -- | -- | -- | --- |
| V4-01 | 共有メモリ初期化 | RT/TS/APIで共有するグローバル状態が安全側初期値になること | 高 |
| V4-02 | ログ/イベントリング境界 | 固定長リングの満杯、wrap、lost countを確認 | 高 |
| V4-03 | TS_SERVICE副作用 | 先読み、ログフラッシュ、UI snapshotの副作用を確認 | 高 |
| V4-04 | RT_CONTROL安全遷移 | インタロック発生時にALARM/安全出力へ遷移すること | 高 |
| V4-05 | API要求のRT確定 | 不正モード/不正状態で要求が拒否されること | 高 |
| V4-06 | I/Oトレース | リング更新とアラーム時freezeを確認 | 中 |
| V4-07 | 先読み補正 | G08/高精度制御/コーナ減速/非motion no-op | 中 |
| V4-08 | 補間RT実行 | begin/step/completion/cancel/skip early completion | 高 |
| V4-09 | NCパーサ正常系 | G/M/S/T/F/座標/コメント付き入力の解析 | 高 |
| V4-10 | NCパーサ異常系 | invalid token、未対応G、modal conflict、座標系エラー | 高 |

### 15.3 追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
| ----- | ---- | ----- | ---- | ---- | -- |
| UT-V4-001 | `SystemShared_Initialize` | 安全初期値確認 | 初期化呼出 | STOPPED/安全出力/要求クリア | 正常/副作用 |
| UT-V4-002 | `LogQueue_Push` | ログキュー満杯境界 | 最大件数+1登録 | 最大件数まで登録、超過は失敗 | 境界 |
| UT-V4-003 | `NcEvent_Push` | イベントリングwrap | 最大件数超過登録 | lost count増加、最新イベント保持 | 境界 |
| UT-V4-004 | `TsService_ExecutePrefetch` | 先読み要求処理 | prefetch request ON | result ready、request clear | 正常 |
| UT-V4-005 | `TsService_ExecutePrefetch` | busy中no-op | result busy | 状態を壊さない | 分岐 |
| UT-V4-006 | `TsService_HandleLogFlush` | ログflush要求 | request ON | request clear、ログ登録 | 正常/副作用 |
| UT-V4-007 | `TsService_BuildUiSnapshot` | UI snapshot更新 | machine state設定 | message id増加、状態反映 | 正常 |
| UT-V4-008 | `RtControl_UpdatePrestartInterlock` | 開始前インタロックOK/NG | 安全入力ON/OFF | mask/status更新 | 分岐 |
| UT-V4-009 | `RtControl_UpdateMachineState` | READY/RUNNING遷移 | AUTO/READY等 | 想定状態遷移 | 状態遷移 |
| UT-V4-010 | `RtControl_UpdateAxisTargets` | 軸出力更新 | target position | axis_cmd反映 | 正常/副作用 |
| UT-V4-011 | `RtControl_ProcessModeRequest` | 要求受理/拒否 | 正常/不正モード | response accepted/rejected | 異常 |
| UT-V4-012 | `RtControl_UpdateInterlock` | 異常時フェイルセーフ | interlock input NG | ALARM、安全出力 | 安全 |
| UT-V4-013 | `RtControl_CaptureIoTrace` | trace/freeze | ALARM発生 | freeze設定 | 副作用 |
| UT-V4-014 | `NcLookahead_ApplyBlockTs` | G08 smoothing | G08 P2 | smoothing/high precision flag | 正常 |
| UT-V4-015 | `NcLookahead_ApplyBlockTs` | コーナ減速 | 直交方向連続motion | feed override低下 | 分岐 |
| UT-V4-016 | `NcLookahead_ApplyBlockTs` | NULL/non-motion | NULL/noop block | no-op | early return |
| UT-V4-017 | `NcInterpolation_PrepareBlockTs` | NULL/noop | NULL/noop block | 異常なし/no-op | early return |
| UT-V4-018 | `NcInterpolation_BeginBlockRt`/`StepRt` | 直線補間完了 | linear block | tick進行、完了 | 正常 |
| UT-V4-019 | `NcInterpolation_Cancel` | 補間キャンセル | active中cancel | active解除 | 分岐 |
| UT-V4-020 | `NcInterpolation_StepRt` | G31 skip早期完了 | skip入力ON | skip検出、block完了 | 外部入力異常系 |
| UT-V4-021 | `NcParse_LineToBlock` | 通常NC行解析 | G01/Y/Z/F/S/T/M | block各フィールド設定 | 正常 |
| UT-V4-022 | `NcParse_LineToBlock` | 空行/comment | comment only | no-op扱い | 境界 |
| UT-V4-023 | `NcParse_LineToBlock` | token error/unsupported | `#`, `G999` | エラーコード返却 | 異常 |
| UT-V4-024 | `NcParse_LineToBlock` | modal conflict | `G90 G91`, `G20 G21` | modal conflict | 異常 |
| UT-V4-025 | `NcParse_LineToBlock` | feature boundary | `G08`, `G08 P2` | range error/feature flag | 境界 |

### 15.4 追加したテストコード

| ファイル | 内容 |
| --- | --- |
| `tests/test_system_shared.c` | 共有メモリ初期化、ログキュー、NCイベントリング |
| `tests/test_ts_service.c` | TS_SERVICEの先読み、ログflush、UI snapshot |
| `tests/test_rt_control.c` | RT_CONTROLのインタロック、状態遷移、安全出力、I/O trace |
| `tests/test_nc_lookahead.c` | G08/高精度制御/コーナ減速/No-op |
| `tests/test_nc_interpolation.c` | 補間準備、開始、tick、cancel、G31 skip |
| `tests/test_nc_parser.c` | NC行解析、コメント、未対応G、modal conflict、feature境界 |

### 15.5 追加したスタブ・モックコード

| ファイル | 内容 |
| --- | --- |
| `stubs/stub_rt_control_deps.c` | RT_CONTROL単体ビルド用にNC診断/安全/主軸/工具/補間系依存を最小スタブ化 |
| `stubs/stub_ts_service_deps.c` | TS_SERVICE単体ビルド用にNCプログラムslice処理を最小スタブ化 |

### 15.6 ビルド・実行方法

Windows / Visual Studio / `cl.exe` 前提の一括実行は以下です。

```bat
cd source
scripts\run_tests.bat
```

結果は以下に出力します。

```text
source\build\test_results.txt
```

### 15.7 追加後の確認結果

この実行環境では Windows `cl.exe` が使用できないため、追加6本はGCCで個別ビルド・実行確認しました。前回までの23本の結果と、今回追加6本の結果を `build/test_results.txt` に保存しています。

追加6本の確認結果:

```text
test_system_shared: PASS
test_ts_service: PASS
test_rt_control: PASS
test_nc_lookahead: PASS
test_nc_interpolation: PASS
test_nc_parser: PASS
```

### 15.8 追加後も残るカバレッジ注意点

- `NcParse_LineToBlock` はGコード/Mコード/アドレスの組み合わせが多く、今回のテストは代表パターンです。実機互換を確認するには、Gコード体系別の大量パターンテストが必要です。
- `RtControl_UpdateMachineState` は周期呼出と割込タイミングの影響を受けるため、単体テストでは分岐確認まで、周期ジッタや割込復帰は結合/機能テスト対象です。
- `NcInterpolation_StepRt` は直線補間とskip早期完了を確認していますが、円弧、NURBS、固定サイクル連携、フィルタ遅れ補正は別テストで拡充が必要です。
- `TsService_ExecutePrefetch` はファイルI/Oを伴わない範囲で確認しています。実NCファイル読込、文字コード、長大ファイル、異常行継続可否は結合テスト対象です。

### 15.9 追加で確認すべき仕様不明点

- NCパーサで小数点のみ表記、符号のみ、指数表記、空白なし複数アドレスをどこまで正式対応するか。
- `G08` のパラメータ省略時をエラーにするか、既定値扱いにするか。
- G31 skip検出時に補間を即完了するか、減速停止セグメントを追加するか。
- インタロック発生時、既存RUNNING状態からALARMへ即時遷移する以外に、HOLD経由を許容する条件があるか。
- TS_SERVICEでprefetch busy中に新規要求が来た場合、破棄・待ち・上書き禁止のどれを正式仕様とするか。

---

# 追加単体テスト v4

## 追加対象関数分析

| 関数名 | 役割 | 入力 | 出力 | 依存するグローバル変数 | 外部依存 |
| --- | --- | --- | --- | --- | --- |
| `Api_SetNcCoordinateTransformEnabled` | 座標変換有効/無効のAPI経由設定 | dynamicFixture / workMountError / rotaryFixture | 戻り値、`g_ncCoordinateTransformStatus` | `g_ncCoordinateTransformStatus` | 座標変換モジュール。単体ではスタブ化 |
| `Api_SetNcReferenceAxisConfig` | レファレンス軸設定API | axis, referencePosition, approachDirection, rolloverLimit, marker | 戻り値 | `g_ncReferenceStatus` | レファレンスモジュール。単体ではスタブ化 |
| `Api_SetNcLearningControl` / `Api_SetNcVibrationSuppression` / `Api_SetNcPreviewControl` | 高精度・学習・振動抑制・プレビュー制御API | enabled, gain/window/frequency等 | 戻り値、ステータスコピー | `g_ncPrecisionStatus` | 高精度制御モジュール。単体ではスタブ化 |
| `Api_SetNcAxisDefinition` / `Api_SetNcPathAxisMask` / `Api_SetNcAxisDetachedMask` / `Api_SetNcAxisDiameterMode` | 軸構成API | 軸番号、軸名、マスク、直径モード | 戻り値、ステータスコピー | `g_ncAxisConfigStatus` | 軸構成モジュール。単体ではスタブ化 |
| `NcParser_InitBlock` | NC実行ブロック初期化 | `NC_EXEC_BLOCK*`, lineNo | ブロック初期値 | `g_ncCoordinateState` | 座標・固定サイクル依存はスタブ化 |
| `NcParser_SetFeed` / `NcParser_SetSpindle` | モーダル送り・主軸値設定 | feed, spindleSpeed | ブロック、モーダル保持値 | parser static modal state | なし |
| `NcParser_ApplyGCode` | Gコードモーダル適用 | code10, block, parse context | motion/plane/feed/distance等 | `g_ncCoordinateState`, parser modal state | コード表、補助依存はスタブ化 |
| `NcParser_FinalizeBlock` | ブロック確定処理 | block, error | motion_type, feature flags, error | parser modal state | 補正/軸/補間依存はスタブ化 |
| `NcParser_ApplyToken` | アドレスワード適用 | address, value, block, context | 軸、F/S/T/M/P/Q/R等 | `g_ncCapabilityStatus`, `g_ncCoordinateState` | コード表、座標・固定サイクル依存はスタブ化 |
| `NcProgram_RequestBinaryLoad` | バイナリNCブロックロード要求 | block array, count | 戻り値、要求共有メモリ | `g_ncBinaryProgramRequest`, `g_ncProgramRequest` | なし |
| `TsNcProgram_ExecuteSlice` | TS側バイナリロード確定 | 共有メモリ要求 | バッファ投入、READY状態 | `g_ncProgramBuffer`, `g_ncProgramStatus`, `g_ncBinaryProgramStatus` | パーサ/補正/診断依存はスタブ化 |
| `RtNcProgram_ProcessRequests` | RT側開始/停止/HOLD/RESUME要求処理 | safetyOk, 共有メモリ要求 | 状態遷移、応答コード | `g_machineCtx`, `g_ncProgramStatus`, `g_ncProgramRequest` | 補間キャンセルはスタブ化 |
| `RtNcProgram_ConsumeBlocks` | RT側NCブロック消費 | 共有NCバッファ | 出力共有メモリ、完了状態 | `g_ioOut`, `g_ncAuxStatus`, `g_ncProgramStatus` | 補間・診断・主軸等はスタブ化 |

## 追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
| --- | --- | --- | --- | --- | --- |
| UT-API-EXT-001 | API座標変換 | NULL snapshotと有効化API | snapshot=NULL / enable=(1,0,2) | NULLは-1、状態コピーは1/0/1 | 異常・正常 |
| UT-API-EXT-002 | APIレファレンス/精度/軸設定 | 未対象APIの戻り値伝搬 | valid/invalid axis, path mask, diameter | invalidは-1、validは0、状態反映 | 分岐・境界 |
| UT-API-EXT-003 | API getter群 | 追加getterのNULLとコピー | prestart/aux/io trace/event ring | NULLは-1、コピー値一致 | 異常・副作用 |
| UT-PARSER-MODAL-001 | `NcParser_InitBlock` | モーダル初期値と保持値 | feed/spindle設定後の次ブロック | feed/spindle保持、aux flagはブロック単位 | 正常・副作用 |
| UT-PARSER-MODAL-002 | `NcParser_ApplyGCode` | Gコード同一グループ競合 | G17→G18、G90→G91、G01→G00 | 2回目は-3 | 異常・分岐 |
| UT-PARSER-MODAL-003 | `NcParser_FinalizeBlock` | 空モーションのNo-op/AUX_ONLY化 | G01のみ、G01+S | NONEまたはAUX_ONLYへ変換 | 分岐 |
| UT-PARSER-TOKEN-001 | `NcParser_ApplyToken` | X/Y/F/S/T/Mを個別適用 | G01 X/Y/F/S/T/M | 軸mask、固定小数、補助flag反映 | 正常 |
| UT-PARSER-TOKEN-002 | `NcParser_ApplyToken` | token異常系 | `?`, S=-1, M=-1, G=1000 | 負値/不正値でエラー | 異常・境界 |
| UT-PARSER-TOKEN-003 | `NcParser_ApplyToken` | P/Rの文脈依存 | G04 P, G02 R, G81 R | dwell ticks / arc radius / Rパラメータ | 分岐 |
| UT-NCPROG-001 | `NcProgram_RequestBinaryLoad` / `TsNcProgram_ExecuteSlice` | バイナリロード要求とTS確定 | NULL/0/上限超過/2block | invalid=-1、READY、buffer count=2 | 境界・正常 |
| UT-NCPROG-002 | `RtNcProgram_ProcessRequests` / `RtNcProgram_ConsumeBlocks` | AUTO/安全OK時の開始とAUX消費 | MANUAL、safety NG、AUTO OK | reject後、RUNNING→COMPLETED、出力反映 | 状態遷移 |
| UT-NCPROG-003 | `RtNcProgram_ProcessRequests` | HOLD/RESUME/STOP/ALARM分岐 | RUNNING/HOLD/ALARM | HOLD, RUNNING, COMPLETED, NC_ERROR | 状態遷移・異常 |

## 追加スタブ

| ファイル | 用途 |
| --- | --- |
| `stubs/stub_parser_deps.c` | パーサのモーダル/トークン単体テスト用に、座標・固定サイクル・補正・軸構成・補間準備などを軽量スタブ化 |
| `stubs/stub_nc_program_deps.c` | `nc_program.c` のバイナリロード/RT消費テスト用に、補間・診断・補正・主軸・工具・同期などの周辺処理を軽量スタブ化 |

## 追加後の実行結果

- 既存29本に加えて4本を追加し、合計33本構成。
- 追加4本はGCCで個別ビルド・実行し、全件PASSを確認。
- `build/test_results.txt` に既存結果と追加4本の結果を追記済み。

## 追加カバレッジ上の注意点

- `nc_program.c` のテキストファイルI/Oパスは、RT単体テストではなくTS結合寄りのため、今回の追加ではバイナリロード経路を優先した。
- パーサの直接テストは依存モジュールをスタブ化しているため、座標変換・補正・補間準備との連鎖は既存の `test_nc_parser.c` および結合テスト側で確認する。
- MFIN待ち継続、補間active中の複数周期ステップ、実ファイルの長行/EOF/読取異常は追加候補として残す。

---

## 追加単体テスト v5

前回までの33本に加えて、以下3本を追加しました。

| 追加テストファイル | 主な対象関数 | 追加観点 |
|---|---|---|
| `tests/test_nc_parser_feature_direct.c` | `NcParser_ApplyFeatureG`, `NcParser_FinalizeFeatureBlock` | G08/G05の重複・P未指定・範囲外・HPCC・高速モード、同期/複合/重ね合わせ、工具長/工具径/座標回転/極座標/傾斜面/工具軸/ミラー/軸退避 |
| `tests/test_nc_codes_extended.c` | `NcCodes_GetGCodeInfo`, `NcCodes_GetAddressInfo`, `NcCodes_GetMCodeInfo` | NULL、未対応G、英小文字アドレス正規化、Mコード既定範囲、999/1000境界 |
| `tests/test_nc_program_feature_status.c` | `NcProgram_RequestBinaryLoad`, `TsNcProgram_ExecuteSlice`, `RtNcProgram_ProcessRequests`, `RtNcProgram_ConsumeBlocks` | G08/G05実行時の機能状態更新、同期/複合/重ね合わせON/OFF、MFIN待ち・待機サイクル・完了カウンタ |

### v5追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
|---|---|---|---|---|---|
| TC-V5-001 | `NcParser_ApplyFeatureG` | G08/G05平滑化コマンドの同一ブロック重複 | G08適用後にG05適用 | 2回目は`-3`、`smoothing_seen`保持 | 異常/分岐 |
| TC-V5-002 | `NcParser_ApplyFeatureG` | 同期・複合・重ね合わせ・高度補間Gの直接適用 | G51.4/G50.5/G51.6/G6.2 | 対応feature flagとmotion typeを設定 | 正常/分岐 |
| TC-V5-003 | `NcParser_ApplyFeatureG` | 座標・補正系Gの直接適用 | G68.2/G53.1/G51.1/G10.6/G16/G42/G44/G68 | コマンド種別、coord_mode、feature flag更新 | 正常/副作用 |
| TC-V5-004 | `NcParser_FinalizeFeatureBlock` | G08/G05 finalizeのPワード境界 | Pなし/P2/P10000/P3/P17/工具径補正中 | feature flag設定、`NC_ERR_RANGE`、`NC_ERR_MODAL_CONFLICT` | 境界/異常 |
| TC-V5-005 | `NcCodes_GetGCodeInfo` | decimal Gと未対応G | G51.6/G76.7/G1234/NULL | 対応情報取得、未対応・NULLは`-1` | 正常/異常 |
| TC-V5-006 | `NcCodes_GetAddressInfo` | アドレス小文字正規化 | `x`, `w`, `?`, NULL | X/W情報取得、未知/NULLは`-1` | 境界/異常 |
| TC-V5-007 | `NcCodes_GetMCodeInfo` | Mコード既定範囲境界 | M6/M999/M1000/NULL | M6はMFIN待ち、999対応、1000未対応 | 境界/異常 |
| TC-V5-008 | `RtNcProgram_ConsumeBlocks` | G08/G05機能状態のRT消費反映 | G08 P1/P2, G05 P10000/P0 | counters更新、HPCC/平滑化は最後にOFF | 状態遷移/副作用 |
| TC-V5-009 | `RtNcProgram_ConsumeBlocks` | 同期/複合/重ね合わせのON/OFF | G51.4/G50.4/G51.5/G50.5/G51.6/G50.6 | 各enabled最終状態がOFF、実行数更新 | 状態遷移 |
| TC-V5-010 | `RtNcProgram_ConsumeBlocks` | MFIN待ち補助指令 | AUX block + `NC_AUX_FLAG_MFIN_WAIT` | WAIT継続、wait_cycles加算、MFIN入力でIDLE/完了 | 外部入力スタブ/副作用 |

### v5確認結果

Linux/GCC環境で追加3本を個別にビルド・実行し、全件PASSを確認しました。Windows `cl.exe` での確認は本環境では未実施です。

```text
[new_tests_v5] PASS: 3 added test executables
```

---

## 追加単体テスト v6

前回までの36本に加えて、以下の5本を追加した。既存の `src/` / `inc/` は変更せず、`tests/` とビルド/実行スクリプトのみを追加・更新した。

| 追加テストファイル | 主な対象関数 | 追加観点 |
|---|---|---|
| `tests/test_nc_coordinate_transform_direct.c` | `NcCoordinateTransform_SetWorkOffset`, `NcCoordinateTransform_SetTemporaryAbsolute`, `NcCoordinateTransform_SetRotaryTableOffset`, `NcCoordinateTransform_ApplyBlockTs` | 軸範囲外、NULL、補正有効/無効、複数補正の合算、副作用状態 |
| `tests/test_nc_safety_motion_edges.c` | `NcSafetyMotion_UpdateRt`, `NcSafetyMotion_ApplyBlockRt` | POWER_FAIL優先、外部減速同時入力、立上りエッジ検出、絶対位置参照喪失、フェイルセーフtick補正 |
| `tests/test_nc_reference_edges.c` | `NcReference_OnParsedBlockTs`, `NcReference_OnBlockRt`, `NcReference_ServiceRt` | G30全軸復帰、ロールオーバ警告、距離コードマーカ、立上りエッジのみの捕捉 |
| `tests/test_nc_tool_management_timeout.c` | `NcToolManagement_RequestPrepare`, `NcToolManagement_OnParsedBlockTs`, `NcToolManagement_ServiceRt` | ポケット未割当、MFIN待ちタイムアウト、警告イベント登録 |
| `tests/test_nc_precision_edges.c` | `NcPrecision_OnParsedBlockTs`, `NcPrecision_OnBlockRt`, `NcPrecision_ServiceRt` | G08/G05キャンセル、AI高精度/自由曲面モード、学習補正の軸上限飽和 |

### 追加テストケース一覧 v6

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
|---|---|---|---|---|---|
| UTC-V6-001 | `NcCoordinateTransform_SetWorkOffset` ほか | 座標変換APIの範囲外軸・範囲外ワーク | `axis >= AXIS_MAX`, `workIndex >= NC_WORK_COORD_SYSTEMS` | `-1`、reject counter更新 | 異常系/境界値 |
| UTC-V6-002 | `NcCoordinateTransform_ApplyBlockTs` | 動的治具・取付誤差・ロータリ補正の合算 | X軸に `+10,-5,+7` | X指令が `+12` 補正、last target更新 | 正常系/副作用 |
| UTC-V6-003 | `NcSafetyMotion_UpdateRt` | POWER_FAILと外部減速の同時入力 | `SENSOR_POWER_FAIL_BIT | SENSOR_EXTERNAL_DECEL_BIT` | POWER_FAIL減速率を優先 | 分岐/安全系 |
| UTC-V6-004 | `NcSafetyMotion_UpdateRt` | 絶対位置参照喪失の立下り検出 | valid→invalid | loss counterが1回だけ増加 | 異常系/エッジ |
| UTC-V6-005 | `NcReference_OnBlockRt` | G30で軸指定なし | `G30`, `axis_mask=0` | 全軸復帰、G30 counter更新 | 正常系/境界 |
| UTC-V6-006 | `NcReference_ServiceRt` | 距離コードマーカ捕捉 | reference mark rising edge | capture counter更新、保持中は二重加算なし | 副作用/境界 |
| UTC-V6-007 | `NcToolManagement_ServiceRt` | MFIN待ちタイムアウト | M06 + MFIN_WAIT、MFINなし | exchange解除、timeout counter、warning event | 異常系/タイムアウト |
| UTC-V6-008 | `NcPrecision_OnParsedBlockTs` | G08/G05キャンセル | `G08 P2→P0`, `G05 P10000→P0` | preview/high precision状態解除 | 状態遷移 |
| UTC-V6-009 | `NcPrecision_ServiceRt` | 学習補正の軸上限飽和 | 出力が上限直前、補正あり | `NC_PRECISION_AXIS_TARGET_LIMIT` で飽和 | 境界値/安全側 |

### 追加実行結果 v6

GCCで追加5本を個別ビルド・実行し、全件PASSを確認した。結果は `build/test_results.txt` の末尾に追記済み。

```text
[new_tests_v6] PASS: 5 added test executables
```

---

# 追加単体テスト v7

前回の41本構成に対して、以下5本の単体テストを追加した。既存 `src/` / `inc/` は変更せず、`tests/` と `scripts/` のみを追加・更新した。

| 追加テストファイル | 主な対象関数 | 追加観点 |
|---|---|---|
| `tests/test_nc_feed_profile_boundaries.c` | `NcFeed_ProfileRatioRt`, `NcFeed_CancelRt`, `NcFeed_BeginBlockRt` | 0 tick、加減速なし、台形プロファイル境界、cancel副作用 |
| `tests/test_nc_cycle_segment_variants.c` | `NcCycle_BuildSegmentPlanRt`, `NcCycle_ApplyCycleGCode` | G82ドウェル、G83ペック、G88手動戻り、セグメント種別・カウンタ |
| `tests/test_nc_capability_history_wrap.c` | `NcCapability_OnParsedBlockTs`, `NcCapability_RecordUnsupported` | G/M履歴リングのラップ、未知kindのイベント副作用 |
| `tests/test_nc_interference_warning_edges.c` | `NcInterference_CheckBlockTs`, `NcInterference_SetEnabled` | 無効時No-op、非モーションNo-op、軸限界警告、円弧半径警告、減速副作用 |
| `tests/test_nc_spindle_orientation_edges.c` | `NcSpindle_SetSpeedLimit`, `NcSpindle_OnParsedBlockTs`, `NcSpindle_OnBlockRt`, `NcSpindle_ServiceRt` | CSS回転数クランプ、M03起動、M19位置決め完了、M19タイムアウト |

## 追加テストケース一覧 v7

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
|---|---|---|---|---|---|
| UT-V7-FEED-001 | `NcFeed_ProfileRatioRt` | `interp_ticks=0` と加減速なしの比率境界 | tick=0/4/8 | 0tickは1.0、加減速なしは線形比率 | 境界値 |
| UT-V7-FEED-002 | `NcFeed_ProfileRatioRt` | 台形プロファイルの単調性 | 10tick, accel/decel=2 | 0<r1<r2<r3<1、終端1.0 | 境界値 |
| UT-V7-FEED-003 | `NcFeed_CancelRt` | active中cancel | `NcFeed_BeginBlockRt` 後 | `NC_FEED_CTRL_IDLE`、generation増加 | 副作用 |
| UT-V7-CYCLE-001 | `NcCycle_BuildSegmentPlanRt` | G82ドウェル付きサイクル | P指定あり | rapid/feed/dwell/retract 4セグメント | 分岐 |
| UT-V7-CYCLE-002 | `NcCycle_BuildSegmentPlanRt` | G83深穴ペック | Q指定あり | 複数peck/retract、peck_index更新 | 分岐・境界 |
| UT-V7-CYCLE-003 | `NcCycle_ApplyCycleGCode` | G88手動戻りボーリング | G88 | manual_return_blocks更新 | 状態副作用 |
| UT-V7-CAP-001 | `NcCapability_OnParsedBlockTs` | G履歴リングラップ | 履歴サイズ+3件 | index更新、最新Gを保持 | 配列境界 |
| UT-V7-CAP-002 | `NcCapability_OnParsedBlockTs` | M履歴リングラップ | 履歴サイズ+2件 | index更新、最新Mを保持 | 配列境界 |
| UT-V7-CAP-003 | `NcCapability_RecordUnsupported` | 未知kind | kind=99 | カウンタ不変、イベントのみ登録 | 異常系 |
| UT-V7-INTF-001 | `NcInterference_CheckBlockTs` | 無効時・非モーションNo-op | AUX_ONLY / disabled | checked_blocks不変 | early return |
| UT-V7-INTF-002 | `NcInterference_CheckBlockTs` | 軸限界超過 | target > limit | warning、override低下、イベント登録 | 異常系 |
| UT-V7-INTF-003 | `NcInterference_CheckBlockTs` | 円弧半径警告 | 小半径arc | path warning、override低下 | 境界値 |
| UT-V7-SPDL-001 | `NcSpindle_SetSpeedLimit` | 速度制限範囲検証 | max=0/min>max/正常 | 不正値reject、正常値反映 | 異常系 |
| UT-V7-SPDL-002 | `NcSpindle_OnBlockRt` | CSS推定回転数クランプ | G96 S指令、X最小径 | max_rpmへクランプ、出力更新 | 境界値 |
| UT-V7-SPDL-003 | `NcSpindle_ServiceRt` | M19完了/タイムアウト | センサ完了bit、timeout超過 | 完了/timeoutイベント登録 | 状態遷移 |

## 実行確認 v7

この実行環境では Windows `cl.exe` は実行できないため、GCCで追加5本を個別ビルド・実行し、全件PASSを確認した。結果は `build/test_results.txt` に追記した。

```text
Existing 41 tests + added 5 tests = 46 tests
Additional v7 PASS
```

---

# 追加単体テスト v8

## 追加対象関数分析

| 関数名 | 役割 | 入力 | 出力 | 依存するグローバル変数 | 外部依存 |
| --- | --- | --- | --- | --- | --- |
| `NcCompensation_ApplyBlockTs` | 座標回転、極座標、工具長、工具径補正をTS側で固定ブロックへ反映 | `NC_EXEC_BLOCK*`, `NC_ERROR_CODE*` | 補正後ブロック、補正状態 | `g_ncCompensationStatus` | なし |
| `NcInterp_PrepareArcTs` | R指定/IJK指定の円弧中心・半径・経路長を計算 | `NC_EXEC_BLOCK*`, `double*`, `NC_ERROR_CODE*` | 円弧幾何、経路長、エラー | なし | `math.h` |
| `NcKinematics_ApplyBlockTs` | 軸取り外し、ミラー、傾斜面の有効/解除をTS側で反映 | `NC_EXEC_BLOCK*`, `NC_ERROR_CODE*` | 変換後ブロック、キネマ状態 | `g_ncKinematicsStatus` | `math.h` |
| `NcSynchronization_ApplyBlockRt` | 同期/複合/重ね合わせ/双テーブル制御をRTブロックへ反映 | `NC_EXEC_BLOCK*` | ブロック更新、診断カウンタ | `g_ncSynchronizationStatus`, `g_ncEventRing` | なし |
| `NcLatheCycle_OnParsedBlockTs` / `NcLatheCycle_OnBlockRt` | 旋削、研削、ねじ切りサイクルの計画/実行診断を更新 | `NC_EXEC_BLOCK*` | サイクル状態、イベント | `g_ncTurningCycleStatus`, `g_ncThreadCycleStatus`, `g_ncEventRing` | なし |

## 追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
| --- | --- | --- | --- | --- | --- |
| UTC-V8-001 | `NcCompensation_ApplyBlockTs` | 座標回転90度とキャンセル | G68相当、中心0、X=1000/Y=0 | X≈0/Y≈1000、キャンセルで無効 | 正常/状態遷移 |
| UTC-V8-002 | `NcCompensation_ApplyBlockTs` | 極座標変換 | G16相当、半径1000、角度90度、中心あり | 中心補正込みでXYへ変換 | 正常/境界 |
| UTC-V8-003 | `NcCompensation_ApplyBlockTs` | 負方向工具長と右工具径補正 | G44 + G42相当 | Z減算、右側法線へ補正 | 正常/副作用 |
| UTC-V8-004 | `NcInterp_PrepareArcTs` | R指定で半径不足 | chord > 2R | `NC_ERR_ARC_GEOMETRY` | 異常 |
| UTC-V8-005 | `NcInterp_PrepareArcTs` | R指定の円弧中心・長さ | R=10000、XY平面 | 半径・中心・長さ計算 | 正常/境界 |
| UTC-V8-006 | `NcInterp_PrepareArcTs` | 同一点IJK円弧とZ差分 | XY同一点、Zのみ差分 | Z差分だけ経路長へ反映 | 境界 |
| UTC-V8-007 | `NcKinematics_ApplyBlockTs` | 取り外し軸の除外 | Y軸detach | Y目標が開始位置へ戻り、軸mask除外 | 副作用 |
| UTC-V8-008 | `NcKinematics_ApplyBlockTs` | ミラー有効後キャンセル | mirror enable/cancel | `mirror_axis_mask` が解除 | 状態遷移 |
| UTC-V8-009 | `NcKinematics_ApplyBlockTs` | 傾斜面有効後キャンセル | tilt enable/cancel | active=0、角度=0 | 状態遷移 |
| UTC-V8-010 | `NcSynchronization_ApplyBlockRt` | 同期モードキャンセル | G51.4→G50.4 | cancel counter増加、mode bit解除 | 状態遷移 |
| UTC-V8-011 | `NcSynchronization_ApplyBlockRt` | 双テーブルで同期補完 | sync Gコードなし、double table有効 | slave軸がmaster deltaで更新 | 正常/副作用 |
| UTC-V8-012 | `NcSynchronization_ApplyBlockRt` | 追従誤差警告 | slave目標がmaster deltaから閾値超過 | warning counterとイベント更新 | 異常/境界 |
| UTC-V8-013 | `NcLatheCycle_OnParsedBlockTs` | 仕上げ/溝/端面サイクル | G70/G74/G75/G94 | 各カウンタ更新 | 正常 |
| UTC-V8-014 | `NcLatheCycle_OnParsedBlockTs` | 円弧ねじ/可変リード | G32 + IJK + Q | 円弧ねじ/可変リードを記録 | 正常/分岐 |
| UTC-V8-015 | `NcLatheCycle_OnBlockRt` | Gコードなしthread flag | `NC_FEATURE_FLAG_THREAD` | ねじ切り実行イベント登録 | 境界/副作用 |

## 追加テストコード

- `tests/test_nc_compensation_rotation_polar_edges.c`
- `tests/test_nc_interp_math_arc_r_edges.c`
- `tests/test_nc_kinematics_detach_tilt_cancel.c`
- `tests/test_nc_synchronization_cancel_double_table.c`
- `tests/test_nc_lathe_cycle_variants.c`

## 追加カバレッジ上の注意点

- 円弧補間の数学検証は、浮動小数演算と固定小数丸めの許容差を含めている。実機用には象限別、CW/CCW別、R正負別の追加が必要。
- キネマティクスはサンプル実装の状態更新確認であり、5軸機の実幾何精度は結合/機能テストで確認する。
- 同期制御の追従誤差は共有状態とイベント登録を確認している。実サーボ周期での追従性はHIL/実機試験対象。
- 旋削/研削/ねじ切りサイクルは診断カウンタ中心で、工具経路の実幾何までは本単体テストの対象外。

## 追加仕様不明点

- G74/G75が研削サイクル範囲にも該当する場合の二重カウントを正式仕様とするか、旋削系と研削系をモードで排他するか。
- 同一点IJK円弧をフル円として扱うか、今回の実装同様に平面外差分のみの経路長として扱うか。
- 極座標モード時の角度単位と中心点の扱いを、機種別仕様として明文化する必要がある。

## 追加単体テスト v9/v10

前回までのテスト群に対し、以下の10ファイルを追加した。

| 追加テストファイル | 主な対象 | 主な観点 |
|---|---|---|
| `test_control_api_remaining_setters.c` | 主軸、工具管理、旋削直径モード、同期、ロータリMCC、干渉API | setter戻り値、getter NULL、共有状態コピー |
| `test_nc_program_request_edges.c` | バイナリNCロード要求 | NULL/0/最大超過、要求フラグ、固定長ブロックコピー |
| `test_rt_control_dispatch_edges.c` | RTディスパッチ | 周期カウンタ、RT時間計測、ALARM時安全出力、I/Oトレースfreeze |
| `test_ts_service_log_and_slice_edges.c` | TSサービス | prefetch、UI snapshot、log flush要求クリア、ログキュー副作用 |
| `test_nc_motion_filter_multiaxis_accel_edges.c` | モーションフィルタ | 複数軸mask、加速度/速度制限、未mask軸No-op、終点強制補正 |
| `test_nc_coordinate_program_machine_edges.c` | 座標管理 | 絶対/増分、ワーク+ローカルオフセット、機械座標逆算、一時絶対座標 |
| `test_nc_buffer_generation_validity_edges.c` | NCリングバッファ | NULL/空、invalid head、head/tail wrap、producer/consumer generation |
| `test_nc_axis_config_apply_edges.c` | 軸構成TS適用 | path軸フィルタ、軸取り外し、直径モード変換、非motion no-op |
| `test_nc_coordinate_transform_apply_edges.c` | 座標変換TS適用 | 動的治具/取付誤差/ロータリ補正のON/OFF、円弧中心補正、NULL |
| `test_system_shared_event_log_more_edges.c` | 共有ログ/イベントリング | write_index異常復旧、cycle/state記録、ログ満杯後のhead進行 |

### 追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
|---|---|---|---|---|---|
| V9-API-001 | `Api_SetNcSpindleSpeedLimit` 等 | 未網羅API setter/getter | 正常値、逆転範囲、NULL getter | 状態更新、異常戻り値、NULL拒否 | 正常/異常 |
| V9-NCP-001 | `NcProgram_RequestBinaryLoad` | バイナリロード要求境界 | NULL、0件、最大超過、2件 | 異常拒否、要求フラグとブロックコピー | 異常/境界 |
| V9-RT-001 | `RtDispatcher_ExecuteCycle` | RT周期処理副作用 | READY/ALARM状態 | cycle/tick更新、安全出力、trace freeze | 状態遷移/安全 |
| V9-TS-001 | `TsService_ExecutePrefetch` 等 | TSサービス副作用 | prefetch要求、log flush要求 | ready/generation更新、ログ登録 | 正常/副作用 |
| V9-MF-001 | `NcMotionFilter_ApplyRt` | 複数軸・加速度制限 | axis mask、負値limit、endpoint | 制限イベント、未mask no-op、終点一致 | 境界/副作用 |
| V10-COORD-001 | `NcCoordinate_SetProgramAxis` 等 | 座標系直接操作 | G54相当、local shift、incremental | machine/program位置更新 | 正常/境界 |
| V10-BUF-001 | `NcBuffer_CommitBlock` 等 | リング世代・invalid slot | wrap境界、valid=0 | generation更新、peek拒否、consume安全 | 境界/異常 |
| V10-AXIS-001 | `NcAxisConfig_ApplyBlockTs` | 軸構成適用 | path mask、detached、diameter | 軸mask/target変換、イベント記録 | 分岐/副作用 |
| V10-XFORM-001 | `NcCoordinateTransform_ApplyBlockTs` | 座標補正ON/OFF | dynamic ON、mount OFF、rotary ON | 対象補正のみ加算、arc center補正 | 分岐/正常 |
| V10-SHARED-001 | `NcEvent_Push` / `LogQueue_Push` | リング境界 | write_index範囲外、満杯後head進行 | index復旧、lostなし、再push成功 | 境界/副作用 |

### カバレッジ上の補足

今回の追加により、公開APIの薄いgetter/setter、RT/TS周期入口、座標・軸構成・リングバッファの境界分岐を追加で確認した。なお、実機依存のモーションコントローラ応答、PLC MFIN信号タイミング、ファイルI/O失敗詳細、実時間usec計測は単体テストではスタブ/状態確認に留め、結合テストまたは機能テストで確認する。

### 実行確認メモ

追加ファイルはGCC `-fsyntax-only -Wall -Wextra` による構文確認を実施した。個別実行できた追加テストについては `build/test_results.txt` に結果を追記した。全61本の一括実行は実行環境の時間制限により未完了のため、Windows環境では `scripts\\run_tests.bat` により再実行すること。

---

## 追加単体テスト v11

前回までの61本に加え、以下5本を追加し、合計66本の単体テスト構成へ拡張した。

| 追加テストファイル | 主な対象関数 | 追加観点 |
|---|---|---|
| `tests/test_nc_feed_per_rev_and_error_edges.c` | `NcFeed_PrepareMotionTs`, `NcFeed_PrepareDwellTs`, `NcFeed_BeginBlockRt` | 回転送り時の主軸回転数0、NULL、負経路長、ドウェル0/max超過、override 0 HOLD |
| `tests/test_nc_compensation_offset_cancel_edges.c` | `NcCompensation_SetToolLengthOffset`, `NcCompensation_SetCutterRadiusOffset`, `NcCompensation_ApplyBlockTs` | オフセット番号0/上限、未登録H/D、G49/G40相当キャンセル |
| `tests/test_nc_tool_management_duplicate_and_mfin_edges.c` | `NcToolManagement_SetPocket`, `NcToolManagement_RequestPrepare`, `NcToolManagement_OnBlockRt`, `NcToolManagement_ServiceRt` | ポケット重複、未割当工具、MFIN待ち完了、イベントリング副作用 |
| `tests/test_nc_coordinate_selection_clear_edges.c` | `NcCoordinate_SelectWork`, `NcCoordinate_SetUnitMode`, `NcCoordinate_SetDistanceMode`, `NcCoordinate_SetProgramAxis`, `NcCoordinate_FillTargets` | ワーク座標範囲外、ワーク切替時のlocal/temporary clear、増分座標、NULL no-op |
| `tests/test_nc_codes_decimal_and_address_edges.c` | `NcCodes_GetGCodeInfo`, `NcCodes_GetAddressInfo`, `NcCodes_GetMCodeInfo` | 小数Gコード、U/V/W小文字正規化、未対応アドレス、M999/M1000境界 |

### v11確認結果

追加5本はGCCで個別ビルド・実行し、全件PASSを確認した。結果は `build/test_results.txt` に追記済み。

補足: サンドボックスでは全66本一括実行が時間制限に達したため、Windows環境では `scripts\\run_tests.bat` を再実行して全件の結果を再生成すること。

---

## 追加単体テスト v12

前回までの66本に加え、以下5本を追加し、合計71本の単体テスト構成へ拡張した。

| 追加テストファイル | 主な対象関数 | 追加観点 |
|---|---|---|
| `tests/test_nc_capability_null_and_address_more.c` | `NcCapability_OnParsedBlockTs`, `NcCapability_RecordUnsupported` | NULL no-op、A/X/Y/Z/F/M/S/T/P/Q/R/H/D/Kのアドレスマスク、未定義kindイベント副作用 |
| `tests/test_nc_rotary_mcc_invalid_and_noop_edges.c` | `NcRotaryMcc_SetAxisRadius`, `NcRotaryMcc_SetMccOutput`, `NcRotaryMcc_ApplyBlockTs` | NULL引数、半径最小/最大境界、MCC出力bitクリア、G13.1キャンセル |
| `tests/test_nc_parser_public_line_more_edges.c` | `NcParse_LineToBlock` | `%`行、コメント行、セミコロン以降無視、数値欠落TOKENエラー、小文字アドレス正規化 |
| `tests/test_nc_interpolation_begin_cancel_more_edges.c` | `NcInterpolation_PrepareBlockTs`, `NcInterpolation_BeginBlockRt`, `NcInterpolation_Cancel`, `NcInterpolation_StepRt` | NULL/非active拒否、cancel副作用、DWELL時のサーボ指令No-op |
| `tests/test_system_shared_defaults_reinit_edges.c` | `SystemShared_Initialize`, `NcEvent_Push`, `LogQueue_Push` | 初期値復元、イベントcycle/state記録、ログ/イベント副作用の再初期化クリア |

### v12追加テストケース一覧

| TC-ID | 対象関数 | テスト内容 | 入力条件 | 期待結果 | 種別 |
|---|---|---|---|---|---|
| V12-CAP-001 | `NcCapability_OnParsedBlockTs` | NULLブロックno-op | `pBlock=NULL` | parsed/generation/eventに副作用なし | 異常/副作用 |
| V12-CAP-002 | `NcCapability_OnParsedBlockTs` | アドレスマスク集計 | 軸、補助、P/Q/R、H/D、Kあり | address maskと履歴が更新 | 正常/副作用 |
| V12-CAP-003 | `NcCapability_RecordUnsupported` | 未定義kind | kind=99 | unsupported件数は増えずイベントのみ記録 | 異常/分岐 |
| V12-MCC-001 | `NcRotaryMcc_ApplyBlockTs` | NULL引数拒否 | block/error NULL | -1返却、状態破壊なし | 異常 |
| V12-MCC-002 | `NcRotaryMcc_SetAxisRadius` | 半径境界 | axis範囲外、min未満、max超過、min一致 | 異常拒否、min一致は受理 | 境界 |
| V12-MCC-003 | `NcRotaryMcc_SetMccOutput` / `ApplyBlockTs` | 出力OFFとG13.1 | command bit ON後にOFF、G13.1 | 出力bitクリア、mode disabled、イベント記録 | 正常/副作用 |
| V12-PARSE-001 | `NcParse_LineToBlock` | `%`/コメント行 | `%`、`(comment)` | no-op戻り値1、error none | 正常/分岐 |
| V12-PARSE-002 | `NcParse_LineToBlock` | セミコロン以降無視 | `G01 X... F... ; Y...` | Yはaxis_maskへ反映されない | 正常/分岐 |
| V12-PARSE-003 | `NcParse_LineToBlock` | 数値欠落 | `G X1`, `G01 X` | `NC_ERR_TOKEN` | 異常 |
| V12-PARSE-004 | `NcParse_LineToBlock` | 小文字アドレス | `g01 x... f...` | motion/feed/axis mask反映 | 正常/境界 |
| V12-INTP-001 | `NcInterpolation_PrepareBlockTs` / `BeginBlockRt` | NULL/非active拒否 | NULL、`NC_MOTION_NONE` | -1、activeなし | 異常/early return |
| V12-INTP-002 | `NcInterpolation_Cancel` | cancel副作用 | linear block開始後cancel | active解除、state IDLE、cycle inactive | 状態遷移 |
| V12-INTP-003 | `NcInterpolation_StepRt` | DWELL時No servo | dwell 2tick | command_bitsへservo/startを立てない | RT安全/副作用 |
| V12-SHARED-001 | `SystemShared_Initialize` | 安全初期値 | 初期化直後 | MANUAL/STOPPED/NC_IDLE等 | 正常 |
| V12-SHARED-002 | `NcEvent_Push` | cycle/state記録 | cycle/state設定後push | event itemへcycle/state/value反映 | 副作用 |
| V12-SHARED-003 | `SystemShared_Initialize` | 再初期化で副作用クリア | ログ/イベント/テーブル改変後 | queue/event/default table復元 | 境界/副作用 |

### v12確認結果

追加5本はGCCで個別ビルド・実行し、全件PASSを確認した。結果は `build/test_results.txt` に追記済み。

補足: サンドボックスでは全71本一括実行が時間制限に達しやすいため、Windows環境では `scripts\\run_tests.bat` を再実行して全件の結果を再生成すること。

---

# 追加単体テスト v13（20本）

前回までの71本に対して、以下の20本を追加した。既存 `src/` / `inc/` は変更せず、`tests/` 配下のテスト追加と `scripts/build_tests.*` / `scripts/run_tests.*` の対象追加のみを行った。

| No | 追加テストファイル | 主な対象 | 観点 |
|---:|---|---|---|
| 1 | `test_nc_axis_config_definition_more.c` | `NcAxisConfig_SetAxisDefinition` | 小文字軸名正規化、不正axisType拒否 |
| 2 | `test_nc_axis_config_apply_path_filter_more.c` | `NcAxisConfig_ApplyBlockTs` | path軸外除去、NULL引数 |
| 3 | `test_nc_axis_config_diameter_rounding_more.c` | `NcAxisConfig_ApplyBlockTs` | 直径モードの正負丸め境界 |
| 4 | `test_nc_coordinate_work_offset_more.c` | `NcCoordinate_SetWorkOffsetAxis` / `NcCoordinate_SetProgramAxis` | ワークオフセット反映、範囲外workIndex |
| 5 | `test_nc_coordinate_machine_inverse_more.c` | `NcCoordinate_SetMachineAxis` | 機械座標指定からプログラム座標逆算 |
| 6 | `test_nc_coordinate_fill_targets_more.c` | `NcCoordinate_FillTargets` | 未指定軸補完、NULL no-op |
| 7 | `test_nc_coordinate_transform_enable_more.c` | `NcCoordinateTransform_SetEnabled` | 有効フラグ切替、範囲外axis拒否 |
| 8 | `test_nc_coordinate_transform_apply_combo_more.c` | `NcCoordinateTransform_ApplyBlockTs` | 動的治具・取付誤差・ロータリ補正の合算 |
| 9 | `test_nc_cycle_modal_replay_more.c` | `NcCycle_FinalizeParserBlock` / `NcCycle_InitParserBlock` | 固定サイクルmodal replay |
| 10 | `test_nc_cycle_segment_guards_more.c` | `NcCycle_BuildSegmentPlanRt` / `NcCycle_SetRepeatWord` | NULL拒否、K/repeat丸め |
| 11 | `test_nc_feed_prepare_motion_zero_override_more.c` | `NcFeed_PrepareMotionTs` | override 0時の最小tick・HOLD相当 |
| 12 | `test_nc_feed_prepare_dwell_bounds_more.c` | `NcFeed_PrepareDwellTs` | dwell 0、最大超過 |
| 13 | `test_nc_interference_check_axis_limit_more.c` | `NcInterference_CheckBlockTs` | 軸限界警告、減速副作用 |
| 14 | `test_nc_lookahead_null_and_g08_more.c` | `NcLookahead_ApplyBlockTs` | NULL no-op、G08指令反映 |
| 15 | `test_nc_precision_setters_valid_invalid_more.c` | `NcPrecision_Set*` | learning/vibration/preview境界 |
| 16 | `test_nc_spindle_css_stop_more.c` | `NcSpindle_OnParsedBlockTs` / `NcSpindle_OnBlockRt` | CSS有効化、M05停止 |
| 17 | `test_nc_tool_management_prepare_invalid_more.c` | `NcToolManagement_RequestPrepare` | 不正工具番号拒否、工具準備成功 |
| 18 | `test_nc_synchronization_overlay_bounds_more.c` | `NcSynchronization_SetOverlayAxis` | overlay軸範囲外、mask/offset設定 |
| 19 | `test_nc_reference_one_direction_more.c` | `NcReference_SetOneDirectionApproach` | 負値拒否、接近量設定 |
| 20 | `test_nc_rotary_mcc_output_service_more.c` | `NcRotaryMcc_ServiceRt` | 機械状態によるMCC出力ゲート、OFF時クリア |

## v13確認結果

この実行環境ではWindows `cl.exe` は実行できないため、追加20本についてGCCで個別ビルド・実行確認を行った。結果は `build/test_results_v13_added.txt` および `build/test_results.txt` に追記済み。全20本PASS。

## v13のカバレッジ上の注意点

今回追加した20本は、API/直接関数の戻り値、共有状態の更新、NULL/範囲外、境界値、副作用を中心に確認している。複数モジュール連携後の時系列整合性、実機I/O応答、リアルタイム周期のワーストケース時間は、引き続き結合テスト・機能テストで確認する。


## v16 追加単体テスト

- 追加: 10ファイル / 40ケース
- 結果: GCCで追加分40ケースPASS
- 詳細: `docs/unit_test_report_v16.md`、`docs/manifest_v16_unit_tests.json`、`build/test_results_v16_added.txt`

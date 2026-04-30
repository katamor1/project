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

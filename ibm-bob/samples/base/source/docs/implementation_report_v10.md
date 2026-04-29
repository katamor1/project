# IBM-Bob source implementation report v10

## 目的
アップロードされた基本設計書・詳細設計書群のうち、既存 v9 実装で不足していた横断機能を追加実装した。
RT/TS/API 分離、固定長共有メモリ、RT 側でのファイル I/O・動的メモリ禁止を維持した。

## 追加実装

### 1. 高速バイナリプログラム運転の入口実装
- `NC_BINARY_PROGRAM_REQUEST` / `NC_BINARY_PROGRAM_STATUS` を共有メモリに追加。
- `Api_RequestNcBinaryProgramLoad()` を追加し、固定長 `NC_EXEC_BLOCK` 配列を TS 側で NC 実行リングに投入可能にした。
- 最大ブロック数は `NC_BINARY_PROGRAM_MAX_BLOCKS` とし、RT 側は従来どおり固定ブロックのみを消費する。

### 2. Gコード/Mコード/同一ブロックアドレス解釈の状態公開
- `nc_capability.h/c` を追加。
- `NC_CAPABILITY_STATUS` を追加し、解析済み G/M/アドレス語の使用状況、履歴、未対応コード検出を公開。
- `Api_GetNcCapabilityStatus()` を追加。

### 3. 経路間干渉チェック・3D干渉チェックのTS側スクリーニング
- `nc_interference.h/c` を追加。
- TS 側固定ブロック化後に、軸可動域・簡易クリアランス・小半径円弧を判定。
- 警告時はイベントリングに `NC_EVENT_CODE_INTERFERENCE_WARN` を記録し、対象ブロックのオーバライドを安全側へ制限。
- `Api_SetNcInterferenceCheckEnabled()` / `Api_GetNcInterferenceStatus()` を追加。

### 4. 外部減速・停電時減速停止・位置スイッチ・絶対位置検出状態
- `nc_safety_motion.h/c` を追加。
- RT周期で外部減速、停電時減速停止、位置スイッチラッチ、絶対基準有効状態を監視。
- 実行ブロック開始時に安全モーションオーバレイを適用し、フィードオーバライドと減速 tick を安全側へ補正。
- `Api_GetNcSafetyMotionStatus()` を追加。

### 5. ビルド対象更新
- `build_vc2026.bat` / `build_clang.bat` に追加モジュールを反映。
- GCC スモーク確認では v10 の追加状態行を出力対象に追加。

## 変更ファイル
- `inc/system_config.h`
- `inc/system_shared.h`
- `inc/control_api.h`
- `inc/nc_program.h`
- `inc/nc_capability.h`
- `inc/nc_interference.h`
- `inc/nc_safety_motion.h`
- `src/system_shared.c`
- `src/control_api.c`
- `src/nc_program.c`
- `src/nc_parser_modal.c`
- `src/nc_parser_tokens.c`
- `src/rt_control.c`
- `src/main.c`
- `src/nc_capability.c`
- `src/nc_interference.c`
- `src/nc_safety_motion.c`
- `scripts/build_vc2026.bat`
- `scripts/build_clang.bat`

## 確認結果
`gcc -O0 -std=c99 -Wall -Wextra -Iinc src/*.c -lm` 相当でビルド確認し、サンプル実行が正常終了することを確認した。
出力詳細は `docs/verification_output_v10.txt` を参照。

## 制限事項
- バイナリプログラムは固定ブロック投入の入口実装であり、外部バイナリファイル形式の永続仕様・CRC・バージョン互換は未実装。
- 干渉チェックはTS側スクリーニングであり、完全な工具形状/治具/ワーク3Dモデル干渉計算ではない。
- 停電時減速停止は安全オーバレイと状態公開までで、実機電源監視回路やサーボドライブ側停止カテゴリとの整合は別途詳細化が必要。

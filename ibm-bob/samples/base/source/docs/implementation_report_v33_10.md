# IBM-Bob ソースコード追加実装レポート v33_10

## 概要

`ibm_bob_source_implemented_additional_features_v32_10` をベースに、未実装/入口実装止まりだった機能を10件追加した。

- 対象モジュール: `src/nc_feature_backlog.c`, `inc/system_config.h`
- 追加ID: `250`〜`259`
- 実装方式: TS側の固定長 `NC_EXEC_BLOCK` 補正 + RT側の状態更新/イベントカウント
- RT制約: 動的メモリ確保、ファイルI/O、Windows API呼び出しなし

## 追加した10機能

| ID | 機能名 | 実装概要 |
|---:|---|---|
| 250 | 主軸ベルト滑り予兆監視 | 主軸速度未設定時の補完、送り減速、減速tick追加、予兆モーダルラッチ |
| 251 | クーラントフィルタ目詰まりHOLD | MFIN待ち、軸退避フラグ、送り減速、長めの減速tick付与 |
| 252 | エアシリンダ到達端確認Mコード監視 | Mコード/MFIN待ち、タイムアウト用P word補完、監視ラッチ |
| 253 | ワーク洗浄ノズル位置インターロック | Mコード待ち、XY可動範囲クランプ、軸割付フラグ、送り減速 |
| 254 | ロボット搬入出エリア干渉待ち | Mコード待ち、干渉待ちP word設定、軸退避フラグ、送り減速 |
| 255 | チャック把握力段階監視 | 主軸速度補完、MFIN待ち、送り減速、減速tick追加 |
| 256 | センタースルークーラント圧上昇待ち | Mコード/MFIN待ち、圧上昇待ちP word補完、平滑化フラグ |
| 257 | 切粉堆積センサ安全減速 | MFIN待ち、軸退避フラグ、送り減速、減速tick追加 |
| 258 | 機械照明/扉ロック保守モードゲート | Mコード/MFIN待ち、保守時の送り上限クランプ、軸割付フラグ |
| 259 | UPS低残量安全停止シーケンス | MFIN待ち、軸退避フラグ、送り最小化、停止用減速tick追加 |

## 変更点

- `NC_IMPL_BACKLOG_FEATURE_COUNT`: `250U` → `260U`
- `NC_IMPL_BACKLOG_WORD_COUNT`: `8U` → `9U`
- `s_backlog[]` にID `250`〜`259` の10件を追加
- `ApplyV23Policy()` にID `250`〜`259` の固定長ブロック補正caseを追加
- GCCスモークビルド/テスト用に `build_gcc_smoke_v33_10.sh` / `test_gcc_smoke_v33_10.sh` を追加

## 検証

この環境では GCC smoke build/test を実行し、正常終了を確認した。
詳細は `verification_output_v33_10.txt` を参照。

## Windows側確認

Windows / Visual Studio / clang 環境では、ZIP展開後に以下を実行する。

```bat
cd source
scripts\build_vc2026.bat
scripts\test_vc2026.bat
scripts\build_clang.bat
scripts\test_clang.bat
```

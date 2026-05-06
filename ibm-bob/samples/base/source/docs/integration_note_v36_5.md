<!-- ibm-bob/samples/base/source/docs/integration_note_v36_5.md -->
<!-- Archives the pre-normalization v36_5 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_lube_brake_thermal.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_lube_brake_thermal.c -->
# v36_5 統合メモ

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_lube_brake_thermal.h`, `src/nc_feature_backlog_lube_brake_thermal.c`, and `tests/test_nc_feature_backlog_lube_brake_thermal.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 目的

v35_10の次段として、ID `280-284` の5機能を追加する。

## 追加ID

- 280: 潤滑圧低下段階HOLD
- 281: 軸ブレーキ解除フィードバック不一致ガード
- 282: チャッククランプ圧確認
- 283: パレットクランプ/アンクランプタイムアウト
- 284: 熱変位補正READYゲート

## 統合ポイント

1. `source/inc/nc_feature_backlog_v36.h` をinclude配下に追加する。
2. `source/src/nc_feature_backlog_v36.c` をビルド対象に追加する。
3. 既存の統合backlog定義を持つ場合は、総数を `285` にする。
4. word countは `ceil(285/32)=9` のため変更不要。
5. Windows確認は `scripts\test_vc2026_v36_5.bat` または `scripts\test_clang_v36_5.bat` を使用する。

## RT/TS分担

- TS: 固定長ブロックにHOLD、インターロック、ゲート、診断、熱補正オフセットを反映。
- RT: センサ不一致、圧力不足、READY未成立、温度差閾値超過を周期監視。

## 注意

本ZIPはv35_10 ZIPをベースに、v36_5追加モジュールを同梱したもの。既存プロジェクトに取り込む場合は、既存ビルドスクリプトのソース一覧にも `src\nc_feature_backlog_v36.c` を追加すること。

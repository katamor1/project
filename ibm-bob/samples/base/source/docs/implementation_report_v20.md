# IBM-Bob 追加機能実装レポート v20

## 概要

v19 ソースをベースに、詳細設計書・基本設計書で未実装または状態公開に留まっていた高度補間/特殊サイクル系の機能を 10 件追加した。

実装方針は、既存の IBM-Bob 基盤制約に合わせて以下とした。

- RT_CONTROL には重い幾何計算・動的メモリ確保・ファイル I/O を追加しない。
- TS_SERVICE 側で固定長 `NC_EXEC_BLOCK` の補正・状態確定・速度抑制を行う。
- RT_CONTROL 側は実行カウンタとイベントリング登録を行う。
- 追加状態は固定長共有メモリ `NC_DESIGN_FEATURE_STATUS` と API で公開する。

## 追加実装した10機能

1. NURBS補間 G06.2 の固定長高度補間ブロック化
2. 指数補間 G02.3/G03.3 の高度補間ブロック化と速度抑制
3. 円筒補間/回転軸仮想送り G07.1/G12.1/G13.1 の状態管理と軸変換補助
4. 極座標モード G16/G15 の状態管理と極座標中モーションの速度抑制
5. 円弧螺旋補間B 相当の3D/回転軸付き円弧検出
6. 自動コーナオーバライド G62/G64 の状態管理と後続モーション速度抑制
7. 研削専用サイクル G70.7〜G76.7/G77〜G79 の検出と速度抑制
8. EGB軸用スキップ G31 + 回転軸(A軸) のアーム状態管理
9. 3D干渉監視対象ブロックの検出と安全側速度抑制
10. レファレンス点復帰時のフォローアップ/ロールオーバー検出入口

## 追加・変更ファイル

- `inc/nc_design_features.h`
- `src/nc_design_features.c`
- `inc/system_config.h`
- `inc/system_shared.h`
- `src/system_shared.c`
- `inc/control_api.h`
- `src/control_api.c`
- `src/nc_parser_modal.c`
- `src/nc_program.c`
- `src/main.c`
- build/test script 群

## 追加API

- `Api_GetNcDesignFeatureStatus()`
- `Api_SetNcDesignAutoCornerOverride()`

## 検証

GCC smoke build/test を実行し、`nc_state=6` / `nc_error=0` を確認した。

代表出力:

```text
design_features mode=0x000001ff nurbs=1 exp=1 cyl=3 polar=3 helical_b=1 auto_corner=2 grinding=10 egb_skip=1 interference3d=23 followup=0 ts=33 rt=32 slowdown=36 override=70
```

補足: フォローアップ/ロールオーバー検出は実装済みだが、長時間実行化を避けるため smoke シナリオでは意図的に発火させていない。

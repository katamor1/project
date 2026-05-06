# IBM-Bob source implementation report v23

## Scope

v22をベースに、設計書ZIP側でまだ「入口実装」または「状態公開中心」に留まっていたNC制御機能を20件追加実装した。既存の `nc_feature_backlog` を拡張し、機能ID `100`〜`119` をv23追加分として割り当てた。

v22までの100件は後方互換のため観測中心のまま維持し、v23追加分だけ `NcFeatureBacklog_ApplyBlockTs()` 内で固定長のTS側ブロック補正を実施する。RT側では従来通り実行カウントとイベント記録のみを行い、Windows API、ファイルI/O、動的メモリ確保、可変長探索は追加していない。

## Implemented 20 items

1. **G70仕上げサイクル実行パス整合**
   - 仕上げサイクルの最小パス数、cycle flag、最小補間tick、仕上げ送り抑制を追加。

2. **G71粗削りサイクル切込み分割**
   - `Q` 未指定時の固定切込み量補完、X移動量からのパス数上限付き算出、粗削り送り抑制を追加。

3. **G72端面粗削りサイクル逃げ量補正**
   - Z軸逃げ量を固定長ブロックへ反映し、RT側では補正済み終点だけを消費する方式にした。

4. **G73パターン繰返し残回数管理**
   - repeat未指定時の残回数補完、Pワード補完、最小補間tickを追加。

5. **G75溝入れペック分割**
   - `Q` 未指定時のペック量補完、ドウェルP補完、溝入れcycle flagを追加。

6. **G76複数パスねじ切り安全減速**
   - thread motion化、feed/rev化、主軸回転数フォールバック、複数パス数補完、安全減速を追加。

7. **G90直線テーパ終点補正**
   - Z移動量からX終点を補正し、テーパ切削用の固定ブロック終点を作る処理を追加。

8. **G92ねじ切りピッチ保持**
   - thread motion、feed/rev、送り量/主軸回転数の安全側補完を追加。

9. **G94端面切削退避面補正**
   - R値未指定時の退避量補完とZ終点補正を追加。

10. **G31高速スキップラッチ**
    - skip motion化、MFIN待ちフラグ、送り抑制、最小tickを追加。

11. **EGB同期スキップ位相窓**
    - Pワードを位相窓として補完し、smoothing flagと最小tickを追加。

12. **距離コード付きリファレンスマーク復帰**
    - reference return化し、距離コード相当のX目標を固定長ブロックへ反映。

13. **絶対アドレスリファレンスマーク復帰**
    - reference return化し、絶対マーク位置へのXスナップを追加。

14. **位置スイッチ制御ラッチ減速**
    - axis retract flag、安全減速、最小tickを追加。

15. **ロールオーバー最短回転補正**
    - A軸を±180度相当範囲へ正規化し、長回り指令を短回り指令に変換。

16. **フォローアップ偏差予兆減速**
    - 軸移動量が閾値を超えた場合に送りを安全側へ抑制。

17. **ロータリフィクスチャ動的オフセット**
    - XYZの固定オフセット補正と座標回転flagを固定ブロックへ反映。

18. **ワーク取付誤差固定補正**
    - XYZの小補正とtilted plane flagを固定ブロックへ反映。

19. **円筒/極座標補間送り正規化**
    - polar/coord rotation flag、送り抑制、最小tickを追加。

20. **Mコード補助指令ハンドシェイク監視**
    - Mコード/MFIN待ちflag、補助Mコード未指定時の補完、待ちPワード補完を追加。

## Changed files

- `inc/system_config.h`
  - `NC_IMPL_BACKLOG_FEATURE_COUNT` を `100U` から `120U` へ拡張。
  - v23追加範囲、追加件数、ロールオーバー正規化用定数を追加。

- `src/nc_feature_backlog.c`
  - `s_backlog[]` に機能ID `100`〜`119` の20件を追加。
  - `ApplyV23Policy()`、`SetMinTicks()`、`SlowFeed()`、`NormalizeRotaryShortestPath()` などのRT安全な固定長補正処理を追加。
  - `NcFeatureBacklog_ApplyBlockTs()` でv23追加分のみTS側固定ブロック補正を実行。

- `scripts/build_gcc_smoke_v23.sh`
  - v23用GCCスモークビルドスクリプトを追加。

- `scripts/test_gcc_smoke_v23.sh`
  - v23用GCCスモークテストスクリプトを追加。

- `docs/verification_output_v23.txt`
  - v23スモーク実行結果を保存。

- `docs/continuation_v23_diff.patch`
  - v22入力との差分パッチ。

## Verification

GCC smoke build/test を実行し、正常終了を確認した。

```text
implementation_backlog implemented=120 configured=120 ts=130 rt=126 synthetic=120 cat_mask=0x000003ff last_id=119 last_cat=4 last_policy=3 slowdown=16 cat0=30 cat9=22 last_x=1119
nc_state=6 nc_error=0
cycle_error_checks=4
```

Full output: `docs/verification_output_v23.txt`.

## Notes

- 今回の20件は、実機制御の完全な幾何演算ではなく、IBM-Bobサンプル基盤に合わせた「固定長NC実行ブロックへの安全側変換」として実装している。
- 重い幾何計算、テーブル探索、ファイルI/O、動的メモリ確保はTS/RT双方に追加していない。
- 既存公開API `Api_GetNcImplementationBacklogStatus()`、`Api_SetNcImplementationBacklogFeature()`、`Api_EnableAllNcImplementationBacklogFeatures()`、`Api_RunNcImplementationBacklogSelfCheck()` は互換維持し、feature count拡張だけでv23追加分を操作できる。

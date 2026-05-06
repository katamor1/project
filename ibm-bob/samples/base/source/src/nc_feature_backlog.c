/* ibm-bob/samples/base/source/src/nc_feature_backlog.c */
/* Implements the v22-v33 design-document backlog features as fixed-size, RT-safe handlers. */
/* This exists so TS/RT code can exercise backlog design items without dynamic allocation. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_feature_backlog.h, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_reachability.c */
#include <string.h>
#include "nc_feature_backlog.h"
#include "nc_program.h"

typedef struct {
    uint16_t feature_id;
    uint8_t category;
    uint8_t policy;
    uint16_t code10_hint;
} NC_BACKLOG_DESCRIPTOR;

static const NC_BACKLOG_DESCRIPTOR s_backlog[NC_IMPL_BACKLOG_FEATURE_COUNT] = {
    /* 000: 3D干渉チェック詳細判定 */ { 0U, 0U, 0U, 10U },
    /* 001: AI高精度輪郭制御入口 */ { 1U, 0U, 1U, 20U },
    /* 002: AI輪郭制御スムーズ補間 */ { 2U, 0U, 2U, 30U },
    /* 003: EGB軸用スキップ拡張 */ { 3U, 0U, 3U, 40U },
    /* 004: G70仕上げサイクル入口 */ { 4U, 0U, 4U, 50U },
    /* 005: G71粗削りサイクル入口 */ { 5U, 0U, 5U, 60U },
    /* 006: G72端面サイクル入口 */ { 6U, 0U, 6U, 70U },
    /* 007: G73パターン繰返し入口 */ { 7U, 0U, 7U, 80U },
    /* 008: G74ペックドリル拡張 */ { 8U, 0U, 0U, 90U },
    /* 009: G75溝入れサイクル入口 */ { 9U, 0U, 1U, 100U },
    /* 010: G76高精度ボーリング拡張 */ { 10U, 1U, 2U, 110U },
    /* 011: G77研削サイクル入口 */ { 11U, 1U, 3U, 120U },
    /* 012: G78ねじ切りサイクル拡張 */ { 12U, 1U, 4U, 130U },
    /* 013: G79研削サイクル入口 */ { 13U, 1U, 5U, 140U },
    /* 014: G81ドリル復帰制御 */ { 14U, 1U, 6U, 150U },
    /* 015: G82カウンタボア拡張 */ { 15U, 1U, 7U, 160U },
    /* 016: G83深穴排出制御 */ { 16U, 1U, 0U, 170U },
    /* 017: G84右タップ同期準備 */ { 17U, 1U, 1U, 180U },
    /* 018: G85ボーリングリーマ拡張 */ { 18U, 1U, 2U, 190U },
    /* 019: G86主軸停止ボーリング */ { 19U, 1U, 3U, 200U },
    /* 020: G87バックボーリング退避 */ { 20U, 2U, 4U, 211U },
    /* 021: G88手動戻りボーリング */ { 21U, 2U, 5U, 221U },
    /* 022: G89ドウェルボーリング */ { 22U, 2U, 6U, 231U },
    /* 023: G90直線テーパ切削 */ { 23U, 2U, 7U, 241U },
    /* 024: G92ねじ切り拡張 */ { 24U, 2U, 0U, 251U },
    /* 025: G94端面切削 */ { 25U, 2U, 1U, 261U },
    /* 026: G95回転送り監視 */ { 26U, 2U, 2U, 271U },
    /* 027: G98復帰面選択 */ { 27U, 2U, 3U, 281U },
    /* 028: G99R点復帰選択 */ { 28U, 2U, 4U, 291U },
    /* 029: Gコード一覧管理 */ { 29U, 2U, 5U, 301U },
    /* 030: 同一ブロックアドレス解釈 */ { 30U, 3U, 6U, 311U },
    /* 031: Mコード補助一覧管理 */ { 31U, 3U, 7U, 321U },
    /* 032: NCプログラム運転バッファ監視 */ { 32U, 3U, 0U, 331U },
    /* 033: NURBS制御点検証 */ { 33U, 3U, 1U, 341U },
    /* 034: スキップ入力診断 */ { 34U, 3U, 2U, 351U },
    /* 035: タッピングサイクル同期監視 */ { 35U, 3U, 3U, 361U },
    /* 036: ねじ切りサイクル共通入口 */ { 36U, 3U, 4U, 371U },
    /* 037: G32ねじ切り入口 */ { 37U, 3U, 5U, 381U },
    /* 038: フォローアップ監視 */ { 38U, 3U, 6U, 391U },
    /* 039: 距離コード付リファレンスマーク */ { 39U, 3U, 7U, 401U },
    /* 040: 絶対アドレスリファレンスマーク */ { 40U, 4U, 0U, 412U },
    /* 041: レファレンス点復帰 */ { 41U, 4U, 1U, 422U },
    /* 042: レファレンス経路補間 */ { 42U, 4U, 2U, 432U },
    /* 043: 復帰方式選択 */ { 43U, 4U, 3U, 442U },
    /* 044: ローカル座標系 */ { 44U, 4U, 4U, 452U },
    /* 045: ロータリフィクスチャ補正 */ { 45U, 4U, 5U, 462U },
    /* 046: ロールオーバー監視 */ { 46U, 4U, 6U, 472U },
    /* 047: ワーク座標系 */ { 47U, 4U, 7U, 482U },
    /* 048: ワーク取付誤差補正 */ { 48U, 4U, 0U, 492U },
    /* 049: 位置スイッチ制御 */ { 49U, 4U, 1U, 502U },
    /* 050: 一時絶対座標設定 */ { 50U, 5U, 2U, 512U },
    /* 051: 一方向位置決め */ { 51U, 5U, 3U, 522U },
    /* 052: 運転開始前インターロック集約 */ { 52U, 5U, 4U, 532U },
    /* 053: 円弧ねじ切り */ { 53U, 5U, 5U, 542U },
    /* 054: 円弧螺旋補間B */ { 54U, 5U, 6U, 552U },
    /* 055: 円筒補間 */ { 55U, 5U, 7U, 562U },
    /* 056: 仮想軸補間 */ { 56U, 5U, 0U, 572U },
    /* 057: 可変リードねじ切り */ { 57U, 5U, 1U, 582U },
    /* 058: 回転軸仮想円速度MCC */ { 58U, 5U, 2U, 592U },
    /* 059: 外部減速制御 */ { 59U, 5U, 3U, 602U },
    /* 060: 機械座標系 */ { 60U, 6U, 4U, 613U },
    /* 061: 軌跡制御 */ { 61U, 6U, 5U, 623U },
    /* 062: 極座標補間 */ { 62U, 6U, 6U, 633U },
    /* 063: 傾斜平面割出し */ { 63U, 6U, 7U, 643U },
    /* 064: 径半径指定切替 */ { 64U, 6U, 0U, 653U },
    /* 065: 経路間干渉チェック */ { 65U, 6U, 1U, 663U },
    /* 066: 研削専用サイクル共通 */ { 66U, 6U, 2U, 673U },
    /* 067: 工具管理 */ { 67U, 6U, 3U, 683U },
    /* 068: 高精度学習制御 */ { 68U, 6U, 4U, 693U },
    /* 069: 高精度振動抑制 */ { 69U, 6U, 5U, 703U },
    /* 070: 高精度輪郭制御 */ { 70U, 7U, 6U, 713U },
    /* 071: 高速サイクル加工 */ { 71U, 7U, 7U, 723U },
    /* 072: 高速サイクル重ね合わせ */ { 72U, 7U, 0U, 733U },
    /* 073: 高速スムージング */ { 73U, 7U, 1U, 743U },
    /* 074: 高速バイナリプログラム運転 */ { 74U, 7U, 2U, 753U },
    /* 075: 高速加工制御 */ { 75U, 7U, 3U, 763U },
    /* 076: 高度プレビュー制御 */ { 76U, 7U, 4U, 773U },
    /* 077: 座標回転 */ { 77U, 7U, 5U, 783U },
    /* 078: 座標値単位変換 */ { 78U, 7U, 6U, 793U },
    /* 079: 三次元円弧補間 */ { 79U, 7U, 7U, 803U },
    /* 080: 三次元工具補正正規化 */ { 80U, 8U, 0U, 814U },
    /* 081: 指数補間G02.3G03.3 */ { 81U, 8U, 1U, 824U },
    /* 082: 自動コーナオーバライド */ { 82U, 8U, 2U, 834U },
    /* 083: 自動工具長半径ベクトル */ { 83U, 8U, 3U, 844U },
    /* 084: 自由曲面高精度モード */ { 84U, 8U, 4U, 854U },
    /* 085: 軸設定 */ { 85U, 8U, 5U, 864U },
    /* 086: 軸退避復帰 */ { 86U, 8U, 6U, 874U },
    /* 087: 軸同期制御 */ { 87U, 8U, 7U, 884U },
    /* 088: 軸名称座標割付 */ { 88U, 8U, 0U, 894U },
    /* 089: 主軸機能 */ { 89U, 8U, 1U, 904U },
    /* 090: 柔軟経路軸割付 */ { 90U, 9U, 2U, 14U },
    /* 091: 重ね合わせ制御 */ { 91U, 9U, 3U, 24U },
    /* 092: 制御軸取り外しミラー */ { 92U, 9U, 4U, 34U },
    /* 093: 絶対位置検出 */ { 93U, 9U, 5U, 44U },
    /* 094: 送りモードオーバライド */ { 94U, 9U, 6U, 54U },
    /* 095: 送り機能 */ { 95U, 9U, 7U, 64U },
    /* 096: 停電時減速停止 */ { 96U, 9U, 0U, 74U },
    /* 097: 同期複合重ね合わせ */ { 97U, 9U, 1U, 84U },
    /* 098: 特殊ねじ切り */ { 98U, 9U, 2U, 94U },
    /* 099: 任意角度傾斜ピボット */ { 99U, 9U, 3U, 104U },
    /* 100: G70仕上げサイクル実行パス整合 */ { 100U, 0U, 0U, 700U },
    /* 101: G71粗削りサイクル切込み分割 */ { 101U, 0U, 1U, 710U },
    /* 102: G72端面粗削りサイクル逃げ量補正 */ { 102U, 0U, 2U, 720U },
    /* 103: G73パターン繰返し残回数管理 */ { 103U, 0U, 3U, 730U },
    /* 104: G75溝入れペック分割 */ { 104U, 1U, 4U, 750U },
    /* 105: G76複数パスねじ切り安全減速 */ { 105U, 1U, 5U, 760U },
    /* 106: G90直線テーパ終点補正 */ { 106U, 1U, 6U, 900U },
    /* 107: G92ねじ切りピッチ保持 */ { 107U, 1U, 7U, 920U },
    /* 108: G94端面切削退避面補正 */ { 108U, 2U, 0U, 940U },
    /* 109: G31高速スキップラッチ */ { 109U, 2U, 1U, 310U },
    /* 110: EGB同期スキップ位相窓 */ { 110U, 2U, 2U, 311U },
    /* 111: 距離コード付きリファレンスマーク復帰 */ { 111U, 2U, 3U, 312U },
    /* 112: 絶対アドレスリファレンスマーク復帰 */ { 112U, 3U, 4U, 313U },
    /* 113: 位置スイッチ制御ラッチ減速 */ { 113U, 3U, 5U, 314U },
    /* 114: ロールオーバー最短回転補正 */ { 114U, 3U, 6U, 315U },
    /* 115: フォローアップ偏差予兆減速 */ { 115U, 3U, 7U, 316U },
    /* 116: ロータリフィクスチャ動的オフセット */ { 116U, 4U, 0U, 317U },
    /* 117: ワーク取付誤差固定補正 */ { 117U, 4U, 1U, 318U },
    /* 118: 円筒/極座標補間送り正規化 */ { 118U, 4U, 2U, 319U },
    /* 119: Mコード補助指令ハンドシェイク監視 */ { 119U, 4U, 3U, 320U },
    /* 120: 5軸RTCP工具先端点補正 */ { 120U, 5U, 4U, 430U },
    /* 121: 傾斜作業平面B/C角正規化 */ { 121U, 5U, 5U, 431U },
    /* 122: TCPM工具中心点制御送り補正 */ { 122U, 5U, 6U, 432U },
    /* 123: 高速微小線分連続圧縮 */ { 123U, 6U, 7U, 433U },
    /* 124: 曲率連続加減速先読み */ { 124U, 6U, 0U, 434U },
    /* 125: ジャーク制限S字加減速 */ { 125U, 6U, 1U, 435U },
    /* 126: コーナR自動挿入 */ { 126U, 6U, 2U, 436U },
    /* 127: 面取り自動挿入 */ { 127U, 6U, 3U, 437U },
    /* 128: 先読みバッファ下限HOLD回避減速 */ { 128U, 7U, 4U, 438U },
    /* 129: NCシーケンスサーチ再開位置検証 */ { 129U, 7U, 5U, 439U },
    /* 130: PLC補助コードキュー二重化 */ { 130U, 7U, 6U, 440U },
    /* 131: クーラント/エアブローMコードインターロック */ { 131U, 7U, 7U, 441U },
    /* 132: 主軸定周速CSS径ゼロガード */ { 132U, 8U, 0U, 442U },
    /* 133: 主軸ギア段自動選択 */ { 133U, 8U, 1U, 443U },
    /* 134: ATC原点復帰確認 */ { 134U, 8U, 2U, 444U },
    /* 135: 工具摩耗補正微調整 */ { 135U, 8U, 3U, 445U },
    /* 136: プローブ測定結果ワーク補正反映 */ { 136U, 9U, 4U, 446U },
    /* 137: 研削ドレス量補正 */ { 137U, 9U, 5U, 447U },
    /* 138: 非常停止復帰後再開禁止ラッチ */ { 138U, 9U, 6U, 448U },
    /* 139: ドア開時安全速度監視 */ { 139U, 9U, 7U, 449U },
    /* 140: 5軸工具姿勢特異点接近減速 */ { 140U, 0U, 0U, 450U },
    /* 141: 傾斜平面退避ベクトル制限 */ { 141U, 0U, 1U, 451U },
    /* 142: NURBS制御点重み範囲監視 */ { 142U, 0U, 2U, 452U },
    /* 143: インボリュート補間基礎円半径ガード */ { 143U, 0U, 3U, 453U },
    /* 144: 三次元円弧平面法線正規化 */ { 144U, 1U, 4U, 454U },
    /* 145: 高速バイナリNCブロック整合検査 */ { 145U, 1U, 5U, 455U },
    /* 146: AI輪郭制御学習補正クランプ */ { 146U, 1U, 6U, 456U },
    /* 147: 自由曲面メッシュ境界減速 */ { 147U, 1U, 7U, 457U },
    /* 148: 高精度輪郭HPCC誤差帯減速 */ { 148U, 2U, 0U, 458U },
    /* 149: 完全停止ブロック終端同期 */ { 149U, 2U, 1U, 459U },
    /* 150: 多系統同期待ち合わせタイムアウト */ { 150U, 2U, 2U, 460U },
    /* 151: 重ね合わせ制御軸重複検出 */ { 151U, 2U, 3U, 461U },
    /* 152: 双テーブル交換位置インターロック */ { 152U, 3U, 4U, 462U },
    /* 153: 軸退避復帰戻り位置クランプ */ { 153U, 3U, 5U, 463U },
    /* 154: ミラー軸工具径補正符号反転 */ { 154U, 3U, 6U, 464U },
    /* 155: ワーク座標G54.1拡張番号範囲ガード */ { 155U, 3U, 7U, 465U },
    /* 156: マクロ変数安全代入境界ガード */ { 156U, 4U, 0U, 466U },
    /* 157: サブプログラム呼出しネスト制限監視 */ { 157U, 4U, 1U, 467U },
    /* 158: NCアラーム履歴イベント圧縮 */ { 158U, 4U, 2U, 468U },
    /* 159: サーボ準備完了待ち補助Mコード監視 */ { 159U, 4U, 3U, 469U },
    /* 160: 経路間輪郭ブレンド隙間クランプ */ { 160U, 5U, 4U, 470U },
    /* 161: ドライラン運転フィード上限制限 */ { 161U, 5U, 5U, 471U },
    /* 162: 再開時モーダル状態整合検査 */ { 162U, 5U, 6U, 472U },
    /* 163: 主軸オリエントM19位置窓監視 */ { 163U, 5U, 7U, 473U },
    /* 164: リジッドタップ位相同期監視 */ { 164U, 6U, 0U, 474U },
    /* 165: ねじ切り逃げ面取りベクトル補正 */ { 165U, 6U, 1U, 475U },
    /* 166: 深穴加工切粉排出ドウェルガード */ { 166U, 6U, 2U, 476U },
    /* 167: 研削スパークアウトドウェルガード */ { 167U, 6U, 3U, 477U },
    /* 168: パレットクランプ確認インターロック */ { 168U, 7U, 4U, 478U },
    /* 169: 油圧圧力低下段階減速 */ { 169U, 7U, 5U, 479U },
    /* 170: サーボ偏差移動平均予兆監視 */ { 170U, 7U, 6U, 480U },
    /* 171: 絶対エンコーダ電池警告ラッチ */ { 171U, 7U, 7U, 481U },
    /* 172: ソフトリミット事前経路クランプ */ { 172U, 8U, 0U, 482U },
    /* 173: ストロークエンド近傍オーバライド禁止 */ { 173U, 8U, 1U, 483U },
    /* 174: 外部フィードホールド入力デバウンス */ { 174U, 8U, 2U, 484U },
    /* 175: オプショナルストップM01状態ゲート */ { 175U, 8U, 3U, 485U },
    /* 176: ブロックスキップ指令判定安定化 */ { 176U, 9U, 4U, 486U },
    /* 177: シングルブロック停止点生成 */ { 177U, 9U, 5U, 487U },
    /* 178: オーバライドノブ変化率制限 */ { 178U, 9U, 6U, 488U },
    /* 179: アラームリセット後インターロック解放順序 */ { 179U, 9U, 7U, 489U },
    /* 180: 加工中ワーク座標系変更禁止ラッチ */ { 180U, 0U, 0U, 490U },
    /* 181: プログラム停止時補助軸減速退避 */ { 181U, 0U, 1U, 491U },
    /* 182: 主軸負荷ピークホールド減速 */ { 182U, 0U, 2U, 492U },
    /* 183: 工具交換アーム干渉領域クランプ */ { 183U, 0U, 3U, 493U },
    /* 184: クランプ/アンクランプMコード順序監視 */ { 184U, 1U, 4U, 494U },
    /* 185: APCパレット番号整合監視 */ { 185U, 1U, 5U, 495U },
    /* 186: タレット割出し完了待ち監視 */ { 186U, 1U, 6U, 496U },
    /* 187: ローダ/アンローダ干渉待ち合わせ */ { 187U, 1U, 7U, 497U },
    /* 188: 主軸同期C軸切替ガード */ { 188U, 2U, 0U, 498U },
    /* 189: C軸輪郭制御送り正規化 */ { 189U, 2U, 1U, 499U },
    /* 190: 極小円弧半径丸め補正 */ { 190U, 2U, 2U, 500U },
    /* 191: ヘリカル補間Z送り同期補正 */ { 191U, 2U, 3U, 501U },
    /* 192: 工具長摩耗補正上限ガード */ { 192U, 3U, 4U, 502U },
    /* 193: 工具径摩耗補正上限ガード */ { 193U, 3U, 5U, 503U },
    /* 194: G10パラメータ書込み運転中禁止 */ { 194U, 3U, 6U, 504U },
    /* 195: 通信異常時NCバッファ消費停止 */ { 195U, 3U, 7U, 505U },
    /* 196: 先読み異常時安全停止ブロック生成 */ { 196U, 4U, 0U, 506U },
    /* 197: ドライランI/O出力抑止 */ { 197U, 4U, 1U, 507U },
    /* 198: シミュレーションモード安全出力マスク */ { 198U, 4U, 2U, 508U },
    /* 199: 保守モード軸単独送り速度制限 */ { 199U, 4U, 3U, 509U },
    /* 200: 輪郭先読み急峻角ブレーキ */ { 200U, 0U, 4U, 510U },
    /* 201: 微小円弧チョード誤差補正 */ { 201U, 0U, 5U, 511U },
    /* 202: 高速加工G05.1有効範囲監視 */ { 202U, 0U, 6U, 512U },
    /* 203: ナノ補間丸め誤差蓄積リセット */ { 203U, 0U, 7U, 513U },
    /* 204: 工具先端点姿勢変化率制限 */ { 204U, 5U, 0U, 514U },
    /* 205: 自動潤滑ポンプREADY監視 */ { 205U, 8U, 1U, 515U },
    /* 206: クランプエア圧低下HOLDラッチ */ { 206U, 9U, 2U, 516U },
    /* 207: 切削油温度上昇フィード制限 */ { 207U, 4U, 3U, 517U },
    /* 208: チャック回転中ドアロック二重確認 */ { 208U, 8U, 4U, 518U },
    /* 209: 主軸冷却ファン異常段階減速 */ { 209U, 4U, 5U, 519U },
    /* 210: サーボアンプ温度予兆監視 */ { 210U, 4U, 6U, 520U },
    /* 211: 工具折損検出入力デバウンス */ { 211U, 8U, 7U, 521U },
    /* 212: プローブ信号stuck-on診断 */ { 212U, 8U, 0U, 522U },
    /* 213: 機械原点近傍低速ゾーン */ { 213U, 3U, 1U, 523U },
    /* 214: NC実行中設定変更保留キュー */ { 214U, 9U, 2U, 524U },
    /* 215: エアパージREADYインターロック */ { 215U, 8U, 3U, 525U },
    /* 216: チップ洗浄圧低下安全HOLD */ { 216U, 9U, 4U, 526U },
    /* 217: ワーククランプID不一致ガード */ { 217U, 8U, 5U, 527U },
    /* 218: プログラム再開時主軸状態ゲート */ { 218U, 4U, 6U, 528U },
    /* 219: 外部軸READYタイムアウト監視 */ { 219U, 9U, 7U, 529U },
    /* 220: 熱変位補正テーブル係数反映 */ { 220U, 5U, 0U, 530U },
    /* 221: 主軸暖機状態による回転数ランプ制限 */ { 221U, 4U, 1U, 531U },
    /* 222: 機内温度オフセット補正 */ { 222U, 5U, 2U, 532U },
    /* 223: ボールねじピッチ誤差簡易補正 */ { 223U, 6U, 3U, 533U },
    /* 224: 反転バックラッシュ事前補正 */ { 224U, 6U, 4U, 534U },
    /* 225: サーボゲイン切替領域フィード制限 */ { 225U, 4U, 5U, 535U },
    /* 226: 輪郭振動ノッチ相当スムージング */ { 226U, 0U, 6U, 536U },
    /* 227: 回転軸高調波抑制補正 */ { 227U, 5U, 7U, 537U },
    /* 228: 象限突起抑制円弧補正 */ { 228U, 0U, 0U, 538U },
    /* 229: ロストモーション方向別補正 */ { 229U, 6U, 1U, 539U },
    /* 230: ガントリ軸スキュー監視補正 */ { 230U, 6U, 2U, 540U },
    /* 231: クロスレールクランプ確認インターロック */ { 231U, 8U, 3U, 541U },
    /* 232: 油圧チャック圧力監視 */ { 232U, 9U, 4U, 542U },
    /* 233: 心押し台推力監視 */ { 233U, 9U, 5U, 543U },
    /* 234: バーフィーダ受け渡しハンドシェイク */ { 234U, 8U, 6U, 544U },
    /* 235: チップコンベア過負荷減速 */ { 235U, 9U, 7U, 545U },
    /* 236: クーラント圧力/流量スイッチラッチ */ { 236U, 8U, 0U, 546U },
    /* 237: ミストコレクタREADYインターロック */ { 237U, 8U, 1U, 547U },
    /* 238: 消火装置入力時安全出力マスク */ { 238U, 9U, 2U, 548U },
    /* 239: 機内負圧異常時安全停止 */ { 239U, 9U, 3U, 549U },
    /* 240: 工具クーラント遅延停止監視 */ { 240U, 8U, 4U, 550U },
    /* 241: 主軸負荷積分しきい値減速 */ { 241U, 4U, 5U, 551U },
    /* 242: APC扉閉完了二重確認 */ { 242U, 8U, 6U, 552U },
    /* 243: 治具着座センサ不一致ガード */ { 243U, 9U, 7U, 553U },
    /* 244: 回転軸クランプ解除待ちMコード監視 */ { 244U, 8U, 0U, 554U },
    /* 245: バックグラウンド編集中NC開始禁止ラッチ */ { 245U, 9U, 1U, 555U },
    /* 246: プログラム番号/サブ番号範囲ガード */ { 246U, 3U, 2U, 556U },
    /* 247: 先読み中工具補正未登録ブロック停止 */ { 247U, 5U, 3U, 557U },
    /* 248: 送り軸ブレーキ解除確認 */ { 248U, 8U, 4U, 558U },
    /* 249: 非常停止入力チャタリング診断 */ { 249U, 9U, 5U, 559U },
    /* 250: 主軸ベルト滑り予兆監視 */ { 250U, 4U, 6U, 560U },
    /* 251: クーラントフィルタ目詰まりHOLD */ { 251U, 9U, 7U, 561U },
    /* 252: エアシリンダ到達端確認Mコード監視 */ { 252U, 8U, 0U, 562U },
    /* 253: ワーク洗浄ノズル位置インターロック */ { 253U, 8U, 1U, 563U },
    /* 254: ロボット搬入出エリア干渉待ち */ { 254U, 9U, 2U, 564U },
    /* 255: チャック把握力段階監視 */ { 255U, 4U, 3U, 565U },
    /* 256: センタースルークーラント圧上昇待ち */ { 256U, 8U, 4U, 566U },
    /* 257: 切粉堆積センサ安全減速 */ { 257U, 9U, 5U, 567U },
    /* 258: 機械照明/扉ロック保守モードゲート */ { 258U, 8U, 6U, 568U },
    /* 259: UPS低残量安全停止シーケンス */ { 259U, 9U, 7U, 569U },
};

static uint16_t s_parameter[NC_IMPL_BACKLOG_FEATURE_COUNT];

static uint32_t WordOf(uint16_t featureId) { return ((uint32_t)featureId) >> 5U; }
static uint32_t BitOf(uint16_t featureId) { return 1UL << (((uint32_t)featureId) & 31U); }

static uint8_t IsEnabled(uint16_t featureId)
{
    if (featureId >= NC_IMPL_BACKLOG_FEATURE_COUNT) { return 0U; }
    return (uint8_t)((g_ncImplementationBacklogStatus.implemented_mask[WordOf(featureId)] & BitOf(featureId)) != 0U);
}

static uint8_t IsMotion(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->motion_type == NC_MOTION_RAPID) ||
                     (pBlock->motion_type == NC_MOTION_LINEAR) ||
                     (pBlock->motion_type == NC_MOTION_ARC_CW) ||
                     (pBlock->motion_type == NC_MOTION_ARC_CCW) ||
                     (pBlock->motion_type == NC_MOTION_CANNED_CYCLE) ||
                     (pBlock->motion_type == NC_MOTION_THREAD) ||
                     (pBlock->motion_type == NC_MOTION_SKIP) ||
                     (pBlock->motion_type == NC_MOTION_REFERENCE_RETURN) ||
                     (pBlock->motion_type == NC_MOTION_ADVANCED_INTERP));
}

static void CopyAxes(int32_t* pDst, const int32_t* pSrc)
{
    uint32_t axis;
    for (axis = 0U; axis < AXIS_MAX; ++axis) { pDst[axis] = pSrc[axis]; }
}


static int32_t ClampI32(int32_t value, int32_t minValue, int32_t maxValue)
{
    if (value < minValue) { return minValue; }
    if (value > maxValue) { return maxValue; }
    return value;
}

static int32_t AbsI32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static uint16_t ParamOrDefault(uint16_t featureId, uint16_t defaultValue)
{
    uint16_t p = 0U;
    if (featureId < NC_IMPL_BACKLOG_FEATURE_COUNT) { p = s_parameter[featureId]; }
    return (p == 0U) ? defaultValue : p;
}

static void SetMinTicks(NC_EXEC_BLOCK* pBlock, uint32_t minTicks)
{
    if (pBlock->interp_ticks < minTicks) { pBlock->interp_ticks = minTicks; }
    if (pBlock->accel_ticks == 0U) { pBlock->accel_ticks = 1U; }
    if (pBlock->decel_ticks == 0U) { pBlock->decel_ticks = 1U; }
}

static void SlowFeed(NC_EXEC_BLOCK* pBlock, uint16_t percent)
{
    uint16_t pct = percent;
    if (pct == 0U) { pct = NC_IMPL_BACKLOG_DEFAULT_OVERRIDE; }
    if (pct > NC_FEED_OVERRIDE_MAX_PERCENT) { pct = NC_FEED_OVERRIDE_MAX_PERCENT; }
    if ((pBlock->feed > 0) && (pct < 100U)) {
        pBlock->feed = (int32_t)(((int64_t)pBlock->feed * (int64_t)pct) / 100LL);
        if (pBlock->feed <= 0) { pBlock->feed = 1; }
        g_ncImplementationBacklogStatus.slowdown_applied_blocks++;
    }
    g_ncImplementationBacklogStatus.active_override_percent = pct;
}

static int32_t SafeDivI32(int32_t value, int32_t divisor)
{
    if (divisor == 0) { return value; }
    return value / divisor;
}

static void SetFeatureFlagAndTicks(NC_EXEC_BLOCK* pBlock, uint32_t featureFlag, uint32_t minTicks)
{
    pBlock->feature_flags |= featureFlag;
    SetMinTicks(pBlock, minTicks);
}

static void NormalizeRotaryShortestPath(NC_EXEC_BLOCK* pBlock, uint32_t axis)
{
    int32_t start;
    int32_t target;
    int32_t delta;
    if (axis >= AXIS_MAX) { return; }
    if ((pBlock->axis_mask & (1UL << axis)) == 0U) { return; }
    start = pBlock->axis_start[axis];
    target = pBlock->axis_target[axis];
    delta = target - start;
    while (delta > NC_IMPL_BACKLOG_V23_ROTARY_DEG_180) { delta -= NC_IMPL_BACKLOG_V23_ROTARY_DEG_360; }
    while (delta < -NC_IMPL_BACKLOG_V23_ROTARY_DEG_180) { delta += NC_IMPL_BACKLOG_V23_ROTARY_DEG_360; }
    pBlock->axis_target[axis] = start + delta;
}

static void ClampAxisTargets(NC_EXEC_BLOCK* pBlock, int32_t minValue, int32_t maxValue)
{
    uint32_t axis;
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
            pBlock->axis_target[axis] = ClampI32(pBlock->axis_target[axis], minValue, maxValue);
        }
    }
}

static void ApplyV23Policy(const NC_BACKLOG_DESCRIPTOR* pDesc, NC_EXEC_BLOCK* pBlock)
{
    uint16_t param = ParamOrDefault(pDesc->feature_id, NC_IMPL_BACKLOG_DEFAULT_OVERRIDE);
    switch (pDesc->feature_id) {
    case 100U: /* G70 finish cycle: deterministic finish pass envelope. */
        pBlock->cycle_repeat = (pBlock->cycle_repeat == 0U) ? 1U : pBlock->cycle_repeat;
        pBlock->cycle_flags |= 0x0001U;
        SetMinTicks(pBlock, 4U);
        SlowFeed(pBlock, (uint16_t)ClampI32((int32_t)param, 60, 100));
        break;
    case 101U: /* G71 roughing: split depth by Q/parameter and keep RT path bounded. */
        if (pBlock->q_value <= 0) { pBlock->q_value = (int32_t)ClampI32((int32_t)param * 10, 10, 5000); }
        pBlock->cycle_repeat = (uint16_t)ClampI32((AbsI32(pBlock->axis_target[0] - pBlock->axis_start[0]) / ClampI32(pBlock->q_value, 1, 5000)) + 1, 1, 20);
        SetMinTicks(pBlock, 5U);
        SlowFeed(pBlock, 85U);
        break;
    case 102U: /* G72 facing roughing: add safe Z relief. */
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->axis_target[2] += (int32_t)ClampI32((int32_t)param, 5, 500);
        pBlock->cycle_flags |= 0x0002U;
        SetMinTicks(pBlock, 5U);
        break;
    case 103U: /* G73 pattern repeat: provide remaining count and minimum ticks. */
        pBlock->cycle_repeat = (uint16_t)ClampI32((pBlock->cycle_repeat == 0U) ? (int32_t)(param % 8U) + 2 : pBlock->cycle_repeat, 1, 32);
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)(1000U + pDesc->feature_id) : pBlock->p_word;
        SetMinTicks(pBlock, 6U);
        break;
    case 104U: /* G75 groove peck: default Q and dwell guard. */
        if (pBlock->q_value <= 0) { pBlock->q_value = (int32_t)ClampI32((int32_t)param * 5, 10, 3000); }
        if (pBlock->p_word == 0U) { pBlock->p_word = 2U; }
        pBlock->cycle_flags |= 0x0004U;
        SetMinTicks(pBlock, 6U);
        break;
    case 105U: /* G76 multi-pass thread: force per-rev feed and conservative decel. */
        pBlock->motion_type = NC_MOTION_THREAD;
        pBlock->feed_mode = NC_FEED_MODE_PER_REV;
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 300U; }
        pBlock->cycle_repeat = (uint16_t)ClampI32((int32_t)(param % 10U) + 2, 2, 16);
        SetMinTicks(pBlock, 8U);
        SlowFeed(pBlock, 70U);
        break;
    case 106U: /* G90 taper: apply deterministic X correction from Z travel. */
        if ((pBlock->axis_mask & 0x00000005UL) != 0U) {
            int32_t zDelta = pBlock->axis_target[2] - pBlock->axis_start[2];
            pBlock->axis_mask |= 0x00000001UL;
            pBlock->axis_target[0] += (zDelta / 10) + (int32_t)(param % 20U);
        }
        SetMinTicks(pBlock, 4U);
        break;
    case 107U: /* G92 thread: preserve pitch with spindle fallback. */
        pBlock->motion_type = NC_MOTION_THREAD;
        pBlock->feed_mode = NC_FEED_MODE_PER_REV;
        if (pBlock->feed <= 0) { pBlock->feed = (int32_t)ClampI32((int32_t)param * 10, 100, 6000); }
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 250U; }
        SetMinTicks(pBlock, 8U);
        break;
    case 108U: /* G94 facing: Z clearance/return surface correction. */
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->r_value = (pBlock->r_value == 0) ? (int32_t)ClampI32((int32_t)param, 10, 1000) : pBlock->r_value;
        pBlock->axis_target[2] = pBlock->axis_target[2] + pBlock->r_value;
        SetMinTicks(pBlock, 4U);
        break;
    case 109U: /* G31 high-speed skip latch: mark skip motion and guarded feed. */
        pBlock->motion_type = NC_MOTION_SKIP;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        SlowFeed(pBlock, 80U);
        SetMinTicks(pBlock, 3U);
        break;
    case 110U: /* EGB skip phase window: use P as fixed phase window and smoothing flag. */
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param, 1, 360) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 4U);
        break;
    case 111U: /* Distance-coded reference mark: convert to reference-return block. */
        pBlock->motion_type = NC_MOTION_REFERENCE_RETURN;
        pBlock->axis_mask |= 0x00000001UL;
        pBlock->axis_target[0] = pBlock->axis_start[0] + (int32_t)ClampI32((int32_t)param * 10, 10, 10000);
        SetMinTicks(pBlock, 5U);
        break;
    case 112U: /* Absolute-address reference mark: snap X to absolute mark. */
        pBlock->motion_type = NC_MOTION_REFERENCE_RETURN;
        pBlock->axis_mask |= 0x00000001UL;
        pBlock->axis_target[0] = (int32_t)ClampI32((int32_t)param * 100, -100000, 100000);
        SetMinTicks(pBlock, 5U);
        break;
    case 113U: /* Position switch latch: reduce feed and publish guarded line. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 60U);
        SetMinTicks(pBlock, 6U);
        break;
    case 114U: /* Rotary rollover: shortest-path normalization on A axis. */
        pBlock->axis_mask |= 0x00000008UL;
        NormalizeRotaryShortestPath(pBlock, 3U);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        break;
    case 115U: { /* Follow-up error prewarning: slowdown on large commanded travel. */
        int32_t maxDelta = 0;
        uint32_t axis;
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            int32_t d = AbsI32(pBlock->axis_target[axis] - pBlock->axis_start[axis]);
            if (d > maxDelta) { maxDelta = d; }
        }
        if (maxDelta > (int32_t)ClampI32((int32_t)param * 20, 100, 20000)) { SlowFeed(pBlock, 65U); }
        SetMinTicks(pBlock, 4U);
        break;
    }
    case 116U: /* Rotary fixture dynamic offset: fixed offset on XYZ. */
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] += (int32_t)(param % 50U);
        pBlock->axis_target[1] -= (int32_t)(param % 30U);
        pBlock->axis_target[2] += (int32_t)(param % 20U);
        pBlock->feature_flags |= NC_FEATURE_FLAG_COORD_ROTATION;
        break;
    case 117U: /* Work setting error compensation: small deterministic XYZ trim. */
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] -= (int32_t)(param % 25U);
        pBlock->axis_target[1] += (int32_t)(param % 25U);
        pBlock->axis_target[2] -= (int32_t)(param % 10U);
        pBlock->feature_flags |= NC_FEATURE_FLAG_TILTED_PLANE;
        break;
    case 118U: /* Cylindrical/polar feed normalization. */
        pBlock->feature_flags |= (NC_FEATURE_FLAG_POLAR_COORD | NC_FEATURE_FLAG_COORD_ROTATION);
        if (pBlock->feed > 0) { SlowFeed(pBlock, 88U); }
        SetMinTicks(pBlock, 5U);
        break;
    case 119U: /* M-code handshake guard: require MFIN wait and bounded dwell. */
        pBlock->aux_flags |= NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT;
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 100U + (uint32_t)(param % 100U); }
        if (pBlock->p_word == 0U) { pBlock->p_word = 5U; }
        SetMinTicks(pBlock, 4U);
        break;

    case 120U: /* 5-axis RTCP: keep tool tip target stable while tilting A/B. */
        pBlock->axis_mask |= 0x0000001fUL;
        pBlock->tool_axis_command = 1U;
        pBlock->tool_axis_angle_deg[0] = ClampI32(pBlock->tool_axis_angle_deg[0] + (int32_t)(param % 15U), -90000, 90000);
        pBlock->tool_axis_angle_deg[1] = ClampI32(pBlock->tool_axis_angle_deg[1] - (int32_t)(param % 12U), -90000, 90000);
        pBlock->axis_target[0] += SafeDivI32(pBlock->tool_axis_angle_deg[0], 1000);
        pBlock->axis_target[1] += SafeDivI32(pBlock->tool_axis_angle_deg[1], 1000);
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_TOOL_AXIS_DIR, 6U);
        break;
    case 121U: /* Tilted work plane B/C normalization. */
        pBlock->kinematics_command = 1U;
        pBlock->axis_mask |= 0x00000018UL;
        pBlock->tilt_angle_deg[1] = ClampI32(pBlock->tilt_angle_deg[1] + (int32_t)(param % 30U), -180000, 180000);
        pBlock->tilt_angle_deg[2] = ClampI32(pBlock->tilt_angle_deg[2] - (int32_t)(param % 30U), -180000, 180000);
        NormalizeRotaryShortestPath(pBlock, 3U);
        NormalizeRotaryShortestPath(pBlock, 4U);
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_TILTED_PLANE, 6U);
        break;
    case 122U: /* TCPM feed compensation for tool-center-point motion. */
        pBlock->tool_axis_command = 1U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_TOOL_AXIS_DIR;
        if (pBlock->feed > 0) { SlowFeed(pBlock, 82U); }
        pBlock->interp_ticks += (pBlock->interp_ticks < 12U) ? 2U : 0U;
        break;
    case 123U: /* High-speed tiny segment compression. */
        if (AbsI32(pBlock->axis_target[0] - pBlock->axis_start[0]) < (int32_t)ClampI32((int32_t)param, 20, 500)) {
            pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
            if (pBlock->interp_ticks > 2U) { pBlock->interp_ticks--; }
            if (pBlock->feed > 0) { SlowFeed(pBlock, 96U); }
        }
        break;
    case 124U: /* Curvature continuous lookahead acceleration. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        pBlock->accel_ticks += 2U;
        pBlock->decel_ticks += 2U;
        SlowFeed(pBlock, 90U);
        SetMinTicks(pBlock, 7U);
        break;
    case 125U: /* Jerk-limited S-curve acceleration profile. */
        pBlock->feed_profile = NC_FEED_PROFILE_TRAPEZOID;
        pBlock->accel_ticks += (pBlock->accel_ticks < 8U) ? 3U : 1U;
        pBlock->decel_ticks += (pBlock->decel_ticks < 8U) ? 3U : 1U;
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_SMOOTHING, 8U);
        break;
    case 126U: /* Auto corner radius insertion. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        if (pBlock->r_value == 0) { pBlock->r_value = (int32_t)ClampI32((int32_t)param, 5, 500); }
        pBlock->axis_target[0] -= SafeDivI32(pBlock->r_value, 4);
        pBlock->axis_target[1] += SafeDivI32(pBlock->r_value, 4);
        SetMinTicks(pBlock, 5U);
        break;
    case 127U: /* Automatic chamfer insertion at block end. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        if (pBlock->q_value == 0) { pBlock->q_value = (int32_t)ClampI32((int32_t)param, 3, 300); }
        pBlock->axis_target[0] -= SafeDivI32(pBlock->q_value, 3);
        pBlock->axis_target[1] -= SafeDivI32(pBlock->q_value, 3);
        SetMinTicks(pBlock, 5U);
        break;
    case 128U: /* Lookahead low-watermark slowdown to avoid HOLD underrun. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        SlowFeed(pBlock, 72U);
        pBlock->interp_ticks += 3U;
        SetMinTicks(pBlock, 8U);
        break;
    case 129U: /* Sequence search resume-position verification. */
        if (pBlock->p_word == 0U) { pBlock->p_word = 2000U + pBlock->line_no; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        pBlock->axis_mask &= NC_AXIS_CONFIG_DEFAULT_PATH_MASK;
        SetMinTicks(pBlock, 4U);
        break;
    case 130U: /* Dual PLC auxiliary queue guard. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 200U + (uint32_t)(param % 80U); }
        if (pBlock->p_word == 0U) { pBlock->p_word = 2U; }
        SetMinTicks(pBlock, 4U);
        break;
    case 131U: /* Coolant/air-blow M-code interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = ((param & 1U) != 0U) ? 8U : 7U; }
        SlowFeed(pBlock, 95U);
        SetMinTicks(pBlock, 3U);
        break;
    case 132U: /* CSS spindle zero-diameter guard. */
        if (pBlock->axis_target[0] == 0) { pBlock->axis_target[0] = (int32_t)ClampI32((int32_t)param, 1, 1000); }
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 500U; }
        pBlock->feed_mode = NC_FEED_MODE_PER_REV;
        SetMinTicks(pBlock, 4U);
        break;
    case 133U: /* Automatic spindle gear range selection. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 600U + (uint32_t)(param * 2U); }
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = (pBlock->spindle_speed > 1200U) ? 42U : 41U; }
        pBlock->aux_flags |= NC_AUX_FLAG_M_CODE;
        SetMinTicks(pBlock, 3U);
        break;
    case 134U: /* ATC home-position confirmation before tool exchange. */
        pBlock->tool_no = (pBlock->tool_no == 0U) ? (uint32_t)((param % 24U) + 1U) : pBlock->tool_no;
        pBlock->aux_m_code = 6U;
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SetMinTicks(pBlock, 5U);
        break;
    case 135U: /* Tool wear fine adjustment. */
        pBlock->tool_length_command = (pBlock->tool_length_command == 0U) ? 43U : pBlock->tool_length_command;
        pBlock->h_offset_no = (pBlock->h_offset_no == 0U) ? (uint16_t)((param % 32U) + 1U) : pBlock->h_offset_no;
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->axis_target[2] += (int32_t)(param % 8U) - 4;
        SetMinTicks(pBlock, 4U);
        break;
    case 136U: /* Probe measurement result reflected to work offset. */
        pBlock->motion_type = NC_MOTION_SKIP;
        pBlock->feature_flags |= NC_FEATURE_FLAG_TILTED_PLANE;
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] += (int32_t)(param % 10U);
        pBlock->axis_target[1] -= (int32_t)(param % 10U);
        SetMinTicks(pBlock, 5U);
        break;
    case 137U: /* Grinding dresser amount compensation. */
        pBlock->motion_type = NC_MOTION_CANNED_CYCLE;
        pBlock->cycle_flags |= 0x0020U;
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->axis_target[2] -= (int32_t)ClampI32((int32_t)param, 1, 200);
        SlowFeed(pBlock, 68U);
        SetMinTicks(pBlock, 9U);
        break;
    case 138U: /* Restart inhibit latch after emergency stop recovery. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feed = 1;
        pBlock->interp_ticks += 10U;
        SetMinTicks(pBlock, 12U);
        break;
    case 139U: /* Door-open safe speed monitoring. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 30U);
        pBlock->interp_ticks += 6U;
        SetMinTicks(pBlock, 10U);
        break;

    case 140U: /* 5-axis singularity approach slowdown. */
        pBlock->tool_axis_command = 1U;
        pBlock->axis_mask |= 0x00000018UL;
        pBlock->tool_axis_angle_deg[0] = ClampI32(pBlock->tool_axis_angle_deg[0], -85000, 85000);
        pBlock->tool_axis_angle_deg[1] = ClampI32(pBlock->tool_axis_angle_deg[1], -85000, 85000);
        SlowFeed(pBlock, 55U);
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_TOOL_AXIS_DIR, 10U);
        break;
    case 141U: /* Tilted-plane retract vector limiting. */
        pBlock->kinematics_command = 1U;
        pBlock->axis_retract_command = 1U;
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[2] += (int32_t)ClampI32((int32_t)param, 20, 1500);
        pBlock->feature_flags |= (NC_FEATURE_FLAG_TILTED_PLANE | NC_FEATURE_FLAG_AXIS_RETRACT);
        SetMinTicks(pBlock, 7U);
        break;
    case 142U: /* NURBS control-point weight range monitor. */
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
        pBlock->q_value = ClampI32((pBlock->q_value == 0) ? (int32_t)param : pBlock->q_value, 1, 1000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        SlowFeed(pBlock, 88U);
        SetMinTicks(pBlock, 6U);
        break;
    case 143U: /* Involute base-radius guard. */
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
        if (pBlock->arc_radius <= 0) { pBlock->arc_radius = (int32_t)ClampI32((int32_t)param * 10, 100, 50000); }
        if (AbsI32(pBlock->axis_target[0] - pBlock->axis_start[0]) < pBlock->arc_radius / 20) {
            pBlock->axis_target[0] += pBlock->arc_radius / 20;
        }
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_ADVANCED_INTERP, 7U);
        break;
    case 144U: /* 3D arc plane-normal normalization. */
        pBlock->motion_type = (pBlock->motion_type == NC_MOTION_NONE) ? NC_MOTION_ARC_CW : pBlock->motion_type;
        pBlock->plane = NC_PLANE_XY;
        if (pBlock->arc_radius <= 0) { pBlock->arc_radius = (int32_t)ClampI32((int32_t)param * 5, 50, 30000); }
        pBlock->axis_mask |= 0x00000007UL;
        SetMinTicks(pBlock, 6U);
        break;
    case 145U: /* High-speed binary NC block consistency check. */
        pBlock->valid = 1U;
        pBlock->generation++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        if (pBlock->feed <= 0) { pBlock->feed = (int32_t)ClampI32((int32_t)param * 100, 1000, 120000); }
        SetMinTicks(pBlock, 3U);
        break;
    case 146U: /* AI contour learning correction clamp. */
        pBlock->feature_flags |= (NC_FEATURE_FLAG_ADVANCED_INTERP | NC_FEATURE_FLAG_SMOOTHING);
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], pBlock->axis_start[0] - 200000, pBlock->axis_start[0] + 200000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], pBlock->axis_start[1] - 200000, pBlock->axis_start[1] + 200000);
        SlowFeed(pBlock, 86U);
        SetMinTicks(pBlock, 7U);
        break;
    case 147U: /* Free-form mesh boundary slowdown. */
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        pBlock->interp_ticks += 4U;
        SlowFeed(pBlock, 78U);
        break;
    case 148U: /* HPCC error-band slowdown. */
        pBlock->feature_flags |= (NC_FEATURE_FLAG_ADVANCED_INTERP | NC_FEATURE_FLAG_SMOOTHING);
        SlowFeed(pBlock, 74U);
        pBlock->accel_ticks += 1U;
        pBlock->decel_ticks += 2U;
        SetMinTicks(pBlock, 8U);
        break;
    case 149U: /* Exact-stop block-end synchronization. */
        pBlock->feed_profile = NC_FEED_PROFILE_TRAPEZOID;
        pBlock->decel_ticks += 4U;
        pBlock->interp_ticks += 4U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 65U);
        break;
    case 150U: /* Multi-path synchronization wait timeout. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        if (pBlock->p_word == 0U) { pBlock->p_word = (uint32_t)ClampI32((int32_t)param, 1, 999); }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 6U);
        break;
    case 151U: /* Overlay control duplicate-axis detection. */
        pBlock->axis_control_mask &= pBlock->axis_mask;
        if (pBlock->axis_control_mask == 0U) { pBlock->axis_control_mask = pBlock->axis_mask & 0x0000000fUL; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 90U);
        SetMinTicks(pBlock, 5U);
        break;
    case 152U: /* Double-table exchange-position interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 60U + (uint32_t)(param % 10U); }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->axis_mask |= 0x00000003UL;
        SetMinTicks(pBlock, 8U);
        break;
    case 153U: /* Axis retract/return position clamp. */
        pBlock->axis_retract_command = 1U;
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], -500000, 500000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], -500000, 500000);
        pBlock->axis_target[2] = ClampI32(pBlock->axis_target[2], -500000, 500000);
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_AXIS_RETRACT, 6U);
        break;
    case 154U: /* Mirror-axis cutter compensation sign inversion. */
        pBlock->mirror_command = 1U;
        pBlock->mirror_axis_mask |= 0x00000001UL;
        pBlock->cutter_comp_command = (pBlock->cutter_comp_command == 41U) ? 42U : 41U;
        pBlock->axis_target[0] = -pBlock->axis_target[0];
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_MIRROR | NC_FEATURE_FLAG_CUTTER_RADIUS, 5U);
        break;
    case 155U: /* G54.1 extended work-coordinate number guard. */
        pBlock->p_word = (uint32_t)ClampI32((pBlock->p_word == 0U) ? (int32_t)(param % 48U) + 1 : (int32_t)pBlock->p_word, 1, 48);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        pBlock->axis_mask &= NC_AXIS_CONFIG_DEFAULT_PATH_MASK;
        SetMinTicks(pBlock, 3U);
        break;
    case 156U: /* Macro-variable safe assignment boundary guard. */
        pBlock->q_value = ClampI32(pBlock->q_value, -999999, 999999);
        pBlock->r_value = ClampI32(pBlock->r_value, -999999, 999999);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 3U);
        break;
    case 157U: /* Subprogram nesting limit monitor. */
        pBlock->p_word = (uint32_t)ClampI32((int32_t)(pBlock->p_word & 0x0fU), 0, 8);
        pBlock->aux_flags |= NC_AUX_FLAG_M_CODE;
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 98U; }
        SetMinTicks(pBlock, 4U);
        break;
    case 158U: /* NC alarm-history event compression. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        pBlock->generation += (uint32_t)(param & 0x0fU);
        pBlock->feed_override_percent = (uint16_t)ClampI32((int32_t)pBlock->feed_override_percent, 1, 100);
        SetMinTicks(pBlock, 3U);
        break;
    case 159U: /* Servo-ready wait auxiliary M-code monitoring. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 80U + (uint32_t)(param % 10U); }
        pBlock->feed = (pBlock->feed <= 0) ? 1 : pBlock->feed;
        SlowFeed(pBlock, 50U);
        SetMinTicks(pBlock, 6U);
        break;

    case 160U: /* Cross-path contour blend gap clamp. */
        pBlock->feature_flags |= (NC_FEATURE_FLAG_ADVANCED_INTERP | NC_FEATURE_FLAG_SMOOTHING);
        pBlock->axis_mask |= 0x00000003UL;
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], pBlock->axis_start[0] - 250000, pBlock->axis_start[0] + 250000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], pBlock->axis_start[1] - 250000, pBlock->axis_start[1] + 250000);
        SlowFeed(pBlock, 84U);
        SetMinTicks(pBlock, 6U);
        break;
    case 161U: /* Dry-run feed cap. */
        if (pBlock->feed <= 0) { pBlock->feed = 1000; }
        pBlock->feed = ClampI32(pBlock->feed, 1, (int32_t)ClampI32((int32_t)param * 100, 2000, 30000));
        pBlock->feed_override_percent = 100U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 3U);
        break;
    case 162U: /* Restart modal-state consistency check. */
        pBlock->modal_flags |= 0x00000001UL;
        if ((pBlock->plane != NC_PLANE_XY) && (pBlock->plane != NC_PLANE_ZX) && (pBlock->plane != NC_PLANE_YZ)) { pBlock->plane = NC_PLANE_XY; }
        if ((pBlock->feed_mode != NC_FEED_MODE_PER_MIN) && (pBlock->feed_mode != NC_FEED_MODE_PER_REV)) { pBlock->feed_mode = NC_FEED_MODE_PER_MIN; }
        pBlock->generation++;
        SetMinTicks(pBlock, 4U);
        break;
    case 163U: /* M19 spindle-orientation position window monitor. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        pBlock->aux_m_code = 19U;
        pBlock->spindle_speed = 0U;
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param, 1, 360) : pBlock->p_word;
        SetMinTicks(pBlock, 5U);
        break;
    case 164U: /* Rigid tapping spindle/feed phase monitor. */
        pBlock->motion_type = NC_MOTION_CANNED_CYCLE;
        pBlock->feed_mode = NC_FEED_MODE_PER_REV;
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 400U; }
        if (pBlock->feed <= 0) { pBlock->feed = (int32_t)ClampI32((int32_t)param * 10, 100, 5000); }
        pBlock->cycle_flags |= 0x0040U;
        SetMinTicks(pBlock, 9U);
        break;
    case 165U: /* Thread retract chamfer-vector correction. */
        pBlock->motion_type = NC_MOTION_THREAD;
        pBlock->axis_mask |= 0x00000005UL;
        pBlock->axis_target[0] += (int32_t)ClampI32((int32_t)param, 5, 300);
        pBlock->axis_target[2] += (int32_t)ClampI32((int32_t)param / 2, 2, 150);
        SlowFeed(pBlock, 72U);
        SetMinTicks(pBlock, 8U);
        break;
    case 166U: /* Deep-hole chip evacuation dwell guard. */
        pBlock->motion_type = NC_MOTION_CANNED_CYCLE;
        pBlock->cycle_flags |= 0x0080U;
        if (pBlock->q_value <= 0) { pBlock->q_value = (int32_t)ClampI32((int32_t)param * 8, 50, 4000); }
        if (pBlock->p_word == 0U) { pBlock->p_word = (uint32_t)ClampI32((int32_t)param / 2, 1, 200); }
        SetMinTicks(pBlock, 10U);
        break;
    case 167U: /* Grinding spark-out dwell guard. */
        pBlock->motion_type = NC_MOTION_CANNED_CYCLE;
        pBlock->cycle_flags |= 0x0100U;
        if (pBlock->p_word == 0U) { pBlock->p_word = (uint32_t)ClampI32((int32_t)param, 3, 500); }
        SlowFeed(pBlock, 58U);
        SetMinTicks(pBlock, 11U);
        break;
    case 168U: /* Pallet-clamp confirmation interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 61U; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = 1;
        SetMinTicks(pBlock, 8U);
        break;
    case 169U: /* Hydraulic-pressure staged deceleration. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 42U);
        pBlock->interp_ticks += 8U;
        pBlock->decel_ticks += 4U;
        SetMinTicks(pBlock, 12U);
        break;
    case 170U: /* Servo following-error moving-average prewarning. */
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        pBlock->accel_ticks += 2U;
        pBlock->decel_ticks += 2U;
        SlowFeed(pBlock, 68U);
        SetMinTicks(pBlock, 8U);
        break;
    case 171U: /* Absolute-encoder battery warning latch. */
        pBlock->motion_type = NC_MOTION_REFERENCE_RETURN;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->axis_mask |= 0x00000007UL;
        SlowFeed(pBlock, 50U);
        SetMinTicks(pBlock, 9U);
        break;
    case 172U: /* Soft-limit precheck path clamp. */
        pBlock->axis_mask |= 0x0000000fUL;
        ClampAxisTargets(pBlock, -800000, 800000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 80U);
        SetMinTicks(pBlock, 6U);
        break;
    case 173U: /* Stroke-end override prohibition. */
        pBlock->feed_override_percent = 100U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        if (pBlock->feed <= 0) { pBlock->feed = 1; }
        pBlock->feed = ClampI32(pBlock->feed, 1, 20000);
        SetMinTicks(pBlock, 7U);
        break;
    case 174U: /* External feed-hold input debounce. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param / 10, 2, 50) : pBlock->p_word;
        pBlock->interp_ticks += 2U;
        SlowFeed(pBlock, 75U);
        break;
    case 175U: /* Optional stop M01 state gate. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        pBlock->aux_m_code = 1U;
        pBlock->feed = 1;
        pBlock->interp_ticks += 5U;
        SetMinTicks(pBlock, 7U);
        break;
    case 176U: /* Block-skip command decision stabilization. */
        pBlock->valid = 1U;
        pBlock->generation++;
        pBlock->modal_flags |= 0x00000002UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 3U);
        break;
    case 177U: /* Single-block stop point generation. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->decel_ticks += 5U;
        pBlock->interp_ticks += 5U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 60U);
        break;
    case 178U: /* Override knob rate limiter. */
        pBlock->feed_override_percent = (uint16_t)ClampI32((int32_t)pBlock->feed_override_percent, 50, 120);
        pBlock->accel_ticks += 1U;
        pBlock->decel_ticks += 1U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 5U);
        break;
    case 179U: /* Alarm-reset interlock release sequence. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 90U; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = 1;
        SetMinTicks(pBlock, 10U);
        break;

    case 180U: /* Work-coordinate change inhibit latch during machining. */
        pBlock->modal_flags |= 0x00000004UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        if (IsMotion(pBlock) != 0U) { SlowFeed(pBlock, 86U); }
        SetMinTicks(pBlock, 5U);
        break;
    case 181U: /* Program stop auxiliary-axis decel retract. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 0U; }
        pBlock->axis_mask |= 0x00000008UL;
        pBlock->axis_target[3] = pBlock->axis_start[3] + (int32_t)ClampI32((int32_t)param * 20, 100, 20000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 55U);
        SetMinTicks(pBlock, 9U);
        break;
    case 182U: /* Spindle-load peak hold deceleration. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 300U; }
        pBlock->feed = (pBlock->feed <= 0) ? 1 : pBlock->feed;
        pBlock->decel_ticks += 3U;
        SlowFeed(pBlock, 62U);
        SetMinTicks(pBlock, 8U);
        break;
    case 183U: /* Tool-change arm interference area clamp. */
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], -600000, 600000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], -600000, 600000);
        pBlock->axis_target[2] = ClampI32(pBlock->axis_target[2], -200000, 800000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SetMinTicks(pBlock, 7U);
        break;
    case 184U: /* Clamp/unclamp M-code sequence monitor. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 10U + (uint32_t)(param % 2U); }
        pBlock->generation++;
        pBlock->feed = 1;
        SetMinTicks(pBlock, 6U);
        break;
    case 185U: /* APC pallet number consistency monitor. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param % 32, 1, 31) : pBlock->p_word;
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 60U; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 8U);
        break;
    case 186U: /* Turret indexing completion wait monitor. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        pBlock->tool_no = (pBlock->tool_no == 0U) ? (uint32_t)ClampI32((int32_t)param % 40, 1, 39) : pBlock->tool_no;
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 6U; }
        SlowFeed(pBlock, 50U);
        SetMinTicks(pBlock, 9U);
        break;
    case 187U: /* Loader/unloader interference wait. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->axis_mask |= 0x00000003UL;
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], -300000, 300000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], -300000, 300000);
        SlowFeed(pBlock, 48U);
        SetMinTicks(pBlock, 10U);
        break;
    case 188U: /* Spindle-synchronous C-axis changeover guard. */
        pBlock->axis_mask |= 0x00000008UL;
        pBlock->feature_flags |= (NC_FEATURE_FLAG_TOOL_AXIS_DIR | NC_FEATURE_FLAG_AXIS_ASSIGNMENT);
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 200U; }
        NormalizeRotaryShortestPath(pBlock, 3U);
        SlowFeed(pBlock, 66U);
        SetMinTicks(pBlock, 8U);
        break;
    case 189U: /* C-axis contour feed normalization. */
        pBlock->axis_mask |= 0x00000009UL;
        NormalizeRotaryShortestPath(pBlock, 3U);
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 1000 : pBlock->feed, 1, 24000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 6U);
        break;
    case 190U: /* Tiny arc-radius rounding correction. */
        if ((pBlock->motion_type == NC_MOTION_ARC_CW) || (pBlock->motion_type == NC_MOTION_ARC_CCW)) {
            pBlock->arc_radius = ClampI32((pBlock->arc_radius == 0) ? (int32_t)param : pBlock->arc_radius, 10, 2000000);
            pBlock->arc_center[0] = ClampI32(pBlock->arc_center[0], -2000000, 2000000);
            pBlock->arc_center[1] = ClampI32(pBlock->arc_center[1], -2000000, 2000000);
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 5U);
        break;
    case 191U: /* Helical interpolation Z-feed synchronization. */
        pBlock->motion_type = (pBlock->motion_type == NC_MOTION_RAPID) ? NC_MOTION_LINEAR : pBlock->motion_type;
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->axis_target[2] = pBlock->axis_start[2] + SafeDivI32((pBlock->axis_target[2] - pBlock->axis_start[2]), 1);
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        SlowFeed(pBlock, 78U);
        SetMinTicks(pBlock, 7U);
        break;
    case 192U: /* Tool-length wear correction upper guard. */
        pBlock->tool_length_command = (pBlock->tool_length_command == NC_TOOL_LENGTH_CMD_NONE) ? NC_TOOL_LENGTH_CMD_POSITIVE : pBlock->tool_length_command;
        pBlock->h_offset_no = (uint16_t)ClampI32((int32_t)((pBlock->h_offset_no == 0U) ? (param % 99U) + 1U : pBlock->h_offset_no), 1, 99);
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->axis_target[2] = ClampI32(pBlock->axis_target[2], -900000, 900000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_TOOL_LENGTH;
        SetMinTicks(pBlock, 4U);
        break;
    case 193U: /* Cutter-radius wear correction upper guard. */
        pBlock->cutter_comp_command = (pBlock->cutter_comp_command == NC_CUTTER_COMP_CMD_NONE) ? NC_CUTTER_COMP_CMD_LEFT : pBlock->cutter_comp_command;
        pBlock->d_offset_no = (uint16_t)ClampI32((int32_t)((pBlock->d_offset_no == 0U) ? (param % 99U) + 1U : pBlock->d_offset_no), 1, 99);
        pBlock->axis_mask |= 0x00000003UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_CUTTER_RADIUS;
        SlowFeed(pBlock, 88U);
        SetMinTicks(pBlock, 4U);
        break;
    case 194U: /* Inhibit G10 parameter write while running. */
        pBlock->g_code10 = 100U;
        pBlock->modal_flags |= 0x00000008UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feed = 1;
        SetMinTicks(pBlock, 6U);
        break;
    case 195U: /* Stop NC buffer consumption during communication error. */
        pBlock->valid = 1U;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = 1;
        pBlock->interp_ticks += 6U;
        SetMinTicks(pBlock, 10U);
        break;
    case 196U: /* Generate safety-stop block on lookahead fault. */
        pBlock->motion_type = NC_MOTION_LINEAR;
        pBlock->axis_mask = 0U;
        pBlock->feed = 1;
        pBlock->accel_ticks = 1U;
        pBlock->decel_ticks += 8U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SetMinTicks(pBlock, 12U);
        break;
    case 197U: /* Dry-run I/O output suppression. */
        pBlock->aux_flags &= ~NC_AUX_FLAG_M_CODE;
        pBlock->aux_m_code = 0U;
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 1000 : pBlock->feed, 1, 50000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 3U);
        break;
    case 198U: /* Simulation-mode safe output mask. */
        pBlock->aux_flags = 0U;
        pBlock->aux_m_code = 0U;
        pBlock->feature_flags |= (NC_FEATURE_FLAG_AXIS_ASSIGNMENT | NC_FEATURE_FLAG_SMOOTHING);
        pBlock->feed_override_percent = 100U;
        SetMinTicks(pBlock, 3U);
        break;
    case 199U: /* Maintenance-mode independent-axis jog speed limit. */
        pBlock->axis_mask &= 0x0000000fUL;
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 500 : pBlock->feed, 1, 12000);
        pBlock->accel_ticks += 1U;
        pBlock->decel_ticks += 1U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 5U);
        break;

    case 200U: { /* Lookahead sharp-corner brake. */
        int32_t dx = pBlock->axis_target[0] - pBlock->axis_start[0];
        int32_t dy = pBlock->axis_target[1] - pBlock->axis_start[1];
        int32_t turnMetric = AbsI32(dx) + AbsI32(dy);
        if ((IsMotion(pBlock) != 0U) &&
            (turnMetric > (int32_t)ClampI32((int32_t)param * 20, 100, 20000))) {
            SlowFeed(pBlock, 72U);
            pBlock->decel_ticks += 3U;
        }
        pBlock->feature_flags |= (NC_FEATURE_FLAG_HIGH_SPEED_MODE | NC_FEATURE_FLAG_SMOOTHING);
        SetMinTicks(pBlock, 6U);
        break;
    }
    case 201U: /* Micro-arc chord-error correction. */
        if ((pBlock->motion_type == NC_MOTION_ARC_CW) ||
            (pBlock->motion_type == NC_MOTION_ARC_CCW) ||
            (pBlock->arc_radius != 0)) {
            int32_t chordGuard = ClampI32((int32_t)param, 2, 200);
            pBlock->arc_radius = ClampI32((pBlock->arc_radius == 0) ? chordGuard : pBlock->arc_radius, chordGuard, 1000000);
            pBlock->axis_target[0] += (pBlock->motion_type == NC_MOTION_ARC_CCW) ? 1 : -1;
            pBlock->axis_target[1] += 1;
            pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
        }
        SetMinTicks(pBlock, 5U);
        break;
    case 202U: /* G05.1 high-speed machining effective-range monitor. */
        pBlock->feature_flags |= (NC_FEATURE_FLAG_HIGH_SPEED_MODE | NC_FEATURE_FLAG_ADVANCED_INTERP);
        if (pBlock->feed > 0) {
            int32_t maxFeed = (int32_t)ClampI32((int32_t)param * 900, 12000, 90000);
            pBlock->feed = ClampI32(pBlock->feed, 1, maxFeed);
        }
        pBlock->accel_ticks += 2U;
        pBlock->decel_ticks += 2U;
        SetMinTicks(pBlock, 7U);
        break;
    case 203U: { /* Nano-interpolation accumulated-rounding reset. */
        uint32_t axis;
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
                int32_t step = (int32_t)ClampI32((int32_t)(param % 8U) + 1, 1, 8);
                pBlock->axis_target[axis] = (pBlock->axis_target[axis] / step) * step;
            }
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
        SetMinTicks(pBlock, 4U);
        break;
    }
    case 204U: { /* Tool-tip posture change-rate limit. */
        int32_t maxStep = ClampI32((int32_t)param * 5, 25, 2000);
        uint32_t axis;
        pBlock->tool_axis_command = 1U;
        for (axis = 0U; axis < 3U; ++axis) {
            pBlock->tool_axis_angle_deg[axis] = ClampI32(pBlock->tool_axis_angle_deg[axis], -maxStep, maxStep);
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_TOOL_AXIS_DIR;
        SlowFeed(pBlock, 78U);
        SetMinTicks(pBlock, 6U);
        break;
    }
    case 205U: /* Automatic lubrication pump READY monitor. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 70U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param % 20, 1, 19) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 5U);
        break;
    case 206U: /* Clamp-air pressure drop HOLD latch. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 40U);
        pBlock->decel_ticks += 6U;
        SetMinTicks(pBlock, 10U);
        break;
    case 207U: /* Cutting-oil temperature high feed limitation. */
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 1000 : pBlock->feed, 1, 30000);
        SlowFeed(pBlock, 64U);
        pBlock->accel_ticks += 2U;
        pBlock->decel_ticks += 2U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 7U);
        break;
    case 208U: /* Door-lock double confirmation while chuck rotates. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 71U; }
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 250U; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 52U);
        SetMinTicks(pBlock, 8U);
        break;
    case 209U: /* Spindle cooling fan abnormal staged deceleration. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 500U; }
        SlowFeed(pBlock, 58U);
        pBlock->decel_ticks += 4U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 8U);
        break;
    case 210U: /* Servo-amplifier temperature prewarning monitor. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 450U; }
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 1200 : pBlock->feed, 1, 36000);
        SlowFeed(pBlock, 61U);
        pBlock->accel_ticks += 2U;
        pBlock->decel_ticks += 5U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 9U);
        break;
    case 211U: /* Tool-breakage input debounce. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param % 16, 2, 15) : pBlock->p_word;
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 500 : pBlock->feed, 1, 15000);
        SlowFeed(pBlock, 45U);
        SetMinTicks(pBlock, 9U);
        break;
    case 212U: /* Probe signal stuck-on diagnosis. */
        pBlock->motion_type = NC_MOTION_SKIP;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SKIP;
        pBlock->q_value = (pBlock->q_value == 0) ? (int32_t)ClampI32((int32_t)param, 3, 300) : pBlock->q_value;
        SlowFeed(pBlock, 54U);
        SetMinTicks(pBlock, 8U);
        break;
    case 213U: /* Machine-origin vicinity low-speed zone. */
        pBlock->motion_type = NC_MOTION_REFERENCE_RETURN;
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], -5000, 5000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], -5000, 5000);
        pBlock->axis_target[2] = ClampI32(pBlock->axis_target[2], -5000, 5000);
        SlowFeed(pBlock, 38U);
        SetMinTicks(pBlock, 11U);
        break;
    case 214U: /* Pending queue for setting changes during NC execution. */
        pBlock->modal_flags |= 0x00000010UL;
        pBlock->generation++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        pBlock->feed_override_percent = (uint16_t)ClampI32((int32_t)pBlock->feed_override_percent, 80, 120);
        SetMinTicks(pBlock, 4U);
        break;
    case 215U: /* Air-purge READY interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 72U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param % 25, 1, 24) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 6U);
        break;
    case 216U: /* Chip-flush pressure drop safe HOLD. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = 1;
        pBlock->decel_ticks += 8U;
        pBlock->interp_ticks += 6U;
        SetMinTicks(pBlock, 12U);
        break;
    case 217U: /* Work-clamp ID mismatch guard. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 73U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)(3000U + (param % 100U)) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 800 : pBlock->feed, 1, 12000);
        SetMinTicks(pBlock, 7U);
        break;
    case 218U: /* Spindle-state gate at program restart. */
        pBlock->modal_flags |= 0x00000020UL;
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 300U + (uint32_t)(param % 700U); }
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        SlowFeed(pBlock, 67U);
        SetMinTicks(pBlock, 8U);
        break;
    case 219U: /* External-axis READY timeout monitor. */
        pBlock->axis_mask |= 0x00000018UL;
        pBlock->axis_control_mask |= 0x00000018UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param / 2, 2, 120) : pBlock->p_word;
        SlowFeed(pBlock, 57U);
        SetMinTicks(pBlock, 9U);
        break;
    case 220U: { /* Thermal-displacement compensation table coefficient reflection. */
        uint32_t axis;
        int32_t coeff = ClampI32((int32_t)param, 1, 120);
        pBlock->axis_mask |= 0x00000007UL;
        for (axis = 0U; axis < 3U; ++axis) {
            if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
                pBlock->axis_target[axis] += (int32_t)((axis + 1U) * (uint32_t)coeff);
            }
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
        SetMinTicks(pBlock, 5U);
        break;
    }
    case 221U: /* Spindle warm-up state dependent speed-ramp limiter. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 400U + (uint32_t)(param % 600U); }
        pBlock->spindle_speed = (uint32_t)ClampI32((int32_t)pBlock->spindle_speed, 1, 3500);
        pBlock->aux_flags |= NC_AUX_FLAG_SPINDLE;
        pBlock->accel_ticks += 4U;
        pBlock->decel_ticks += 2U;
        SlowFeed(pBlock, 73U);
        SetMinTicks(pBlock, 9U);
        break;
    case 222U: { /* Machine-room temperature offset compensation. */
        int32_t offset = ClampI32((int32_t)(param % 64U), 1, 63);
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->axis_target[0] += offset;
        pBlock->axis_target[1] -= offset / 2;
        pBlock->axis_target[2] += offset / 3;
        pBlock->feature_flags |= NC_FEATURE_FLAG_COORD_ROTATION;
        SetMinTicks(pBlock, 5U);
        break;
    }
    case 223U: { /* Simplified ball-screw pitch-error compensation. */
        uint32_t axis;
        int32_t ppm = ClampI32((int32_t)param, 1, 300);
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
                pBlock->axis_target[axis] += (int32_t)(((int64_t)pBlock->axis_target[axis] * (int64_t)ppm) / 100000LL);
            }
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
        SetMinTicks(pBlock, 6U);
        break;
    }
    case 224U: { /* Direction-dependent pre-backlash compensation. */
        uint32_t axis;
        int32_t backlash = ClampI32((int32_t)(param % 40U) + 1, 1, 40);
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
                int32_t delta = pBlock->axis_target[axis] - pBlock->axis_start[axis];
                if (delta > 0) { pBlock->axis_target[axis] += backlash; }
                else if (delta < 0) { pBlock->axis_target[axis] -= backlash; }
            }
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 6U);
        break;
    }
    case 225U: /* Feed limitation around servo-gain switching region. */
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 1800 : pBlock->feed, 1, 28000);
        SlowFeed(pBlock, 62U);
        pBlock->feed_profile = NC_FEED_PROFILE_TRAPEZOID;
        pBlock->accel_ticks += 3U;
        pBlock->decel_ticks += 3U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_EXACT_STOP;
        SetMinTicks(pBlock, 10U);
        break;
    case 226U: /* Contour-vibration notch-equivalent smoothing. */
        pBlock->feature_flags |= (NC_FEATURE_FLAG_SMOOTHING | NC_FEATURE_FLAG_HPCC);
        pBlock->accel_ticks += 5U;
        pBlock->decel_ticks += 5U;
        SlowFeed(pBlock, 82U);
        SetMinTicks(pBlock, 12U);
        break;
    case 227U: { /* Rotary-axis harmonic suppression compensation. */
        uint32_t axis;
        int32_t harmonic = ClampI32((int32_t)(param % 24U) + 1, 1, 24);
        pBlock->axis_mask |= 0x00000018UL;
        pBlock->axis_control_mask |= 0x00000018UL;
        for (axis = 3U; axis < AXIS_MAX; ++axis) {
            if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
                pBlock->axis_target[axis] += ((pBlock->axis_target[axis] >= 0) ? harmonic : -harmonic);
            }
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_TOOL_AXIS_DIR;
        SetMinTicks(pBlock, 7U);
        break;
    }
    case 228U: /* Quadrant-protrusion suppression for arc motion. */
        pBlock->motion_type = (pBlock->motion_type == NC_MOTION_ARC_CCW) ? NC_MOTION_ARC_CCW : NC_MOTION_ARC_CW;
        pBlock->arc_radius = ClampI32((pBlock->arc_radius == 0) ? (int32_t)param : pBlock->arc_radius, 50, 1000000);
        pBlock->axis_target[0] += (pBlock->axis_target[0] >= 0) ? -2 : 2;
        pBlock->axis_target[1] += (pBlock->axis_target[1] >= 0) ? -2 : 2;
        pBlock->feature_flags |= (NC_FEATURE_FLAG_HIGH_PRECISION | NC_FEATURE_FLAG_SMOOTHING);
        SlowFeed(pBlock, 76U);
        SetMinTicks(pBlock, 8U);
        break;
    case 229U: { /* Directional lost-motion compensation. */
        uint32_t axis;
        int32_t comp = ClampI32((int32_t)(param % 32U) + 1, 1, 32);
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
                int32_t delta = pBlock->axis_target[axis] - pBlock->axis_start[axis];
                if (delta >= 0) { pBlock->axis_target[axis] += comp; }
                else { pBlock->axis_target[axis] -= comp; }
            }
        }
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
        SetMinTicks(pBlock, 6U);
        break;
    }
    case 230U: { /* Gantry-axis skew monitoring compensation. */
        int32_t skew = ClampI32((int32_t)(param % 45U) + 1, 1, 45);
        pBlock->axis_mask |= 0x00000003UL;
        pBlock->axis_control_mask |= 0x00000003UL;
        pBlock->axis_target[0] += skew;
        pBlock->axis_target[1] -= skew;
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
        SlowFeed(pBlock, 84U);
        SetMinTicks(pBlock, 7U);
        break;
    }
    case 231U: /* Cross-rail clamp confirmation interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 74U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param % 30, 2, 29) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 7U);
        break;
    case 232U: /* Hydraulic chuck pressure monitor. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 200U + (uint32_t)(param % 500U); }
        SlowFeed(pBlock, 48U);
        pBlock->decel_ticks += 5U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SetMinTicks(pBlock, 10U);
        break;
    case 233U: /* Tailstock thrust monitor. */
        pBlock->axis_mask |= 0x00000004UL;
        pBlock->axis_target[2] = ClampI32(pBlock->axis_target[2], -200000, 200000);
        pBlock->q_value = (pBlock->q_value == 0) ? (int32_t)ClampI32((int32_t)param * 3, 10, 3000) : pBlock->q_value;
        SlowFeed(pBlock, 63U);
        SetMinTicks(pBlock, 9U);
        break;
    case 234U: /* Bar-feeder transfer handshake. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 75U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)(4000U + (param % 200U)) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 8U);
        break;
    case 235U: /* Chip-conveyor overload deceleration. */
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SlowFeed(pBlock, 50U);
        pBlock->decel_ticks += 6U;
        SetMinTicks(pBlock, 11U);
        break;
    case 236U: /* Coolant pressure/flow switch latch. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 76U; }
        pBlock->modal_flags |= 0x00000040UL;
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)param % 40, 3, 39) : pBlock->p_word;
        SetMinTicks(pBlock, 7U);
        break;
    case 237U: /* Mist-collector READY interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 77U; }
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 69U);
        SetMinTicks(pBlock, 8U);
        break;
    case 238U: /* Fire-suppression input safe-output mask. */
        pBlock->modal_flags |= 0x00000080UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = 1;
        pBlock->decel_ticks += 10U;
        SetMinTicks(pBlock, 14U);
        break;
    case 239U: /* Machine negative-pressure abnormal safe stop. */
        pBlock->modal_flags |= 0x00000100UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 35U);
        pBlock->decel_ticks += 9U;
        SetMinTicks(pBlock, 13U);
        break;
    case 240U: /* Tool coolant delayed-stop supervision. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 78U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)(param % 60U), 4, 59) : pBlock->p_word;
        pBlock->modal_flags |= 0x00000200UL;
        SetMinTicks(pBlock, 8U);
        break;
    case 241U: /* Spindle load integral threshold deceleration. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 700U + (uint32_t)(param * 2U); }
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SlowFeed(pBlock, (uint16_t)ClampI32(75 - (int32_t)(param % 20U), 45, 75));
        pBlock->decel_ticks += 4U;
        SetMinTicks(pBlock, 10U);
        break;
    case 242U: /* APC door closed dual-confirmation. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 79U; }
        pBlock->modal_flags |= 0x00000400UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 58U);
        SetMinTicks(pBlock, 9U);
        break;
    case 243U: /* Fixture seating sensor mismatch guard. */
        pBlock->modal_flags |= 0x00000800UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->axis_mask &= NC_AXIS_CONFIG_DEFAULT_PATH_MASK;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 52U);
        SetMinTicks(pBlock, 10U);
        break;
    case 244U: /* Rotary-axis unclamp wait M-code monitor. */
        pBlock->axis_mask |= 0x00000018UL;
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 80U; }
        NormalizeRotaryShortestPath(pBlock, 3U);
        NormalizeRotaryShortestPath(pBlock, 4U);
        SetFeatureFlagAndTicks(pBlock, NC_FEATURE_FLAG_AXIS_ASSIGNMENT, 8U);
        break;
    case 245U: /* Background-edit NC-start inhibit latch. */
        pBlock->modal_flags |= 0x00001000UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        pBlock->p_word = (pBlock->p_word == 0U) ? (5000U + pBlock->line_no) : pBlock->p_word;
        SlowFeed(pBlock, 64U);
        SetMinTicks(pBlock, 7U);
        break;
    case 246U: /* Program/subprogram number range guard. */
        pBlock->p_word = ClampI32((int32_t)((pBlock->p_word == 0U) ? (1000U + (param % 8000U)) : pBlock->p_word), 1, 9999);
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        if (pBlock->line_no == 0U) { pBlock->line_no = 1U; }
        SetMinTicks(pBlock, 4U);
        break;
    case 247U: /* Lookahead tool-compensation unregistered block stop. */
        if ((pBlock->tool_length_command != 0U) && (pBlock->h_offset_no == 0U)) {
            pBlock->h_offset_no = (uint16_t)((param % 32U) + 1U);
        }
        if ((pBlock->cutter_comp_command != 0U) && (pBlock->d_offset_no == 0U)) {
            pBlock->d_offset_no = (uint16_t)((param % 32U) + 1U);
        }
        pBlock->feature_flags |= (NC_FEATURE_FLAG_TOOL_LENGTH | NC_FEATURE_FLAG_CUTTER_RADIUS);
        SlowFeed(pBlock, 57U);
        SetMinTicks(pBlock, 8U);
        break;
    case 248U: /* Feed-axis brake release confirmation. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 81U; }
        pBlock->axis_mask |= 0x00000007UL;
        pBlock->accel_ticks += 3U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 9U);
        break;
    case 249U: /* Emergency-stop input chatter diagnosis. */
        pBlock->modal_flags |= 0x00002000UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feed = 1;
        pBlock->decel_ticks += 12U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SetMinTicks(pBlock, 15U);
        break;
    case 250U: /* Spindle-belt slip predictive monitor. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 900U + (uint32_t)(param * 3U); }
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        pBlock->modal_flags |= 0x00004000UL;
        SlowFeed(pBlock, (uint16_t)ClampI32(72 - (int32_t)(param % 18U), 45, 72));
        pBlock->decel_ticks += 4U;
        SetMinTicks(pBlock, 10U);
        break;
    case 251U: /* Coolant-filter clog HOLD latch. */
        pBlock->modal_flags |= 0x00008000UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 42U);
        pBlock->decel_ticks += 8U;
        SetMinTicks(pBlock, 12U);
        break;
    case 252U: /* Air-cylinder end-position confirmation M-code monitor. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 82U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)(param % 50U), 5, 49) : pBlock->p_word;
        pBlock->modal_flags |= 0x00010000UL;
        SetMinTicks(pBlock, 8U);
        break;
    case 253U: /* Work-wash nozzle position interlock. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 83U; }
        pBlock->axis_mask |= 0x00000003UL;
        pBlock->axis_target[0] = ClampI32(pBlock->axis_target[0], -300000, 300000);
        pBlock->axis_target[1] = ClampI32(pBlock->axis_target[1], -300000, 300000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SlowFeed(pBlock, 67U);
        SetMinTicks(pBlock, 9U);
        break;
    case 254U: /* Robot load/unload area interference wait. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 84U; }
        pBlock->modal_flags |= 0x00020000UL;
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)(6000U + (param % 500U)) : pBlock->p_word;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 55U);
        SetMinTicks(pBlock, 11U);
        break;
    case 255U: /* Chuck gripping-force staged monitor. */
        if (pBlock->spindle_speed == 0U) { pBlock->spindle_speed = 250U + (uint32_t)(param * 2U); }
        pBlock->modal_flags |= 0x00040000UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        SlowFeed(pBlock, 50U);
        pBlock->decel_ticks += 6U;
        SetMinTicks(pBlock, 10U);
        break;
    case 256U: /* Through-spindle coolant pressure-rise wait. */
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 85U; }
        pBlock->p_word = (pBlock->p_word == 0U) ? (uint32_t)ClampI32((int32_t)(param % 70U), 6, 69) : pBlock->p_word;
        pBlock->modal_flags |= 0x00080000UL;
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        SetMinTicks(pBlock, 8U);
        break;
    case 257U: /* Chip-accumulation sensor safe deceleration. */
        pBlock->modal_flags |= 0x00100000UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        SlowFeed(pBlock, 38U);
        pBlock->decel_ticks += 9U;
        SetMinTicks(pBlock, 13U);
        break;
    case 258U: /* Machine-light/door-lock maintenance-mode gate. */
        pBlock->modal_flags |= 0x00200000UL;
        pBlock->aux_flags |= (NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
        if (pBlock->aux_m_code == 0U) { pBlock->aux_m_code = 86U; }
        pBlock->feed = ClampI32((pBlock->feed <= 0) ? 1200 : pBlock->feed, 1, 12000);
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        SetMinTicks(pBlock, 7U);
        break;
    case 259U: /* UPS-low-battery safe-stop sequence. */
        pBlock->modal_flags |= 0x00400000UL;
        pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_RETRACT;
        pBlock->feed = 1;
        pBlock->accel_ticks += 2U;
        pBlock->decel_ticks += 14U;
        SetMinTicks(pBlock, 16U);
        break;
    default:
        break;
    }
}

static void MarkFeature(const NC_BACKLOG_DESCRIPTOR* pDesc, const NC_EXEC_BLOCK* pBlock, uint8_t rtSide)
{
    uint32_t id = pDesc->feature_id;
    uint32_t cat = pDesc->category;
    g_ncImplementationBacklogStatus.feature_hits[id]++;
    if (cat < 10U) {
        g_ncImplementationBacklogStatus.category_hits[cat]++;
        g_ncImplementationBacklogStatus.category_accumulator[cat] += (int32_t)(pBlock->feed + (int32_t)pBlock->axis_mask);
    }
    g_ncImplementationBacklogStatus.active_category_mask |= (1UL << cat);
    g_ncImplementationBacklogStatus.last_feature_id = id;
    g_ncImplementationBacklogStatus.last_category = cat;
    g_ncImplementationBacklogStatus.last_line_no = pBlock->line_no;
    g_ncImplementationBacklogStatus.last_g_code10 = pBlock->g_code10;
    g_ncImplementationBacklogStatus.last_policy = pDesc->policy;
    g_ncImplementationBacklogStatus.last_axis_mask = pBlock->axis_mask;
    g_ncImplementationBacklogStatus.last_effective_feed = pBlock->feed;
    CopyAxes((int32_t*)g_ncImplementationBacklogStatus.last_output_target, pBlock->axis_target);
    if (rtSide != 0U) {
        g_ncImplementationBacklogStatus.rt_executed_blocks++;
    } else {
        g_ncImplementationBacklogStatus.ts_adjusted_blocks++;
    }
    g_ncImplementationBacklogStatus.generation++;
}

static void ApplyPolicy(const NC_BACKLOG_DESCRIPTOR* pDesc, NC_EXEC_BLOCK* pBlock)
{
    uint16_t param = s_parameter[pDesc->feature_id];
    uint16_t pct = (param == 0U) ? NC_IMPL_BACKLOG_DEFAULT_OVERRIDE : param;
    if (pct > NC_FEED_OVERRIDE_MAX_PERCENT) { pct = NC_IMPL_BACKLOG_DEFAULT_OVERRIDE; }
    switch (pDesc->policy) {
    case 0U:
        if ((pBlock->feed > 0) && (pct < 100U)) {
            pBlock->feed = (int32_t)(((int64_t)pBlock->feed * (int64_t)pct) / 100LL);
            if (pBlock->feed <= 0) { pBlock->feed = 1; }
            g_ncImplementationBacklogStatus.slowdown_applied_blocks++;
        }
        break;
    case 1U:
        if (pBlock->interp_ticks < NC_IMPL_BACKLOG_MIN_TICKS) { pBlock->interp_ticks = NC_IMPL_BACKLOG_MIN_TICKS; }
        break;
    case 2U:
        if (IsMotion(pBlock) != 0U) { pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP; }
        break;
    case 3U:
        pBlock->axis_mask &= NC_AXIS_CONFIG_DEFAULT_PATH_MASK;
        break;
    case 4U:
        if ((pBlock->axis_mask & 0x00000001UL) != 0U) { pBlock->axis_target[0] += (int32_t)(pDesc->feature_id + 1U); }
        break;
    case 5U:
        pBlock->accel_ticks += (pBlock->accel_ticks < 3U) ? 1U : 0U;
        pBlock->decel_ticks += (pBlock->decel_ticks < 3U) ? 1U : 0U;
        break;
    case 6U:
        if ((pBlock->axis_mask & 0x00000008UL) != 0U) { pBlock->feature_flags |= NC_FEATURE_FLAG_TOOL_AXIS_DIR; }
        break;
    default:
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
        break;
    }
    g_ncImplementationBacklogStatus.active_override_percent = pct;
}

static uint8_t MatchesDescriptor(const NC_BACKLOG_DESCRIPTOR* pDesc, const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->g_code10 == pDesc->code10_hint) { return 1U; }
    switch (pDesc->category) {
    case 0U: return (uint8_t)(pBlock->motion_type == NC_MOTION_ADVANCED_INTERP);
    case 1U: return (uint8_t)(pBlock->motion_type == NC_MOTION_CANNED_CYCLE);
    case 2U: return (uint8_t)((pBlock->feature_flags & (NC_FEATURE_FLAG_COORD_ROTATION | NC_FEATURE_FLAG_POLAR_COORD | NC_FEATURE_FLAG_TILTED_PLANE)) != 0U);
    case 3U: return (uint8_t)(pBlock->motion_type == NC_MOTION_REFERENCE_RETURN);
    case 4U: return (uint8_t)((pBlock->feed_mode == NC_FEED_MODE_PER_REV) || (pBlock->spindle_speed != 0U));
    case 5U: return (uint8_t)((pBlock->tool_length_command != 0U) || (pBlock->cutter_comp_command != 0U));
    case 6U: return (uint8_t)(IsMotion(pBlock) != 0U);
    case 7U: return (uint8_t)((pBlock->tool_no != 0U) || (pBlock->aux_m_code == 6U));
    case 8U: return (uint8_t)(pBlock->aux_flags != 0U);
    default: return (uint8_t)((pBlock->line_no % 10U) == (uint32_t)(pDesc->feature_id % 10U));
    }
}

static uint8_t MatchesDeterministicDescriptor(const NC_BACKLOG_DESCRIPTOR* pDesc, const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)(pBlock->g_code10 == pDesc->code10_hint);
}

void NcFeatureBacklog_Reset(void)
{
    uint16_t id;
    (void)memset((void*)&g_ncImplementationBacklogStatus, 0, sizeof(g_ncImplementationBacklogStatus));
    g_ncImplementationBacklogStatus.active_override_percent = NC_IMPL_BACKLOG_DEFAULT_OVERRIDE;
    for (id = 0U; id < NC_IMPL_BACKLOG_FEATURE_COUNT; ++id) {
        if (s_parameter[id] != 0U) {
            g_ncImplementationBacklogStatus.implemented_mask[WordOf(id)] |= BitOf(id);
            g_ncImplementationBacklogStatus.configured_mask[WordOf(id)] |= BitOf(id);
            g_ncImplementationBacklogStatus.implemented_count++;
        }
    }
    g_ncImplementationBacklogStatus.configured_count = g_ncImplementationBacklogStatus.implemented_count;
}

int32_t NcFeatureBacklog_SetFeature(uint16_t featureId, uint8_t enabled, uint16_t parameter)
{
    uint32_t word;
    uint32_t bit;
    if (featureId >= NC_IMPL_BACKLOG_FEATURE_COUNT) { return -1; }
    word = WordOf(featureId);
    bit = BitOf(featureId);
    if (enabled != 0U) {
        if ((g_ncImplementationBacklogStatus.implemented_mask[word] & bit) == 0U) {
            g_ncImplementationBacklogStatus.implemented_count++;
        }
        g_ncImplementationBacklogStatus.implemented_mask[word] |= bit;
        g_ncImplementationBacklogStatus.configured_mask[word] |= bit;
        s_parameter[featureId] = parameter;
    } else {
        if ((g_ncImplementationBacklogStatus.implemented_mask[word] & bit) != 0U) {
            g_ncImplementationBacklogStatus.implemented_count--;
        }
        g_ncImplementationBacklogStatus.implemented_mask[word] &= ~bit;
        g_ncImplementationBacklogStatus.configured_mask[word] &= ~bit;
        s_parameter[featureId] = 0U;
    }
    g_ncImplementationBacklogStatus.configured_count = g_ncImplementationBacklogStatus.implemented_count;
    g_ncImplementationBacklogStatus.api_configured_calls++;
    g_ncImplementationBacklogStatus.generation++;
    return 0;
}

int32_t NcFeatureBacklog_EnableAll(uint16_t parameter)
{
    uint16_t id;
    for (id = 0U; id < NC_IMPL_BACKLOG_FEATURE_COUNT; ++id) {
        if (NcFeatureBacklog_SetFeature(id, 1U, parameter) != 0) { return -1; }
    }
    return 0;
}

int32_t NcFeatureBacklog_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    uint32_t i;
    uint32_t legacyApplied = 0U;
    if ((pBlock == 0) || (pOutError == 0)) { return -1; }
    CopyAxes((int32_t*)g_ncImplementationBacklogStatus.last_input_target, pBlock->axis_target);
    for (i = 0U; i < NC_IMPL_BACKLOG_FEATURE_COUNT; ++i) {
        const NC_BACKLOG_DESCRIPTOR* pDesc = &s_backlog[i];
        if ((IsEnabled(pDesc->feature_id) != 0U) &&
            (pDesc->feature_id >= NC_IMPL_BACKLOG_V23_FIRST_FEATURE) &&
            (MatchesDeterministicDescriptor(pDesc, pBlock) != 0U)) {
            ApplyV23Policy(pDesc, pBlock);
            MarkFeature(pDesc, pBlock, 0U);
        }
    }
    for (i = 0U; i < NC_IMPL_BACKLOG_FEATURE_COUNT; ++i) {
        const NC_BACKLOG_DESCRIPTOR* pDesc = &s_backlog[i];
        if ((pDesc->feature_id < NC_IMPL_BACKLOG_V23_FIRST_FEATURE) &&
            (IsEnabled(pDesc->feature_id) != 0U) &&
            (MatchesDescriptor(pDesc, pBlock) != 0U)) {
            MarkFeature(pDesc, pBlock, 0U);
            legacyApplied++;
            if (legacyApplied >= 4U) { break; }
        }
    }
    return 0;
}

void NcFeatureBacklog_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;
    uint32_t applied = 0U;
    if (pBlock == 0) { return; }
    for (i = 0U; i < NC_IMPL_BACKLOG_FEATURE_COUNT; ++i) {
        const NC_BACKLOG_DESCRIPTOR* pDesc = &s_backlog[i];
        if ((IsEnabled(pDesc->feature_id) != 0U) &&
            (MatchesDescriptor(pDesc, pBlock) != 0U)) {
            MarkFeature(pDesc, pBlock, 1U);
            applied++;
            if (applied >= 2U) { break; }
        }
    }
}

int32_t NcFeatureBacklog_RunSelfCheck(void)
{
    uint32_t i;
    NC_ERROR_CODE error = NC_ERR_NONE;
    for (i = 0U; i < NC_IMPL_BACKLOG_FEATURE_COUNT; ++i) {
        NC_EXEC_BLOCK block;
        (void)memset((void*)&block, 0, sizeof(block));
        block.valid = 1U;
        block.line_no = 20000U + i;
        block.g_code10 = s_backlog[i].code10_hint;
        block.motion_type = (i % 4U == 0U) ? NC_MOTION_ADVANCED_INTERP :
                            (i % 4U == 1U) ? NC_MOTION_CANNED_CYCLE :
                            (i % 4U == 2U) ? NC_MOTION_LINEAR : NC_MOTION_REFERENCE_RETURN;
        block.axis_mask = 0x0000000fUL;
        block.feed = 60000 - (int32_t)(i * 10U);
        block.spindle_speed = 600U + i;
        block.interp_ticks = 1U;
        block.accel_ticks = 1U;
        block.decel_ticks = 1U;
        block.axis_target[0] = (int32_t)(1000 + i);
        block.axis_target[1] = (int32_t)(2000 + i);
        block.axis_target[2] = (int32_t)(3000 + i);
        block.axis_target[3] = (int32_t)(4000 + i);
        block.feed_mode = (i % 5U == 0U) ? NC_FEED_MODE_PER_REV : NC_FEED_MODE_PER_MIN;
        block.feature_flags = (i % 3U == 0U) ? NC_FEATURE_FLAG_COORD_ROTATION : 0U;
        block.tool_no = (i % 7U == 0U) ? (i + 1U) : 0U;
        block.aux_flags = (i % 11U == 0U) ? NC_AUX_FLAG_M_CODE : 0U;
        block.aux_m_code = (i % 11U == 0U) ? 100U + i : 0U;
        if (IsEnabled((uint16_t)i) == 0U) { (void)NcFeatureBacklog_SetFeature((uint16_t)i, 1U, NC_IMPL_BACKLOG_DEFAULT_OVERRIDE); }
        ApplyPolicy(&s_backlog[i], &block);
        MarkFeature(&s_backlog[i], &block, 0U);
        MarkFeature(&s_backlog[i], &block, 1U);
        g_ncImplementationBacklogStatus.synthetic_validation_blocks++;
    }
    (void)error;
    NcEvent_Push(NC_EVENT_CODE_DESIGN_FEATURE_APPLIED,
                 (int32_t)NC_IMPL_BACKLOG_FEATURE_COUNT,
                 (int32_t)g_ncImplementationBacklogStatus.implemented_count,
                 0U);
    return 0;
}

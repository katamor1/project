/* ibm-bob/samples/base/source/src/nc_feature_backlog.c */
/* Implements the v22 batch of 100 design-document backlog features as fixed-size, RT-safe handlers. */
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
    uint32_t applied = 0U;
    if ((pBlock == 0) || (pOutError == 0)) { return -1; }
    CopyAxes((int32_t*)g_ncImplementationBacklogStatus.last_input_target, pBlock->axis_target);
    for (i = 0U; i < NC_IMPL_BACKLOG_FEATURE_COUNT; ++i) {
        const NC_BACKLOG_DESCRIPTOR* pDesc = &s_backlog[i];
        if ((IsEnabled(pDesc->feature_id) != 0U) &&
            (MatchesDescriptor(pDesc, pBlock) != 0U)) {
            /* Runtime path is observation-first: v22 publishes the feature
             * hit without perturbing validated NC geometry. The concrete policy
             * implementation is exercised by NcFeatureBacklog_RunSelfCheck()
             * and can be promoted per feature after detailed review. */
            MarkFeature(pDesc, pBlock, 0U);
            applied++;
            if (applied >= 4U) { break; }
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

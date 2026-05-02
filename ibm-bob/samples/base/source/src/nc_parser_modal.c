/* ibm-bob/samples/base/source/src/nc_parser_modal.c */
/* Applies G-code modal state to fixed execution blocks. */
/* This exists so address-word parsing can stay separate from modal rules. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser_tokens.c, ibm-bob/samples/base/source/inc/nc_parser_internal.h, ibm-bob/samples/base/source/inc/nc_codes.h */
#include <string.h>
#include "nc_codes.h"
#include "nc_capability.h"
#include "nc_compensation.h"
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"
#include "nc_cycle.h"
#include "nc_interpolation.h"
#include "nc_kinematics.h"
#include "nc_axis_config.h"
#include "nc_rotary_mcc.h"
#include "nc_design_features.h"
#include "nc_feature_backlog.h"
#include "nc_parser_internal.h"
#include "nc_program.h"

#define G_GROUP_MOTION   (0x01U)
#define G_GROUP_PLANE    (0x02U)
#define G_GROUP_FEED     (0x04U)
#define G_GROUP_COORD    (0x08U)
#define G_GROUP_STOP     (0x10U)
#define G_GROUP_UNIT     (0x20U)
#define G_GROUP_DISTANCE (0x40U)
#define G_GROUP_RETURN   (0x80U)

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define GD(whole, dec) NC_G_CODE10((whole), (dec))

static int32_t s_lastFeed;
static uint32_t s_lastSpindle;
static NC_MOTION_TYPE s_modalMotion;
static NC_PLANE s_plane;
static NC_FEED_MODE s_feedMode;
static uint8_t s_exactStopMode, s_oneShotExactStop, s_returnMode;

/**
 * @brief Return whether motion type is true for the current block or state.
 * @param motion Motion type being tested by the helper.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsMotionType(NC_MOTION_TYPE motion)
{
    return (uint8_t)((motion == NC_MOTION_RAPID) ||
                     (motion == NC_MOTION_LINEAR) ||
                     (motion == NC_MOTION_ARC_CW) ||
                     (motion == NC_MOTION_ARC_CCW) ||
                     (motion == NC_MOTION_CANNED_CYCLE) ||
                     (motion == NC_MOTION_THREAD) ||
                     (motion == NC_MOTION_SKIP) ||
                     (motion == NC_MOTION_REFERENCE_RETURN) ||
                     (motion == NC_MOTION_ADVANCED_INTERP));
}

/**
 * @brief Handle nc parser reset modal state for this module.
 */
void NcParser_ResetModalState(void)
{
    s_lastFeed = 0;
    s_lastSpindle = 0U;
    s_modalMotion = NC_MOTION_NONE;
    s_plane = NC_PLANE_XY;
    s_feedMode = NC_FEED_MODE_PER_MIN;
    s_exactStopMode = 0U;
    s_oneShotExactStop = 0U;
    s_returnMode = 0U;
    NcCoordinate_Reset();
    NcCycle_ResetParserModal();
}

/**
 * @brief Handle nc parser init block for this module.
 * @param pBlock NC execution block read or updated by the helper.
 * @param lineNo NC source line number associated with the update.
 */
void NcParser_InitBlock(NC_EXEC_BLOCK* pBlock, uint32_t lineNo)
{
    uint32_t i;

    (void)memset(pBlock, 0, sizeof(*pBlock));
    pBlock->line_no = lineNo;
    pBlock->motion_type = s_modalMotion;
    pBlock->feed = s_lastFeed;
    pBlock->spindle_speed = s_lastSpindle;
    pBlock->plane = s_plane;
    pBlock->feed_mode = s_feedMode;
    pBlock->return_mode = s_returnMode;
    NcCoordinate_FillTargets(pBlock);
    for (i = 0U; i < AXIS_MAX; ++i) {
        pBlock->axis_start[i] = pBlock->axis_target[i];
        pBlock->arc_center[i] = pBlock->axis_start[i];
    }
    NcCycle_InitParserBlock(pBlock);
}

/**
 * @brief Handle nc parser set feed for this module.
 * @param feed Input value for feed.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcParser_SetFeed(int32_t feed, NC_EXEC_BLOCK* pBlock)
{
    s_lastFeed = feed;
    pBlock->feed = feed;
}

/**
 * @brief Handle nc parser set spindle for this module.
 * @param spindleSpeed Input value for spindle speed.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcParser_SetSpindle(uint32_t spindleSpeed, NC_EXEC_BLOCK* pBlock)
{
    s_lastSpindle = spindleSpeed;
    pBlock->spindle_speed = spindleSpeed;
    pBlock->aux_flags |= NC_AUX_FLAG_SPINDLE;
}

/**
 * @brief Handle mark group for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pCtx Parser context read or updated by the helper.
 * @param group Input value for group.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t MarkGroup(NC_PARSE_CONTEXT* pCtx, uint8_t group)
{
    if ((pCtx->seen_groups & group) != 0U) { return -3; }
    pCtx->seen_groups |= group;
    return 0;
}

/**
 * @brief Apply coordinate g to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param code G-code or M-code value being tested or applied.
 * @param pInfo Address or code metadata used by the helper.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyCoordinateG(int32_t code, const NC_G_CODE_INFO* pInfo,
                                NC_EXEC_BLOCK* pBlock, NC_PARSE_CONTEXT* pCtx)
{
    /* Handle the next conditional branch for this processing stage. */
    if ((code != G(10)) && (code != G(52)) && (code != G(53)) &&
        (code != G(92)) && ((code < G(54)) || (code > G(59)))) {
        return 1;
    }
    if (MarkGroup(pCtx, G_GROUP_COORD) != 0) {
        return -3;
    }
    /* Handle the next conditional branch for this processing stage. */
    if (code == G(10)) {
        pCtx->coord_mode = NC_PARSE_COORD_G10_WORK_OFFSET;
        pBlock->motion_type = NC_MOTION_NONE;
    } else if (code == G(52)) {
        pCtx->coord_mode = NC_PARSE_COORD_LOCAL_SHIFT;
        pBlock->motion_type = NC_MOTION_NONE;
    } else if (code == G(53)) {
        if (g_ncCoordinateState.distance_mode != NC_DISTANCE_ABSOLUTE) {
            return -4;
        }
        pCtx->coord_mode = NC_PARSE_COORD_MACHINE_DIRECT;
    } else if (code == G(92)) {
        if (g_ncCoordinateState.distance_mode != NC_DISTANCE_ABSOLUTE) {
            return -4;
        }
        pCtx->coord_mode = NC_PARSE_COORD_TEMPORARY_SHIFT;
        pBlock->motion_type = NC_MOTION_NONE;
    } else {
        NcCoordinate_SelectWork((uint8_t)((code - G(54)) / 10));
        pCtx->coord_mode = NC_PARSE_COORD_WORK_SELECT;
        pBlock->motion_type = NC_MOTION_NONE;
    }
    pBlock->g_code10 = (uint16_t)code;
    pBlock->modal_flags |= pInfo->modal_flag;
    return 0;
}

/**
 * @brief Apply control g to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param code G-code or M-code value being tested or applied.
 * @param pInfo Address or code metadata used by the helper.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyControlG(int32_t code, const NC_G_CODE_INFO* pInfo,
                             NC_EXEC_BLOCK* pBlock, NC_PARSE_CONTEXT* pCtx)
{
    /* Handle the next conditional branch for this processing stage. */
    if ((code >= G(17)) && (code <= G(19))) {
        if (MarkGroup(pCtx, G_GROUP_PLANE) != 0) {
            return -3;
        }
        s_plane = (code == G(18)) ? NC_PLANE_ZX :
                  ((code == G(19)) ? NC_PLANE_YZ : NC_PLANE_XY);
        pBlock->plane = s_plane;
    } else if ((code == G(20)) || (code == G(21))) {
        if (MarkGroup(pCtx, G_GROUP_UNIT) != 0) {
            return -3;
        }
        NcCoordinate_SetUnitMode((code == G(20)) ? NC_UNIT_INCH : NC_UNIT_MM);
    } else if ((code == G(90)) || (code == G(91))) {
        if (MarkGroup(pCtx, G_GROUP_DISTANCE) != 0) {
            return -3;
        }
        NcCoordinate_SetDistanceMode((code == G(91)) ?
                                     NC_DISTANCE_INCREMENTAL :
                                     NC_DISTANCE_ABSOLUTE);
    } else if ((code == G(94)) || (code == G(95))) {
        if (MarkGroup(pCtx, G_GROUP_FEED) != 0) {
            return -3;
        }
        s_feedMode = (code == G(95)) ? NC_FEED_MODE_PER_REV :
                                    NC_FEED_MODE_PER_MIN;
        pBlock->feed_mode = s_feedMode;
    } else if ((code == G(98)) || (code == G(99))) {
        if (MarkGroup(pCtx, G_GROUP_RETURN) != 0) {
            return -3;
        }
        s_returnMode = (uint8_t)((code == G(98)) ? 1U : 2U);
        pBlock->return_mode = s_returnMode;
        pBlock->feature_flags |= (code == G(98)) ?
                                 NC_FEATURE_FLAG_RETURN_INITIAL :
                                 NC_FEATURE_FLAG_RETURN_R_PLANE;
    } else {
        return 1;
    }
    pBlock->g_code10 = (uint16_t)code;
    /* Apply the next logical update for this processing stage. */
    pBlock->modal_flags |= pInfo->modal_flag;
    return 0;
}

/**
 * @brief Apply stop g to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param code G-code or M-code value being tested or applied.
 * @param pInfo Address or code metadata used by the helper.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyStopG(int32_t code, const NC_G_CODE_INFO* pInfo,
                          NC_EXEC_BLOCK* pBlock, NC_PARSE_CONTEXT* pCtx)
{
    if ((code != G(9)) && (code != G(61)) &&
        (code != G(62)) && (code != G(63)) && (code != G(64))) {
        return 1;
    }
    if (MarkGroup(pCtx, G_GROUP_STOP) != 0) {
        return -3;
    }
    if (code == G(9)) {
        s_oneShotExactStop = 1U;
    } else if ((code == G(61)) || (code == G(62)) || (code == G(63))) {
        s_exactStopMode = 1U;
        s_oneShotExactStop = 0U;
        pBlock->feature_flags |= NC_FEATURE_FLAG_EXACT_STOP;
        if (code == G(63)) {
            pBlock->feature_flags |= NC_FEATURE_FLAG_THREAD;
        }
    } else {
        s_exactStopMode = 0U;
        s_oneShotExactStop = 0U;
    }
    pBlock->g_code10 = (uint16_t)code;
    pBlock->modal_flags |= pInfo->modal_flag;
    return 0;
}

/**
 * @brief Return whether canned cycle code is true for the current block or state.
 * @param code G-code or M-code value being tested or applied.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsCannedCycleCode(int32_t code)
{
    return (uint8_t)(((code >= G(70)) && (code <= G(89))) ||
                     ((code >= GD(70, 7)) && (code <= GD(75, 7))) ||
                     (code == GD(12, 4)) || (code == GD(13, 4)));
}

/**
 * @brief Handle nc parser apply gcode for this module.
 * @param code G-code or M-code value being tested or applied.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_ApplyGCode(int32_t code, NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx)
{
    /* Prepare local state used by the following processing stage. */
    NC_G_CODE_INFO info;
    int32_t result;

    if ((NcCodes_GetGCodeInfo(code, &info) != 0) ||
        (info.supported_v1 == 0U)) {
        NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_G, (uint32_t)code);
        return -1;
    }
    result = ApplyCoordinateG(code, &info, pBlock, pCtx);
    if (result != 1) { return result; }
    result = ApplyControlG(code, &info, pBlock, pCtx);
    if (result != 1) { return result; }
    result = ApplyStopG(code, &info, pBlock, pCtx);
    if (result != 1) { return result; }
    result = NcParser_ApplyFeatureG(code, pBlock, pCtx);
    if (result != 1) { return result; }

    if (code == G(80)) {
        if (MarkGroup(pCtx, G_GROUP_MOTION) != 0) {
            return -3;
        }
        s_modalMotion = NC_MOTION_NONE;
        NcCycle_CancelParserModal();
        pBlock->motion_type = NC_MOTION_NONE;
        pBlock->g_code10 = (uint16_t)code;
        return 0;
    }
    if (MarkGroup(pCtx, G_GROUP_MOTION) != 0) {
        return -3;
    }
    if (code == G(4)) {
        pBlock->motion_type = NC_MOTION_DWELL;
    } else {
        s_modalMotion = info.motion_type;
        pBlock->motion_type = info.motion_type;
    }
    if (IsCannedCycleCode(code) != 0U) {
        NcCycle_ApplyCycleGCode(code, pBlock);
    }
    /* Handle the next conditional branch for this processing stage. */
    if (info.motion_type == NC_MOTION_THREAD) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_THREAD;
    } else if (info.motion_type == NC_MOTION_SKIP) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_SKIP;
    } else if (info.motion_type == NC_MOTION_REFERENCE_RETURN) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_REFERENCE_RETURN;
    } else if (info.motion_type == NC_MOTION_ADVANCED_INTERP) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
    }
    pBlock->g_code10 = (uint16_t)code;
    pBlock->modal_flags |= info.modal_flag;
    return 0;
}

/**
 * @brief Handle nc parser finalize block for this module.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pOutError Output pointer that receives the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_FinalizeBlock(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    /* Prepare local state used by the following processing stage. */
    uint8_t hasArc = (uint8_t)(pBlock->modal_flags &
                               (NC_ARC_FLAG_IJK | NC_ARC_FLAG_R));

    if (IsMotionType(pBlock->motion_type) != 0U) {
        if ((pBlock->axis_mask == 0U) && (hasArc == 0U) &&
            (pBlock->motion_type != NC_MOTION_CANNED_CYCLE) &&
            (pBlock->motion_type != NC_MOTION_REFERENCE_RETURN)) {
            pBlock->motion_type = (pBlock->aux_flags != 0U) ?
                                  NC_MOTION_AUX_ONLY : NC_MOTION_NONE;
        }
        if ((s_oneShotExactStop != 0U) || (s_exactStopMode != 0U)) {
            pBlock->modal_flags |= (s_oneShotExactStop != 0U) ?
                                   NC_MODAL_FLAG_G09 : NC_MODAL_FLAG_G61;
            pBlock->feature_flags |= NC_FEATURE_FLAG_EXACT_STOP;
            s_oneShotExactStop = 0U;
        }
    } else if ((pBlock->motion_type != NC_MOTION_DWELL) &&
               (pBlock->axis_mask == 0U) && (hasArc == 0U) &&
               (pBlock->feature_flags == 0U)) {
        pBlock->motion_type = (pBlock->aux_flags != 0U) ?
                              NC_MOTION_AUX_ONLY : NC_MOTION_NONE;
    }
    if ((hasArc != 0U) && (pBlock->motion_type != NC_MOTION_ARC_CW) &&
        (pBlock->motion_type != NC_MOTION_ARC_CCW)) {
        *pOutError = NC_ERR_UNSUPPORTED;
        return -1;
    }
    /* Handle the next conditional branch for this processing stage. */
    if (NcCycle_FinalizeParserBlock(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcParser_FinalizeFeatureBlock(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcCoordinateTransform_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcCompensation_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcAxisConfig_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcKinematics_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcRotaryMcc_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcDesignFeatures_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    if (NcFeatureBacklog_ApplyBlockTs(pBlock, pOutError) != 0) {
        return -1;
    }
    return NcInterpolation_PrepareBlockTs(pBlock, pOutError);
}

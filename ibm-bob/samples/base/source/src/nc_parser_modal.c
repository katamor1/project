/* ibm-bob/samples/base/source/src/nc_parser_modal.c */
/* Applies G-code modal state to fixed execution blocks. */
/* This exists so address-word parsing can stay separate from modal rules. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser_tokens.c, ibm-bob/samples/base/source/inc/nc_parser_internal.h, ibm-bob/samples/base/source/inc/nc_codes.h */
#include <string.h>
#include "nc_codes.h"
#include "nc_coordinate.h"
#include "nc_interpolation.h"
#include "nc_parser_internal.h"
#include "nc_program.h"

#define G_GROUP_MOTION   (0x01U)
#define G_GROUP_PLANE    (0x02U)
#define G_GROUP_FEED     (0x04U)
#define G_GROUP_COORD    (0x08U)
#define G_GROUP_STOP     (0x10U)
#define G_GROUP_UNIT     (0x20U)
#define G_GROUP_DISTANCE (0x40U)

static int32_t s_lastFeed;
static uint32_t s_lastSpindle;
static NC_MOTION_TYPE s_modalMotion;
static NC_PLANE s_plane;
static NC_FEED_MODE s_feedMode;
static uint8_t s_exactStopMode;
static uint8_t s_oneShotExactStop;

static uint8_t IsMotionType(NC_MOTION_TYPE motion)
{
    return (uint8_t)((motion == NC_MOTION_RAPID) ||
                     (motion == NC_MOTION_LINEAR) ||
                     (motion == NC_MOTION_ARC_CW) ||
                     (motion == NC_MOTION_ARC_CCW));
}

void NcParser_ResetModalState(void)
{
    s_lastFeed = 0;
    s_lastSpindle = 0U;
    s_modalMotion = NC_MOTION_NONE;
    s_plane = NC_PLANE_XY;
    s_feedMode = NC_FEED_MODE_PER_MIN;
    s_exactStopMode = 0U;
    s_oneShotExactStop = 0U;
    NcCoordinate_Reset();
}

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
    NcCoordinate_FillTargets(pBlock);
    for (i = 0U; i < AXIS_MAX; ++i) {
        pBlock->axis_start[i] = pBlock->axis_target[i];
        pBlock->arc_center[i] = pBlock->axis_start[i];
    }
}

void NcParser_SetFeed(int32_t feed, NC_EXEC_BLOCK* pBlock)
{
    s_lastFeed = feed;
    pBlock->feed = feed;
}

void NcParser_SetSpindle(uint32_t spindleSpeed, NC_EXEC_BLOCK* pBlock)
{
    s_lastSpindle = spindleSpeed;
    pBlock->spindle_speed = spindleSpeed;
    pBlock->aux_flags |= NC_AUX_FLAG_SPINDLE;
}

static int32_t MarkGroup(NC_PARSE_CONTEXT* pCtx, uint8_t group)
{
    if ((pCtx->seen_groups & group) != 0U) {
        return -3;
    }
    pCtx->seen_groups |= group;
    return 0;
}

static int32_t ApplyCoordinateG(int32_t code,
                                const NC_G_CODE_INFO* pInfo,
                                NC_EXEC_BLOCK* pBlock,
                                NC_PARSE_CONTEXT* pCtx)
{
    if ((code != 10) && (code != 52) && (code != 53) &&
        (code != 92) && ((code < 54) || (code > 59))) {
        return 1;
    }
    if (MarkGroup(pCtx, G_GROUP_COORD) != 0) {
        return -3;
    }
    if (code == 10) {
        pCtx->coord_mode = NC_PARSE_COORD_G10_WORK_OFFSET;
        pBlock->motion_type = NC_MOTION_NONE;
    } else if (code == 52) {
        pCtx->coord_mode = NC_PARSE_COORD_LOCAL_SHIFT;
        pBlock->motion_type = NC_MOTION_NONE;
    } else if (code == 53) {
        if (g_ncCoordinateState.distance_mode != NC_DISTANCE_ABSOLUTE) {
            return -4;
        }
        pCtx->coord_mode = NC_PARSE_COORD_MACHINE_DIRECT;
    } else if (code == 92) {
        if (g_ncCoordinateState.distance_mode != NC_DISTANCE_ABSOLUTE) {
            return -4;
        }
        pCtx->coord_mode = NC_PARSE_COORD_TEMPORARY_SHIFT;
        pBlock->motion_type = NC_MOTION_NONE;
    } else {
        NcCoordinate_SelectWork((uint8_t)(code - 54));
        pCtx->coord_mode = NC_PARSE_COORD_WORK_SELECT;
        pBlock->motion_type = NC_MOTION_NONE;
    }
    pBlock->modal_flags |= pInfo->modal_flag;
    return 0;
}

static int32_t ApplyControlG(int32_t code,
                             const NC_G_CODE_INFO* pInfo,
                             NC_EXEC_BLOCK* pBlock,
                             NC_PARSE_CONTEXT* pCtx)
{
    if ((code >= 17) && (code <= 19)) {
        if (MarkGroup(pCtx, G_GROUP_PLANE) != 0) {
            return -3;
        }
        s_plane = (code == 18) ? NC_PLANE_ZX :
                  ((code == 19) ? NC_PLANE_YZ : NC_PLANE_XY);
        pBlock->plane = s_plane;
    } else if ((code == 20) || (code == 21)) {
        if (MarkGroup(pCtx, G_GROUP_UNIT) != 0) {
            return -3;
        }
        NcCoordinate_SetUnitMode((code == 20) ? NC_UNIT_INCH : NC_UNIT_MM);
    } else if ((code == 90) || (code == 91)) {
        if (MarkGroup(pCtx, G_GROUP_DISTANCE) != 0) {
            return -3;
        }
        NcCoordinate_SetDistanceMode((code == 91) ?
                                     NC_DISTANCE_INCREMENTAL :
                                     NC_DISTANCE_ABSOLUTE);
    } else if ((code == 94) || (code == 95)) {
        if (MarkGroup(pCtx, G_GROUP_FEED) != 0) {
            return -3;
        }
        s_feedMode = (code == 95) ? NC_FEED_MODE_PER_REV :
                                    NC_FEED_MODE_PER_MIN;
        pBlock->feed_mode = s_feedMode;
    } else {
        return 1;
    }
    pBlock->modal_flags |= pInfo->modal_flag;
    return 0;
}

static int32_t ApplyStopG(int32_t code,
                          const NC_G_CODE_INFO* pInfo,
                          NC_EXEC_BLOCK* pBlock,
                          NC_PARSE_CONTEXT* pCtx)
{
    if ((code != 9) && (code != 61) && (code != 64)) {
        return 1;
    }
    if (MarkGroup(pCtx, G_GROUP_STOP) != 0) {
        return -3;
    }
    if (code == 9) {
        s_oneShotExactStop = 1U;
    } else {
        s_exactStopMode = (uint8_t)(code == 61);
        s_oneShotExactStop = 0U;
    }
    pBlock->modal_flags |= pInfo->modal_flag;
    return 0;
}

int32_t NcParser_ApplyGCode(int32_t code,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx)
{
    NC_G_CODE_INFO info;
    int32_t result;

    if ((NcCodes_GetGCodeInfo(code, &info) != 0) ||
        (info.supported_v1 == 0U)) {
        return -1;
    }
    result = ApplyCoordinateG(code, &info, pBlock, pCtx);
    if (result != 1) {
        return result;
    }
    result = ApplyControlG(code, &info, pBlock, pCtx);
    if (result != 1) {
        return result;
    }
    result = ApplyStopG(code, &info, pBlock, pCtx);
    if (result != 1) {
        return result;
    }
    if (MarkGroup(pCtx, G_GROUP_MOTION) != 0) {
        return -3;
    }
    if (code == 4) {
        pBlock->motion_type = NC_MOTION_DWELL;
    } else {
        s_modalMotion = info.motion_type;
        pBlock->motion_type = info.motion_type;
    }
    pBlock->modal_flags |= info.modal_flag;
    return 0;
}

int32_t NcParser_FinalizeBlock(NC_EXEC_BLOCK* pBlock,
                               NC_ERROR_CODE* pOutError)
{
    uint8_t hasArc = (uint8_t)(pBlock->modal_flags &
                               (NC_ARC_FLAG_IJK | NC_ARC_FLAG_R));

    if (IsMotionType(pBlock->motion_type) != 0U) {
        if ((pBlock->axis_mask == 0U) && (hasArc == 0U)) {
            pBlock->motion_type = (pBlock->aux_flags != 0U) ?
                                  NC_MOTION_AUX_ONLY : NC_MOTION_NONE;
            return NcInterpolation_PrepareBlockTs(pBlock, pOutError);
        }
        if ((s_oneShotExactStop != 0U) || (s_exactStopMode != 0U)) {
            pBlock->modal_flags |= (s_oneShotExactStop != 0U) ?
                                   NC_MODAL_FLAG_G09 : NC_MODAL_FLAG_G61;
            s_oneShotExactStop = 0U;
        }
    } else if ((pBlock->motion_type != NC_MOTION_DWELL) &&
               (pBlock->axis_mask == 0U) && (hasArc == 0U)) {
        pBlock->motion_type = (pBlock->aux_flags != 0U) ?
                              NC_MOTION_AUX_ONLY : NC_MOTION_NONE;
    }
    if ((hasArc != 0U) && (pBlock->motion_type != NC_MOTION_ARC_CW) &&
        (pBlock->motion_type != NC_MOTION_ARC_CCW)) {
        *pOutError = NC_ERR_UNSUPPORTED;
        return -1;
    }
    return NcInterpolation_PrepareBlockTs(pBlock, pOutError);
}

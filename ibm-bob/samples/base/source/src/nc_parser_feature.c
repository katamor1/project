/* ibm-bob/samples/base/source/src/nc_parser_feature.c */
/* Applies feature-oriented G-code commands such as smoothing and compensation modes. */
/* This exists so modal parsing stays small while feature code details continue to grow. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/nc_parser_internal.h, ibm-bob/samples/base/source/inc/nc_program.h */
#include "nc_parser_internal.h"
#include "nc_program.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define GD(whole, dec) NC_G_CODE10((whole), (dec))

static int32_t MarkSmoothingFeature(NC_PARSE_CONTEXT* pCtx)
{
    if (pCtx->smoothing_seen != 0U) {
        return -3;
    }
    pCtx->smoothing_seen = 1U;
    return 0;
}

static int32_t ApplySmoothingG(int32_t code, NC_EXEC_BLOCK* pBlock,
                               NC_PARSE_CONTEXT* pCtx)
{
    if ((code == G(8)) || (code == G(5))) {
        if (MarkSmoothingFeature(pCtx) != 0) {
            return -3;
        }
        pBlock->motion_type = NC_MOTION_NONE;
        pBlock->g_code10 = (uint16_t)code;
        pBlock->modal_flags |= (code == G(8)) ? NC_MODAL_FLAG_G08 :
            NC_MODAL_FLAG_G05;
        return 0;
    }
    if ((code == GD(5, 1)) || (code == GD(5, 2))) {
        if (MarkSmoothingFeature(pCtx) != 0) {
            return -3;
        }
        pBlock->feature_flags |= (NC_FEATURE_FLAG_HIGH_PRECISION |
                                  NC_FEATURE_FLAG_SMOOTHING);
        pBlock->motion_type = NC_MOTION_NONE;
        pBlock->g_code10 = (uint16_t)code;
        return 0;
    }
    return 1;
}

static void ApplyCommandFlag(NC_EXEC_BLOCK* pBlock, uint32_t flag,
                             NC_MOTION_TYPE motion)
{
    pBlock->feature_flags |= flag;
    pBlock->motion_type = motion;
}

int32_t NcParser_ApplyFeatureG(int32_t code,
                               NC_EXEC_BLOCK* pBlock,
                               NC_PARSE_CONTEXT* pCtx)
{
    int32_t result = ApplySmoothingG(code, pBlock, pCtx);

    if (result != 1) {
        return result;
    }
    if (code == GD(51, 4)) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_SYNC, NC_MOTION_NONE);
    } else if (code == GD(50, 4)) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_SYNC, NC_MOTION_NONE);
    } else if (code == GD(51, 5)) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_COMPOUND, NC_MOTION_NONE);
    } else if (code == GD(50, 5)) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_COMPOUND, NC_MOTION_NONE);
    } else if (code == GD(51, 6)) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_OVERLAY, NC_MOTION_NONE);
    } else if (code == GD(50, 6)) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_OVERLAY, NC_MOTION_NONE);
    } else if ((code == GD(6, 2)) || (code == GD(7, 1)) ||
               (code == GD(12, 1)) || (code == GD(13, 1)) || (code == GD(12, 4)) || (code == GD(13, 4))) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_ADVANCED_INTERP,
                         NC_MOTION_ADVANCED_INTERP);
    } else if ((code == GD(68, 2)) || (code == GD(69, 1))) {
        pBlock->kinematics_command = (uint8_t)((code == GD(68, 2)) ?
            NC_KINEMATICS_CMD_TILT_ENABLE : NC_KINEMATICS_CMD_TILT_CANCEL);
        if (code == GD(68, 2)) {
            pCtx->coord_mode = NC_PARSE_COORD_TILTED_PLANE;
        }
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_TILTED_PLANE, NC_MOTION_NONE);
    } else if (code == GD(53, 1)) {
        pBlock->tool_axis_command = NC_TOOL_AXIS_CMD_ENABLE;
        pCtx->coord_mode = NC_PARSE_COORD_TOOL_AXIS;
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_TOOL_AXIS_DIR, NC_MOTION_NONE);
    } else if ((code == GD(51, 1)) || (code == GD(50, 1))) {
        pBlock->mirror_command = (uint8_t)((code == GD(51, 1)) ?
            NC_MIRROR_CMD_ENABLE : NC_MIRROR_CMD_CANCEL);
        if (code == GD(51, 1)) {
            pCtx->coord_mode = NC_PARSE_COORD_MIRROR_SETTING;
        }
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_MIRROR, NC_MOTION_NONE);
    } else if ((code == GD(10, 6)) || (code == GD(11, 6))) {
        pBlock->axis_retract_command = (uint8_t)((code == GD(10, 6)) ?
            NC_AXIS_RETRACT_CMD_RETRACT : NC_AXIS_RETRACT_CMD_RECOVER);
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_AXIS_RETRACT,
                         NC_MOTION_REFERENCE_RETURN);
    } else if ((code == G(15)) || (code == G(16))) {
        pBlock->polar_command =
            (uint8_t)((code == G(16)) ? NC_POLAR_CMD_ENABLE : NC_POLAR_CMD_CANCEL);
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_POLAR_COORD, NC_MOTION_NONE);
    } else if ((code == G(40)) || (code == G(41)) || (code == G(42))) {
        pBlock->cutter_comp_command = (code == G(40)) ? NC_CUTTER_COMP_CMD_CANCEL :
            (uint8_t)((code == G(41)) ? NC_CUTTER_COMP_CMD_LEFT : NC_CUTTER_COMP_CMD_RIGHT);
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_CUTTER_RADIUS, NC_MOTION_NONE);
    } else if ((code == G(43)) || (code == G(44)) || (code == G(49))) {
        pBlock->tool_length_command = (code == G(49)) ? NC_TOOL_LENGTH_CMD_CANCEL :
            (uint8_t)((code == G(44)) ? NC_TOOL_LENGTH_CMD_NEGATIVE : NC_TOOL_LENGTH_CMD_POSITIVE);
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_TOOL_LENGTH, NC_MOTION_NONE);
    } else if ((code == G(68)) || (code == G(69))) {
        pBlock->rotation_command =
            (uint8_t)((code == G(68)) ? NC_ROTATION_CMD_ENABLE : NC_ROTATION_CMD_CANCEL);
        if (code == G(68)) {
            pCtx->coord_mode = NC_PARSE_COORD_ROTATION_CENTER;
        }
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_COORD_ROTATION, NC_MOTION_NONE);
    } else if ((code == G(96)) || (code == G(97))) {
        ApplyCommandFlag(pBlock, NC_FEATURE_FLAG_ADVANCED_INTERP, NC_MOTION_NONE);
    } else {
        return 1;
    }
    pBlock->g_code10 = (uint16_t)code;
    return 0;
}

static int32_t FinalizeG08(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_P) == 0U) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if (pBlock->p_word > 2U) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if (pBlock->p_word == 1U) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
    } else if (pBlock->p_word == 2U) {
        pBlock->feature_flags |= (NC_FEATURE_FLAG_SMOOTHING |
                                  NC_FEATURE_FLAG_HIGH_PRECISION);
    }
    return 0;
}

static int32_t FinalizeG05(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_P) == 0U) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if ((pBlock->p_word == 10000U) || (pBlock->p_word == 0U)) {
        if ((pBlock->cutter_comp_command != NC_CUTTER_COMP_CMD_NONE) ||
            (g_ncCompensationStatus.cutter_comp_mode != 0U)) {
            *pOutError = NC_ERR_MODAL_CONFLICT;
            return -1;
        }
    }
    if (pBlock->p_word == 10000U) {
        pBlock->feature_flags |= (NC_FEATURE_FLAG_HPCC |
                                  NC_FEATURE_FLAG_HIGH_PRECISION |
                                  NC_FEATURE_FLAG_SMOOTHING);
    } else if (pBlock->p_word == 3U) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_SPEED_MODE;
    } else if (pBlock->p_word != 0U) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    return 0;
}

int32_t NcParser_FinalizeFeatureBlock(NC_EXEC_BLOCK* pBlock,
                                      NC_ERROR_CODE* pOutError)
{
    if (pBlock->g_code10 == (uint16_t)G(8)) {
        return FinalizeG08(pBlock, pOutError);
    }
    if (pBlock->g_code10 == (uint16_t)G(5)) {
        return FinalizeG05(pBlock, pOutError);
    }
    return 0;
}

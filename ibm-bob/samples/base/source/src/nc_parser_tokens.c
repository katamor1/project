/* ibm-bob/samples/base/source/src/nc_parser_tokens.c */
/* Applies NC address words after modal G-code state has selected context. */
/* This exists so unit scaling and coordinate words stay out of modal code. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/nc_parser_internal.h, ibm-bob/samples/base/source/src/nc_coordinate.c */
#include <limits.h>
#include <math.h>
#include "nc_codes.h"
#include "nc_capability.h"
#include "nc_coordinate.h"
#include "nc_cycle.h"
#include "nc_parser_internal.h"
#include "nc_program.h"

#define NC_INCH_TO_MM (25.4)
/**
 * @brief Handle round to int for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pOut Output pointer that receives the computed value.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t RoundToInt(double value, int32_t* pOut)
{
    value += (value >= 0.0) ? 0.5 : -0.5;
    if ((value > (double)INT_MAX) || (value < (double)INT_MIN)) {
        return -1;
    }
    *pOut = (int32_t)value;
    return 0;
}

/**
 * @brief Handle normalize gcode for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pOutCode10 Output pointer that receives the normalized G-code value.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t NormalizeGCode(double value, int32_t* pOutCode10)
{
    double scaled;
    int32_t code10;

    if ((value < 0.0) || (value > 999.9)) {
        return -1;
    }
    scaled = (value * 10.0) + 0.5;
    if (RoundToInt(scaled - 0.5, &code10) != 0) {
        return -1;
    }
    if (fabs(((double)code10 / 10.0) - value) > 0.0001) {
        return -1;
    }
    *pOutCode10 = code10;
    return 0;
}

/**
 * @brief Handle scale linear for this module.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pOut Output pointer that receives the computed value.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t ScaleLinear(double value, int32_t* pOut)
{
    double scaled = value * (double)NC_POSITION_SCALE;

    if (g_ncCoordinateState.unit_mode == NC_UNIT_INCH) {
        scaled *= NC_INCH_TO_MM;
    }
    return RoundToInt(scaled, pOut);
}

/**
 * @brief Handle scale feed for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pOut Output pointer that receives the computed value.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ScaleFeed(double value, int32_t* pOut)
{
    double scaled = value * (double)NC_POSITION_SCALE;

    if (g_ncCoordinateState.unit_mode == NC_UNIT_INCH) {
        scaled *= NC_INCH_TO_MM;
    }
    if (scaled > (double)INT_MAX) {
        *pOut = INT_MAX;
        return 0;
    }
    if (scaled < (double)INT_MIN) {
        *pOut = INT_MIN;
        return 0;
    }
    return RoundToInt(scaled, pOut);
}

/**
 * @brief Apply g10 axis to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param axisIndex Axis index selected by the parsed address word.
 * @param scaled Fixed-point scaled value from the parsed address word.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyG10Axis(int32_t axisIndex, int32_t scaled,
                            const NC_PARSE_CONTEXT* pCtx)
{
    if ((pCtx->g10_l_valid == 0U) ||
        (pCtx->g10_l_value != NC_G10_L_WORK_OFFSET)) {
        return -1;
    }
    if (pCtx->g10_p_valid == 0U) {
        return -2;
    }
    return (NcCoordinate_SetWorkOffsetAxis((uint8_t)(pCtx->g10_p_value - 1),
                                           axisIndex,
                                           scaled) == 0) ? 0 : -2;
}

/**
 * @brief Apply axis to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pInfo Address or code metadata used by the helper.
 * @param scaled Fixed-point scaled value from the parsed address word.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyAxis(const NC_ADDRESS_INFO* pInfo,
                         int32_t scaled,
                         NC_EXEC_BLOCK* pBlock,
                         NC_PARSE_CONTEXT* pCtx)
{
    /* Handle the next conditional branch for this processing stage. */
    if (pCtx->coord_mode == NC_PARSE_COORD_TILTED_PLANE) {
        if ((pInfo->axis_index >= 0) && (pInfo->axis_index < 3)) {
            pBlock->tilt_origin[pInfo->axis_index] = scaled;
            return 0;
        }
        if (pInfo->axis_index == 3) {
            pBlock->tilt_angle_deg[0] = scaled;
            return 0;
        }
        return -4;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_TOOL_AXIS) {
        if (pInfo->axis_index == 3) {
            pBlock->tool_axis_angle_deg[0] = scaled;
            return 0;
        }
        return -4;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_MIRROR_SETTING) {
        if ((pInfo->axis_index >= 0) && (pInfo->axis_index < (int32_t)AXIS_MAX)) {
            if (scaled != 0) {
                pBlock->mirror_axis_mask |= (1UL << pInfo->axis_index);
            }
            return 0;
        }
        return -4;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_ROTATION_CENTER) {
        if ((pInfo->axis_index == 0) || (pInfo->axis_index == 1)) {
            pBlock->rotation_center[pInfo->axis_index] = scaled;
            return 0;
        }
        return -4;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_LOCAL_SHIFT) {
        return (NcCoordinate_SetLocalShiftAxis(pInfo->axis_index, scaled) == 0) ?
               0 : -4;
    }
    /* Handle the next conditional branch for this processing stage. */
    if (pCtx->coord_mode == NC_PARSE_COORD_WORK_SELECT) {
        return -4;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_G10_WORK_OFFSET) {
        return ApplyG10Axis(pInfo->axis_index, scaled, pCtx);
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_MACHINE_DIRECT) {
        return (NcCoordinate_SetMachineAxis(pInfo->axis_index,
                                            scaled,
                                            pBlock) == 0) ? 0 : -4;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_TEMPORARY_SHIFT) {
        return (NcCoordinate_SetTemporaryAxis(pInfo->axis_index, scaled) == 0) ?
               0 : -4;
    }
    return (NcCoordinate_SetProgramAxis(pInfo->axis_index, scaled, pBlock) == 0) ?
           0 : -4;
}

/**
 * @brief Apply incremental axis to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pInfo Address or code metadata used by the helper.
 * @param scaled Fixed-point scaled value from the parsed address word.
 * @param pBlock NC execution block read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyIncrementalAxis(const NC_ADDRESS_INFO* pInfo,
                                    int32_t scaled,
                                    NC_EXEC_BLOCK* pBlock)
{
    int32_t base;

    if ((pInfo->axis_index < 0) || (pInfo->axis_index >= (int32_t)AXIS_MAX)) {
        return -4;
    }
    base = g_ncCoordinateState.program_position[pInfo->axis_index];
    return (NcCoordinate_SetProgramAxis(pInfo->axis_index,
                                        base + scaled,
                                        pBlock) == 0) ? 0 : -4;
}

/**
 * @brief Return whether arc motion is true for the current block or state.
 * @param motion Motion type being tested by the helper.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsArcMotion(NC_MOTION_TYPE motion)
{
    return (uint8_t)((motion == NC_MOTION_ARC_CW) ||
                     (motion == NC_MOTION_ARC_CCW));
}

/**
 * @brief Apply scaled token to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pInfo Address or code metadata used by the helper.
 * @param scaled Fixed-point scaled value from the parsed address word.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyScaledToken(const NC_ADDRESS_INFO* pInfo,
                                int32_t scaled,
                                NC_EXEC_BLOCK* pBlock,
                                NC_PARSE_CONTEXT* pCtx)
{
    /* Handle the next conditional branch for this processing stage. */
    if (pInfo->kind == NC_ADDRESS_KIND_AXIS) {
        return ApplyAxis(pInfo, scaled, pBlock, pCtx);
    }
    if (pInfo->kind == NC_ADDRESS_KIND_INCREMENTAL_AXIS) {
        return ApplyIncrementalAxis(pInfo, scaled, pBlock);
    }
    if (pInfo->kind == NC_ADDRESS_KIND_ARC_OFFSET) {
        pBlock->arc_center[pInfo->axis_index] =
            pBlock->axis_start[pInfo->axis_index] + scaled;
        pBlock->modal_flags |= NC_ARC_FLAG_IJK;
        return 0;
    }
    /* Handle the next conditional branch for this processing stage. */
    if (pInfo->kind == NC_ADDRESS_KIND_ARC_RADIUS) {
        if (IsArcMotion(pBlock->motion_type) != 0U) {
            pBlock->arc_radius = scaled;
            pBlock->modal_flags |= NC_ARC_FLAG_R;
        } else {
            pBlock->r_value = scaled;
            pBlock->feature_flags |= NC_FEATURE_FLAG_PARAM_R;
        }
        return 0;
    }
    if (pInfo->kind == NC_ADDRESS_KIND_OFFSET) {
        if (pCtx->coord_mode == NC_PARSE_COORD_TILTED_PLANE) {
            if (pInfo->address == 'B') {
                pBlock->tilt_angle_deg[1] = scaled;
                return 0;
            }
            if (pInfo->address == 'C') {
                pBlock->tilt_angle_deg[2] = scaled;
                return 0;
            }
        }
        if (pCtx->coord_mode == NC_PARSE_COORD_TOOL_AXIS) {
            if (pInfo->address == 'B') {
                pBlock->tool_axis_angle_deg[1] = scaled;
                return 0;
            }
            if (pInfo->address == 'C') {
                pBlock->tool_axis_angle_deg[2] = scaled;
                return 0;
            }
        }
        if (pInfo->address == 'H') {
            pBlock->h_offset_no = (uint16_t)(scaled / NC_POSITION_SCALE);
        } else if (pInfo->address == 'D') {
            pBlock->d_offset_no = (uint16_t)(scaled / NC_POSITION_SCALE);
        }
        return 0;
    }
    NcParser_SetFeed(scaled, pBlock);
    return 0;
}

/**
 * @brief Apply lword to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyLWord(double value, NC_PARSE_CONTEXT* pCtx)
{
    int32_t word;

    if ((pCtx->coord_mode != NC_PARSE_COORD_G10_WORK_OFFSET) ||
        (RoundToInt(value, &word) != 0)) {
        return -1;
    }
    pCtx->g10_l_valid = 1U;
    pCtx->g10_l_value = word;
    return (word == NC_G10_L_WORK_OFFSET) ? 0 : -1;
}

/**
 * @brief Apply pword to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyPWord(double value,
                          NC_EXEC_BLOCK* pBlock,
                          NC_PARSE_CONTEXT* pCtx)
{
    int32_t word;

    if (pBlock->motion_type == NC_MOTION_DWELL) {
        if ((value < 0.0) || (value > (double)NC_DWELL_MAX_TICKS)) {
            return -2;
        }
        pBlock->interp_ticks = (uint32_t)(value + 0.5);
        return 0;
    }
    if (pCtx->coord_mode == NC_PARSE_COORD_G10_WORK_OFFSET) {
        if (RoundToInt(value, &word) != 0) {
            return -1;
        }
        if ((word < 1) || (word > (int32_t)NC_WORK_COORD_SYSTEMS)) {
            return -2;
        }
        pCtx->g10_p_valid = 1U;
        pCtx->g10_p_value = word;
        return 0;
    }
    if (value < 0.0) {
        return -2;
    }
    pBlock->p_word = (uint32_t)(value + 0.5);
    pBlock->feature_flags |= NC_FEATURE_FLAG_PARAM_P;
    return 0;
}

/**
 * @brief Apply qword to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pBlock NC execution block read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyQWord(double value, NC_EXEC_BLOCK* pBlock)
{
    int32_t scaled;

    if (ScaleLinear(value, &scaled) != 0) {
        return -2;
    }
    pBlock->q_value = scaled;
    pBlock->feature_flags |= NC_FEATURE_FLAG_PARAM_Q;
    return 0;
}

/**
 * @brief Apply aux token to the current block or state.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pInfo Address or code metadata used by the helper.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pBlock NC execution block read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ApplyAuxToken(const NC_ADDRESS_INFO* pInfo,
                             double value,
                             NC_EXEC_BLOCK* pBlock)
{
    /* Handle the next conditional branch for this processing stage. */
    if (pInfo->kind == NC_ADDRESS_KIND_NUMBER) {
        return 0;
    }
    if (value < 0.0) {
        return -2;
    }
    if (pInfo->kind == NC_ADDRESS_KIND_SPINDLE) {
        NcParser_SetSpindle((uint32_t)(value + 0.5), pBlock);
        return 0;
    }
    /* Handle the next conditional branch for this processing stage. */
    if (pInfo->kind == NC_ADDRESS_KIND_TOOL) {
        pBlock->tool_no = (uint32_t)(value + 0.5);
        pBlock->aux_flags |= NC_AUX_FLAG_TOOL;
        return 0;
    }
    if (pInfo->kind == NC_ADDRESS_KIND_M_CODE) {
        NC_M_CODE_INFO mInfo;
        uint32_t code = (uint32_t)(value + 0.5);

        if ((NcCodes_GetMCodeInfo(code, &mInfo) != 0) ||
            (mInfo.supported_v1 == 0U)) {
            NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_M, code);
            return -1;
        }
        pBlock->aux_m_code = mInfo.code;
        pBlock->aux_flags |= NC_AUX_FLAG_M_CODE;
        if (mInfo.waits_for_mfin != 0U) {
            pBlock->aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
        }
        return 0;
    }
    return -1;
}

/**
 * @brief Handle nc parser apply token for this module.
 * @param address NC address word being parsed.
 * @param value Numeric value being converted, clamped, or tested.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pCtx Parser context read or updated by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_ApplyToken(char address,
                            double value,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx)
{
    /* Prepare local state used by the following processing stage. */
    int32_t scaled;
    int32_t gCode10;
    NC_ADDRESS_INFO info;

    if ((NcCodes_GetAddressInfo(address, &info) != 0) ||
        (info.supported_v1 == 0U)) {
        if ((address >= 'A') && (address <= 'Z')) {
            NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_ADDRESS,
                                           (uint32_t)(address - 'A'));
        }
        return -1;
    }
    if (info.kind == NC_ADDRESS_KIND_G_CODE) {
        if (NormalizeGCode(value, &gCode10) != 0) {
            return -2;
        }
        return NcParser_ApplyGCode(gCode10, pBlock, pCtx);
    }
    if (info.kind == NC_ADDRESS_KIND_L_WORD) {
        return ApplyLWord(value, pCtx);
    }
    if (info.kind == NC_ADDRESS_KIND_P_WORD) {
        return ApplyPWord(value, pBlock, pCtx);
    }
    if (info.kind == NC_ADDRESS_KIND_Q_WORD) {
        return ApplyQWord(value, pBlock);
    }
    if ((info.address == 'K') &&
        /* Apply the next logical update for this processing stage. */
        (pBlock->motion_type == NC_MOTION_CANNED_CYCLE)) {
        return NcCycle_SetRepeatWord(value, pBlock);
    }
    if ((info.address == 'R') &&
        (pCtx->coord_mode == NC_PARSE_COORD_ROTATION_CENTER)) {
        int32_t angle;
        if (RoundToInt(value * (double)NC_ANGLE_SCALE, &angle) != 0) {
            return -2;
        }
        pBlock->rotation_angle_deg = angle;
        return 0;
    }
    if (info.kind == NC_ADDRESS_KIND_FEED) {
        if (ScaleFeed(value, &scaled) != 0) {
            return -2;
        }
        return ApplyScaledToken(&info, scaled, pBlock, pCtx);
    }
    if ((info.kind == NC_ADDRESS_KIND_AXIS) ||
        (info.kind == NC_ADDRESS_KIND_INCREMENTAL_AXIS) ||
        (info.kind == NC_ADDRESS_KIND_ARC_OFFSET) ||
        (info.kind == NC_ADDRESS_KIND_ARC_RADIUS) ||
        (info.kind == NC_ADDRESS_KIND_OFFSET)) {
        if (ScaleLinear(value, &scaled) != 0) {
            return -2;
        }
        return ApplyScaledToken(&info, scaled, pBlock, pCtx);
    }
    return ApplyAuxToken(&info, value, pBlock);
}

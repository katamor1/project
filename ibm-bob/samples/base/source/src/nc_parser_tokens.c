/* ibm-bob/samples/base/source/src/nc_parser_tokens.c */
/* Applies NC address words after modal G-code state has selected context. */
/* This exists so unit scaling and coordinate words stay out of modal code. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/nc_parser_internal.h, ibm-bob/samples/base/source/src/nc_coordinate.c */
#include <limits.h>
#include "nc_codes.h"
#include "nc_coordinate.h"
#include "nc_parser_internal.h"
#include "nc_program.h"

#define NC_INCH_TO_MM (25.4)

static int32_t RoundToInt(double value, int32_t* pOut)
{
    value += (value >= 0.0) ? 0.5 : -0.5;
    if ((value > (double)INT_MAX) || (value < (double)INT_MIN)) {
        return -1;
    }
    *pOut = (int32_t)value;
    return 0;
}

static int32_t ScaleLinear(double value, int32_t* pOut)
{
    double scaled = value * (double)NC_POSITION_SCALE;

    if (g_ncCoordinateState.unit_mode == NC_UNIT_INCH) {
        scaled *= NC_INCH_TO_MM;
    }
    return RoundToInt(scaled, pOut);
}

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

static int32_t ApplyAxis(const NC_ADDRESS_INFO* pInfo,
                         int32_t scaled,
                         NC_EXEC_BLOCK* pBlock,
                         NC_PARSE_CONTEXT* pCtx)
{
    if (pCtx->coord_mode == NC_PARSE_COORD_LOCAL_SHIFT) {
        return (NcCoordinate_SetLocalShiftAxis(pInfo->axis_index, scaled) == 0) ?
               0 : -4;
    }
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

static int32_t ApplyScaledToken(const NC_ADDRESS_INFO* pInfo,
                                int32_t scaled,
                                NC_EXEC_BLOCK* pBlock,
                                NC_PARSE_CONTEXT* pCtx)
{
    if (pInfo->kind == NC_ADDRESS_KIND_AXIS) {
        return ApplyAxis(pInfo, scaled, pBlock, pCtx);
    }
    if (pInfo->kind == NC_ADDRESS_KIND_ARC_OFFSET) {
        pBlock->arc_center[pInfo->axis_index] =
            pBlock->axis_start[pInfo->axis_index] + scaled;
        pBlock->modal_flags |= NC_ARC_FLAG_IJK;
        return 0;
    }
    if (pInfo->kind == NC_ADDRESS_KIND_ARC_RADIUS) {
        pBlock->arc_radius = scaled;
        pBlock->modal_flags |= NC_ARC_FLAG_R;
        return 0;
    }
    NcParser_SetFeed(scaled, pBlock);
    return 0;
}

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
    if ((pCtx->coord_mode != NC_PARSE_COORD_G10_WORK_OFFSET) ||
        (RoundToInt(value, &word) != 0)) {
        return -1;
    }
    if ((word < 1) || (word > (int32_t)NC_WORK_COORD_SYSTEMS)) {
        return -2;
    }
    pCtx->g10_p_valid = 1U;
    pCtx->g10_p_value = word;
    return 0;
}

static int32_t ApplyAuxToken(const NC_ADDRESS_INFO* pInfo,
                             double value,
                             NC_EXEC_BLOCK* pBlock)
{
    if (value < 0.0) {
        return -2;
    }
    if (pInfo->kind == NC_ADDRESS_KIND_SPINDLE) {
        NcParser_SetSpindle((uint32_t)(value + 0.5), pBlock);
        return 0;
    }
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
            return -1;
        }
        pBlock->aux_m_code = mInfo.code;
        pBlock->aux_flags |= NC_AUX_FLAG_M_CODE;
        return 0;
    }
    return -1;
}

int32_t NcParser_ApplyToken(char address,
                            double value,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx)
{
    int32_t scaled;
    NC_ADDRESS_INFO info;

    if ((NcCodes_GetAddressInfo(address, &info) != 0) ||
        (info.supported_v1 == 0U)) {
        return -1;
    }
    if (info.kind == NC_ADDRESS_KIND_G_CODE) {
        return NcParser_ApplyGCode((int32_t)(value + 0.5), pBlock, pCtx);
    }
    if (info.kind == NC_ADDRESS_KIND_L_WORD) {
        return ApplyLWord(value, pCtx);
    }
    if (info.kind == NC_ADDRESS_KIND_P_WORD) {
        return ApplyPWord(value, pBlock, pCtx);
    }
    if (info.kind == NC_ADDRESS_KIND_FEED) {
        if (ScaleFeed(value, &scaled) != 0) {
            return -2;
        }
        return ApplyScaledToken(&info, scaled, pBlock, pCtx);
    }
    if ((info.kind == NC_ADDRESS_KIND_AXIS) ||
        (info.kind == NC_ADDRESS_KIND_ARC_OFFSET) ||
        (info.kind == NC_ADDRESS_KIND_ARC_RADIUS)) {
        if (ScaleLinear(value, &scaled) != 0) {
            return -2;
        }
        return ApplyScaledToken(&info, scaled, pBlock, pCtx);
    }
    return ApplyAuxToken(&info, value, pBlock);
}

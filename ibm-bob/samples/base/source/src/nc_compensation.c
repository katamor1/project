/* ibm-bob/samples/base/source/src/nc_compensation.c */
/* Applies TS-side tool length, cutter radius, coordinate rotation, and polar conversion. */
/* This exists so compensation state is resolved before RT interpolation starts. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_compensation.h, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/src/control_api.c */
#include <math.h>
#include <string.h>
#include "nc_compensation.h"

static int32_t s_toolLengthOffsets[NC_TOOL_OFFSET_TABLE_SIZE];
static int32_t s_cutterRadiusOffsets[NC_TOOL_OFFSET_TABLE_SIZE];

static int32_t RoundFixed(double value)
{
    return (int32_t)(value + ((value >= 0.0) ? 0.5 : -0.5));
}

static uint8_t IsMotionLike(NC_MOTION_TYPE motion)
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

void NcCompensation_Reset(void)
{
    (void)memset((void*)&g_ncCompensationStatus, 0, sizeof(g_ncCompensationStatus));
}

int32_t NcCompensation_SetToolLengthOffset(uint16_t offsetNo, int32_t offset)
{
    if ((offsetNo == 0U) || (offsetNo >= NC_TOOL_OFFSET_TABLE_SIZE)) {
        return -1;
    }
    s_toolLengthOffsets[offsetNo] = offset;
    return 0;
}

int32_t NcCompensation_SetCutterRadiusOffset(uint16_t offsetNo, int32_t offset)
{
    if ((offsetNo == 0U) || (offsetNo >= NC_TOOL_OFFSET_TABLE_SIZE)) {
        return -1;
    }
    s_cutterRadiusOffsets[offsetNo] = offset;
    return 0;
}

static int32_t LookupOffset(const int32_t* pTable, uint16_t offsetNo, int32_t* pOut)
{
    if ((offsetNo == 0U) || (offsetNo >= NC_TOOL_OFFSET_TABLE_SIZE) ||
        (pOut == 0)) {
        return -1;
    }
    *pOut = pTable[offsetNo];
    return 0;
}

static void ApplyToolLengthCommand(const NC_EXEC_BLOCK* pBlock,
                                   NC_ERROR_CODE* pOutError)
{
    int32_t offset = 0;

    if (pBlock->tool_length_command == NC_TOOL_LENGTH_CMD_NONE) {
        return;
    }
    if (pBlock->tool_length_command == NC_TOOL_LENGTH_CMD_CANCEL) {
        g_ncCompensationStatus.tool_length_active = 0U;
        g_ncCompensationStatus.tool_length_sign = 0U;
        g_ncCompensationStatus.active_h_no = 0U;
        g_ncCompensationStatus.tool_length_offset = 0;
        g_ncCompensationStatus.generation++;
        return;
    }
    if (LookupOffset(s_toolLengthOffsets, pBlock->h_offset_no, &offset) != 0) {
        *pOutError = NC_ERR_RANGE;
        return;
    }
    g_ncCompensationStatus.tool_length_active = 1U;
    g_ncCompensationStatus.tool_length_sign =
        (uint8_t)((pBlock->tool_length_command == NC_TOOL_LENGTH_CMD_NEGATIVE) ? 2U : 1U);
    g_ncCompensationStatus.active_h_no = pBlock->h_offset_no;
    g_ncCompensationStatus.tool_length_offset = offset;
    g_ncCompensationStatus.generation++;
}

static void ApplyCutterCommand(const NC_EXEC_BLOCK* pBlock,
                               NC_ERROR_CODE* pOutError)
{
    int32_t offset = 0;

    if (pBlock->cutter_comp_command == NC_CUTTER_COMP_CMD_NONE) {
        return;
    }
    if (pBlock->cutter_comp_command == NC_CUTTER_COMP_CMD_CANCEL) {
        g_ncCompensationStatus.cutter_comp_mode = 0U;
        g_ncCompensationStatus.active_d_no = 0U;
        g_ncCompensationStatus.cutter_radius_offset = 0;
        g_ncCompensationStatus.generation++;
        return;
    }
    if (LookupOffset(s_cutterRadiusOffsets, pBlock->d_offset_no, &offset) != 0) {
        *pOutError = NC_ERR_RANGE;
        return;
    }
    g_ncCompensationStatus.cutter_comp_mode =
        (uint8_t)((pBlock->cutter_comp_command == NC_CUTTER_COMP_CMD_RIGHT) ? 2U : 1U);
    g_ncCompensationStatus.active_d_no = pBlock->d_offset_no;
    g_ncCompensationStatus.cutter_radius_offset = offset;
    g_ncCompensationStatus.generation++;
}

static void ApplyRotationCommand(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->rotation_command == NC_ROTATION_CMD_CANCEL) {
        g_ncCompensationStatus.rotation_active = 0U;
        g_ncCompensationStatus.rotation_angle_deg = 0;
        g_ncCompensationStatus.generation++;
    } else if (pBlock->rotation_command == NC_ROTATION_CMD_ENABLE) {
        g_ncCompensationStatus.rotation_active = 1U;
        g_ncCompensationStatus.rotation_center[0] = pBlock->rotation_center[0];
        g_ncCompensationStatus.rotation_center[1] = pBlock->rotation_center[1];
        g_ncCompensationStatus.rotation_angle_deg = pBlock->rotation_angle_deg;
        g_ncCompensationStatus.generation++;
    }
}

static void ApplyPolarCommand(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->polar_command == NC_POLAR_CMD_ENABLE) {
        g_ncCompensationStatus.polar_active = 1U;
        g_ncCompensationStatus.generation++;
    } else if (pBlock->polar_command == NC_POLAR_CMD_CANCEL) {
        g_ncCompensationStatus.polar_active = 0U;
        g_ncCompensationStatus.generation++;
    }
}

static void ApplyToolLengthToTarget(NC_EXEC_BLOCK* pBlock)
{
    int32_t offset;

    if ((g_ncCompensationStatus.tool_length_active == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U)) {
        return;
    }
    offset = g_ncCompensationStatus.tool_length_offset;
    if (g_ncCompensationStatus.tool_length_sign == 2U) {
        offset = -offset;
    }
    pBlock->axis_target[2] += offset;
    pBlock->axis_mask |= (1UL << 2U);
    pBlock->feature_flags |= NC_FEATURE_FLAG_TOOL_LENGTH;
    g_ncCompensationStatus.adjusted_axis_mask |= (1UL << 2U);
}

static void ApplyCutterRadiusToTarget(NC_EXEC_BLOCK* pBlock)
{
    double dx;
    double dy;
    double length;
    double nx;
    double ny;
    double sign;
    double radius;

    if ((g_ncCompensationStatus.cutter_comp_mode == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U) ||
        ((pBlock->axis_mask & 0x3U) == 0U)) {
        return;
    }
    dx = (double)pBlock->axis_target[0] - (double)pBlock->axis_start[0];
    dy = (double)pBlock->axis_target[1] - (double)pBlock->axis_start[1];
    length = sqrt((dx * dx) + (dy * dy));
    if (length < 1.0) {
        return;
    }
    sign = (g_ncCompensationStatus.cutter_comp_mode == 1U) ? 1.0 : -1.0;
    radius = (double)g_ncCompensationStatus.cutter_radius_offset;
    nx = (-dy / length) * radius * sign;
    ny = (dx / length) * radius * sign;
    pBlock->axis_target[0] += RoundFixed(nx);
    pBlock->axis_target[1] += RoundFixed(ny);
    pBlock->axis_mask |= 0x3U;
    pBlock->feature_flags |= NC_FEATURE_FLAG_CUTTER_RADIUS;
    g_ncCompensationStatus.adjusted_axis_mask |= 0x3U;
}

static void RotateTarget(NC_EXEC_BLOCK* pBlock)
{
    double angle;
    double c;
    double s;
    double x;
    double y;
    double cx;
    double cy;

    if ((g_ncCompensationStatus.rotation_active == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U) ||
        ((pBlock->axis_mask & 0x3U) == 0U)) {
        return;
    }
    angle = ((double)g_ncCompensationStatus.rotation_angle_deg /
             (double)NC_ANGLE_SCALE) * 3.14159265358979323846 / 180.0;
    c = cos(angle);
    s = sin(angle);
    cx = (double)g_ncCompensationStatus.rotation_center[0];
    cy = (double)g_ncCompensationStatus.rotation_center[1];
    x = (double)pBlock->axis_target[0] - cx;
    y = (double)pBlock->axis_target[1] - cy;
    pBlock->axis_target[0] = RoundFixed(cx + (x * c) - (y * s));
    pBlock->axis_target[1] = RoundFixed(cy + (x * s) + (y * c));
    pBlock->axis_mask |= 0x3U;
    pBlock->feature_flags |= NC_FEATURE_FLAG_COORD_ROTATION;
    g_ncCompensationStatus.adjusted_axis_mask |= 0x3U;
}

static void ConvertPolarTarget(NC_EXEC_BLOCK* pBlock)
{
    double radius;
    double angle;
    int32_t centerX;
    int32_t centerY;

    if ((g_ncCompensationStatus.polar_active == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U) ||
        ((pBlock->axis_mask & 0x3U) != 0x3U)) {
        return;
    }
    radius = (double)pBlock->axis_target[0];
    angle = ((double)pBlock->axis_target[1] /
             (double)NC_POSITION_SCALE) * 3.14159265358979323846 / 180.0;
    centerX = g_ncCompensationStatus.rotation_center[0];
    centerY = g_ncCompensationStatus.rotation_center[1];
    pBlock->axis_target[0] = centerX + RoundFixed(radius * cos(angle));
    pBlock->axis_target[1] = centerY + RoundFixed(radius * sin(angle));
    pBlock->feature_flags |= NC_FEATURE_FLAG_POLAR_COORD;
    g_ncCompensationStatus.adjusted_axis_mask |= 0x3U;
}
int32_t NcCompensation_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                    NC_ERROR_CODE* pOutError)
{
    int32_t before[AXIS_MAX];
    uint32_t i;
    uint8_t changed = 0U;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }
    for (i = 0U; i < AXIS_MAX; ++i) {
        before[i] = pBlock->axis_target[i];
    }
    ApplyToolLengthCommand(pBlock, pOutError);
    ApplyCutterCommand(pBlock, pOutError);
    ApplyRotationCommand(pBlock);
    ApplyPolarCommand(pBlock);
    if (*pOutError != NC_ERR_NONE) {
        return -1;
    }
    ConvertPolarTarget(pBlock);
    RotateTarget(pBlock);
    ApplyToolLengthToTarget(pBlock);
    ApplyCutterRadiusToTarget(pBlock);
    for (i = 0U; i < AXIS_MAX; ++i) {
        if (before[i] != pBlock->axis_target[i]) {
            changed = 1U;
        }
    }
    if (changed != 0U) {
        g_ncCompensationStatus.applied_blocks++;
        g_ncCompensationStatus.last_line_no = pBlock->line_no;
        g_ncCompensationStatus.generation++;
    }
    return 0;
}

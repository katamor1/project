/* ibm-bob/samples/base/source/src/nc_rotary_mcc.c */
/* Implements rotary-axis virtual circumferential speed tracking and MCC command output. */
/* This exists so G07.1/G12.1 style rotary modes have deterministic shared state and RT-safe output bits. */
#include <string.h>
#include "nc_codes.h"
#include "nc_program.h"
#include "nc_rotary_mcc.h"

#define GD(whole, dec) NC_G_CODE10((whole), (dec))
/**
 * @brief Handle abs32 for this module.
 * @param value Numeric value being converted, clamped, or tested.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t Abs32(int32_t value)
{
    return (value < 0) ? -value : value;
}

/**
 * @brief Return whether motion like is true for the current block or state.
 * @param motion Motion type being tested by the helper.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
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

/**
 * @brief Set default config in this module.
 */
static void SetDefaultConfig(void)
{
    uint32_t i;

    g_ncRotaryMccStatus.enabled = 1U;
    g_ncRotaryMccStatus.mcc_output_enabled = 1U;
    g_ncRotaryMccStatus.mcc_output_active = 0U;
    g_ncRotaryMccStatus.active_rotary_axis = 3U;
    g_ncRotaryMccStatus.configured_axis_mask = NC_ROTARY_MCC_DEFAULT_AXIS_MASK;
    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncRotaryMccStatus.radius[i] = NC_ROTARY_MCC_DEFAULT_RADIUS;
    }
}

/**
 * @brief Handle virtual linear delta for this module.
 * @param axis Axis index used by the helper.
 * @param rotaryDelta Rotary-axis delta used to estimate linear movement.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t VirtualLinearDelta(uint8_t axis, int32_t rotaryDelta)
{
    int64_t radius;
    int64_t delta;
    int64_t scaled;

    radius = (int64_t)g_ncRotaryMccStatus.radius[axis];
    delta = (int64_t)Abs32(rotaryDelta);
    scaled = (delta * radius) / (int64_t)(360 * NC_ANGLE_SCALE);
    if (scaled > 2147483647LL) {
        scaled = 2147483647LL;
    }
    return (int32_t)scaled;
}

/**
 * @brief Handle first rotary axis in block for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pBlock NC execution block read or updated by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int8_t FirstRotaryAxisInBlock(const NC_EXEC_BLOCK* pBlock)
{
    uint8_t i;
    uint32_t mask;

    if (pBlock == 0) {
        return -1;
    }
    mask = pBlock->axis_mask & g_ncRotaryMccStatus.configured_axis_mask;
    for (i = 0U; i < AXIS_MAX; ++i) {
        if ((mask & (1UL << i)) != 0U) {
            return (int8_t)i;
        }
    }
    return -1;
}

/**
 * @brief Handle nc rotary mcc reset for this module.
 */
void NcRotaryMcc_Reset(void)
{
    uint32_t generation = g_ncRotaryMccStatus.generation;
    (void)memset((void*)&g_ncRotaryMccStatus, 0, sizeof(g_ncRotaryMccStatus));
    SetDefaultConfig();
    g_ncRotaryMccStatus.generation = generation + 1U;
}

/**
 * @brief Handle nc rotary mcc set axis radius for this module.
 * @param axis Axis index used by the helper.
 * @param radius Input value for radius.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcRotaryMcc_SetAxisRadius(uint8_t axis, int32_t radius)
{
    if ((axis >= AXIS_MAX) ||
        (radius < NC_ROTARY_MCC_MIN_RADIUS) ||
        (radius > NC_ROTARY_MCC_MAX_RADIUS)) {
        g_ncRotaryMccStatus.rejected_calls++;
        g_ncRotaryMccStatus.generation++;
        return -1;
    }
    g_ncRotaryMccStatus.radius[axis] = radius;
    g_ncRotaryMccStatus.configured_axis_mask |= (1UL << axis);
    g_ncRotaryMccStatus.active_rotary_axis = axis;
    g_ncRotaryMccStatus.radius_set_calls++;
    g_ncRotaryMccStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc rotary mcc set mcc output for this module.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcRotaryMcc_SetMccOutput(uint8_t enabled)
{
    g_ncRotaryMccStatus.mcc_output_enabled = (enabled != 0U) ? 1U : 0U;
    if (enabled == 0U) {
        g_ncRotaryMccStatus.mcc_output_active = 0U;
        g_ioOut.command_bits &= ~NC_ROTARY_MCC_COMMAND_BIT;
        g_ncRotaryMccStatus.mcc_off_events++;
    } else {
        g_ncRotaryMccStatus.mcc_on_events++;
    }
    g_ncRotaryMccStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc rotary mcc apply block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pOutError Output pointer that receives the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcRotaryMcc_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    /* Prepare local state used by the following processing stage. */
    int8_t axis;
    int32_t rotaryDelta;
    int32_t virtualDelta;
    uint32_t i;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }

    if ((pBlock->g_code10 == (uint16_t)GD(7, 1)) ||
        (pBlock->g_code10 == (uint16_t)GD(12, 1))) {
        g_ncRotaryMccStatus.enabled = 1U;
        g_ncRotaryMccStatus.last_g_code10 = pBlock->g_code10;
        g_ncRotaryMccStatus.last_line_no = pBlock->line_no;
        g_ncRotaryMccStatus.generation++;
        NcEvent_Push(NC_EVENT_CODE_ROTARY_MCC_MODE,
                     1,
                     (int32_t)pBlock->g_code10,
                     pBlock->line_no);
    } else if (pBlock->g_code10 == (uint16_t)GD(13, 1)) {
        g_ncRotaryMccStatus.enabled = 0U;
        g_ncRotaryMccStatus.active_axis_mask = 0U;
        g_ncRotaryMccStatus.mcc_output_active = 0U;
        g_ncRotaryMccStatus.last_g_code10 = pBlock->g_code10;
        g_ncRotaryMccStatus.last_line_no = pBlock->line_no;
        g_ncRotaryMccStatus.generation++;
        NcEvent_Push(NC_EVENT_CODE_ROTARY_MCC_MODE,
                     0,
                     (int32_t)pBlock->g_code10,
                     pBlock->line_no);
    }

    if ((g_ncRotaryMccStatus.enabled == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U)) {
        return 0;
    }

    axis = FirstRotaryAxisInBlock(pBlock);
    if (axis < 0) {
        return 0;
    }

    rotaryDelta = pBlock->axis_target[(uint8_t)axis] - pBlock->axis_start[(uint8_t)axis];
    if (rotaryDelta == 0) {
        return 0;
    }

    virtualDelta = VirtualLinearDelta((uint8_t)axis, rotaryDelta);
    pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
    if (pBlock->motion_type == NC_MOTION_NONE) {
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
    }
    g_ncRotaryMccStatus.active_axis_mask |= (1UL << (uint8_t)axis);
    g_ncRotaryMccStatus.active_rotary_axis = (uint8_t)axis;
    g_ncRotaryMccStatus.parsed_virtual_speed_blocks++;
    g_ncRotaryMccStatus.last_rotary_delta = rotaryDelta;
    /* Apply the next logical update for this processing stage. */
    g_ncRotaryMccStatus.last_virtual_linear_delta = virtualDelta;
    g_ncRotaryMccStatus.last_g_code10 = pBlock->g_code10;
    g_ncRotaryMccStatus.last_line_no = pBlock->line_no;
    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncRotaryMccStatus.last_axis_target[i] = pBlock->axis_target[i];
    }
    g_ncRotaryMccStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_ROTARY_MCC_APPLIED,
                 (int32_t)axis,
                 virtualDelta,
                 pBlock->line_no);
    return 0;
}

/**
 * @brief Handle nc rotary mcc on block rt for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcRotaryMcc_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    int8_t axis;

    if ((pBlock == 0) || (g_ncRotaryMccStatus.enabled == 0U)) {
        return;
    }
    axis = FirstRotaryAxisInBlock(pBlock);
    if (axis < 0) {
        return;
    }
    g_ncRotaryMccStatus.executed_virtual_speed_blocks++;
    g_ncRotaryMccStatus.active_axis_mask |= (1UL << (uint8_t)axis);
    g_ncRotaryMccStatus.active_rotary_axis = (uint8_t)axis;
    g_ncRotaryMccStatus.last_line_no = pBlock->line_no;
    g_ncRotaryMccStatus.generation++;
}

/**
 * @brief Handle nc rotary mcc service rt for this module.
 */
void NcRotaryMcc_ServiceRt(void)
{
    uint8_t shouldOutput;

    shouldOutput = (uint8_t)((g_ncRotaryMccStatus.enabled != 0U) &&
                             (g_ncRotaryMccStatus.mcc_output_enabled != 0U) &&
                             (g_ncRotaryMccStatus.active_axis_mask != 0U) &&
                             (g_machineCtx.machine_state == MACHINE_RUNNING) &&
                             ((g_ncProgramStatus.state == NC_RUNNING) ||
                              (g_ncProgramStatus.state == NC_READY)));
    if (shouldOutput != 0U) {
        g_ioOut.command_bits |= NC_ROTARY_MCC_COMMAND_BIT;
        if (g_ncRotaryMccStatus.mcc_output_active == 0U) {
            g_ncRotaryMccStatus.mcc_on_events++;
            NcEvent_Push(NC_EVENT_CODE_ROTARY_MCC_OUTPUT,
                         1,
                         (int32_t)g_ncRotaryMccStatus.active_axis_mask,
                         g_ncProgramStatus.exec_line_no);
        }
        g_ncRotaryMccStatus.mcc_output_active = 1U;
    } else {
        g_ioOut.command_bits &= ~NC_ROTARY_MCC_COMMAND_BIT;
        if (g_ncRotaryMccStatus.mcc_output_active != 0U) {
            g_ncRotaryMccStatus.mcc_off_events++;
            NcEvent_Push(NC_EVENT_CODE_ROTARY_MCC_OUTPUT,
                         0,
                         (int32_t)g_ncRotaryMccStatus.active_axis_mask,
                         g_ncProgramStatus.exec_line_no);
        }
        g_ncRotaryMccStatus.mcc_output_active = 0U;
    }
    g_ncRotaryMccStatus.last_mcc_command_bits = g_ioOut.command_bits;
    g_ncRotaryMccStatus.generation++;
}

/* ibm-bob/samples/base/source/src/nc_axis_config.c */
/* Applies axis definition, flexible path allocation, detached-axis and diameter/radius conversion rules. */
/* This exists so RT receives fixed axis targets without performing table lookup or axis-mode decisions. */
/* RELEVANT FILES: inc/nc_axis_config.h, src/nc_parser_modal.c, src/control_api.c, src/system_shared.c */
#include "nc_axis_config.h"
/**
 * @brief Handle axis mask all for this module.
 * @return Function-specific result value.
 */
static uint32_t AxisMaskAll(void)
{
    return (uint32_t)((1UL << AXIS_MAX) - 1UL);
}

/**
 * @brief Return whether valid mask is true for the current block or state.
 * @param axisMask Bit mask of axes to validate or translate.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsValidMask(uint32_t axisMask)
{
    return (uint8_t)((axisMask & ~AxisMaskAll()) == 0U);
}

/**
 * @brief Handle normalize name for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param axisName Axis name character to normalize.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t NormalizeName(uint8_t axisName)
{
    if ((axisName >= (uint8_t)'a') && (axisName <= (uint8_t)'z')) {
        return (uint8_t)(axisName - ((uint8_t)'a' - (uint8_t)'A'));
    }
    return axisName;
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
 * @brief Handle half away from zero for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param value Numeric value being converted, clamped, or tested.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t HalfAwayFromZero(int32_t value)
{
    if (value > 0) {
        return (value + 1) / 2;
    }
    if (value < 0) {
        return (value - 1) / 2;
    }
    return 0;
}

/**
 * @brief Handle nc axis config set axis definition for this module.
 * @param axis Axis index used by the helper.
 * @param axisName Axis name character to normalize.
 * @param axisType Input value for axis type.
 * @param coordinateGroup Input value for coordinate group.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetAxisDefinition(uint8_t axis,
                                       uint8_t axisName,
                                       uint8_t axisType,
                                       uint8_t coordinateGroup)
{
    uint32_t bit;
    axisName = NormalizeName(axisName);
    if ((axis >= AXIS_MAX) ||
        ((axisType != NC_AXIS_TYPE_LINEAR) && (axisType != NC_AXIS_TYPE_ROTARY)) ||
        (axisName < (uint8_t)'A') || (axisName > (uint8_t)'Z')) {
        g_ncAxisConfigStatus.rejected_calls++;
        g_ncAxisConfigStatus.generation++;
        return -1;
    }

    bit = (1UL << axis);
    g_ncAxisConfigStatus.axis_type[axis] = axisType;
    g_ncAxisConfigStatus.axis_name[axis] = axisName;
    g_ncAxisConfigStatus.coordinate_group[axis] = coordinateGroup;
    if (axisType == NC_AXIS_TYPE_ROTARY) {
        g_ncAxisConfigStatus.rotary_axis_mask |= bit;
        g_ncAxisConfigStatus.linear_axis_mask &= ~bit;
    } else {
        g_ncAxisConfigStatus.linear_axis_mask |= bit;
        g_ncAxisConfigStatus.rotary_axis_mask &= ~bit;
    }
    g_ncAxisConfigStatus.axis_definition_calls++;
    g_ncAxisConfigStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_AXIS_CONFIG_CHANGE,
                 (int32_t)axis,
                 (int32_t)axisName,
                 0U);
    return 0;
}

/**
 * @brief Handle nc axis config set path axis mask for this module.
 * @param axisMask Bit mask of axes to validate or translate.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetPathAxisMask(uint32_t axisMask)
{
    if ((axisMask == 0U) || (IsValidMask(axisMask) == 0U)) {
        g_ncAxisConfigStatus.rejected_calls++;
        g_ncAxisConfigStatus.generation++;
        return -1;
    }
    g_ncAxisConfigStatus.active_path_axis_mask = axisMask;
    g_ncAxisConfigStatus.path_assignment_calls++;
    g_ncAxisConfigStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_AXIS_CONFIG_CHANGE,
                 (int32_t)axisMask,
                 0,
                 0U);
    return 0;
}

/**
 * @brief Handle nc axis config set detached mask for this module.
 * @param axisMask Bit mask of axes to validate or translate.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetDetachedMask(uint32_t axisMask)
{
    if (IsValidMask(axisMask) == 0U) {
        g_ncAxisConfigStatus.rejected_calls++;
        g_ncAxisConfigStatus.generation++;
        return -1;
    }
    g_ncAxisConfigStatus.detached_axis_mask = axisMask;
    g_ncAxisConfigStatus.detached_set_calls++;
    g_ncAxisConfigStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_AXIS_CONFIG_CHANGE,
                 (int32_t)axisMask,
                 1,
                 0U);
    return 0;
}

/**
 * @brief Handle nc axis config set diameter mode for this module.
 * @param axis Axis index used by the helper.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetDiameterMode(uint8_t axis, uint8_t enabled)
{
    uint32_t bit;
    if (axis >= AXIS_MAX) {
        g_ncAxisConfigStatus.rejected_calls++;
        g_ncAxisConfigStatus.generation++;
        return -1;
    }
    bit = (1UL << axis);
    if (enabled != 0U) {
        g_ncAxisConfigStatus.diameter_axis_mask |= bit;
    } else {
        g_ncAxisConfigStatus.diameter_axis_mask &= ~bit;
    }
    g_ncAxisConfigStatus.diameter_mode_calls++;
    g_ncAxisConfigStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_AXIS_DIAMETER_MODE,
                 (int32_t)axis,
                 (int32_t)(enabled != 0U),
                 0U);
    return 0;
}

/**
 * @brief Handle nc axis config apply block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pOutError Output pointer that receives the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                  NC_ERROR_CODE* pOutError)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t i;
    uint32_t pathMask;
    uint32_t inputMask;
    uint32_t removeMask;
    uint8_t changed = 0U;
    uint8_t diameterChanged = 0U;
    uint8_t detachedChanged = 0U;
    uint8_t pathChanged = 0U;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }
    if (IsMotionLike(pBlock->motion_type) == 0U) {
        return 0;
    }

    pathMask = g_ncAxisConfigStatus.active_path_axis_mask;
    if (pathMask == 0U) {
        pathMask = NC_AXIS_CONFIG_DEFAULT_PATH_MASK;
    }
    inputMask = pBlock->axis_mask;
    removeMask = (inputMask & ~pathMask) |
                 (inputMask & g_ncAxisConfigStatus.detached_axis_mask);

    g_ncAxisConfigStatus.last_input_axis_mask = inputMask;
    for (i = 0U; i < AXIS_MAX; ++i) {
        uint32_t bit = (1UL << i);
        g_ncAxisConfigStatus.last_input_target[i] = pBlock->axis_target[i];

        if (((g_ncAxisConfigStatus.diameter_axis_mask & bit) != 0U) &&
            ((pBlock->axis_mask & bit) != 0U)) {
            pBlock->axis_target[i] = HalfAwayFromZero(pBlock->axis_target[i]);
            pBlock->axis_start[i] = HalfAwayFromZero(pBlock->axis_start[i]);
            pBlock->arc_center[i] = HalfAwayFromZero(pBlock->arc_center[i]);
            changed = 1U;
            diameterChanged = 1U;
        }
        if ((removeMask & bit) != 0U) {
            pBlock->axis_target[i] = pBlock->axis_start[i];
            pBlock->axis_mask &= ~bit;
            changed = 1U;
            detachedChanged = 1U;
            if ((inputMask & ~pathMask & bit) != 0U) {
                pathChanged = 1U;
            }
        }
        g_ncAxisConfigStatus.last_output_target[i] = pBlock->axis_target[i];
    }
    g_ncAxisConfigStatus.last_output_axis_mask = pBlock->axis_mask;

    /* Handle the next conditional branch for this processing stage. */
    if (changed != 0U) {
        g_ncAxisConfigStatus.applied_blocks++;
        g_ncAxisConfigStatus.last_line_no = pBlock->line_no;
        if (diameterChanged != 0U) {
            g_ncAxisConfigStatus.diameter_converted_blocks++;
            NcEvent_Push(NC_EVENT_CODE_AXIS_DIAMETER_MODE,
                         (int32_t)g_ncAxisConfigStatus.diameter_axis_mask,
                         (int32_t)pBlock->axis_mask,
                         pBlock->line_no);
        }
        if (detachedChanged != 0U) {
            g_ncAxisConfigStatus.detached_axis_blocks++;
            NcEvent_Push(NC_EVENT_CODE_AXIS_DETACHED,
                         (int32_t)removeMask,
                         (int32_t)pBlock->axis_mask,
                         pBlock->line_no);
        }
        if (pathChanged != 0U) {
            g_ncAxisConfigStatus.path_filtered_blocks++;
        }
        g_ncAxisConfigStatus.generation++;
        NcEvent_Push(NC_EVENT_CODE_AXIS_CONFIG_APPLIED,
                     (int32_t)inputMask,
                     (int32_t)pBlock->axis_mask,
                     pBlock->line_no);
    }

    (void)pOutError;
    return 0;
}

/* ibm-bob/samples/base/source/src/nc_interference.c */
/* Implements deterministic TS-side screening for path interference and axis envelope warnings. */
#include <stdlib.h>
#include <string.h>
#include "nc_interference.h"
#include "nc_program.h"
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
 * @brief Handle nc interference reset for this module.
 */
void NcInterference_Reset(void)
{
    (void)memset((void*)&g_ncInterferenceStatus, 0, sizeof(g_ncInterferenceStatus));
    g_ncInterferenceStatus.enabled = 1U;
    g_ncInterferenceStatus.generation++;
}

/**
 * @brief Handle nc interference set enabled for this module.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterference_SetEnabled(uint8_t enabled)
{
    g_ncInterferenceStatus.enabled = (uint8_t)(enabled != 0U);
    g_ncInterferenceStatus.generation++;
    return 0;
}

/**
 * @brief Apply warning to the current block or state.
 * @param pBlock NC execution block read or updated by the helper.
 * @param kind Segment or warning kind handled by the helper.
 * @param value Numeric value being converted, clamped, or tested.
 */
static void ApplyWarning(NC_EXEC_BLOCK* pBlock, uint8_t kind, int32_t value)
{
    g_ncInterferenceStatus.last_warning_kind = kind;
    g_ncInterferenceStatus.last_warning_line_no = pBlock->line_no;
    g_ncInterferenceStatus.slowdown_applied_blocks++;
    if ((pBlock->feed_override_percent == 0U) ||
        (pBlock->feed_override_percent > NC_INTERFERENCE_SLOWDOWN_PERCENT)) {
        pBlock->feed_override_percent = NC_INTERFERENCE_SLOWDOWN_PERCENT;
    }
    NcEvent_Push(NC_EVENT_CODE_INTERFERENCE_WARN,
                 (int32_t)kind,
                 value,
                 pBlock->line_no);
}

/**
 * @brief Handle nc interference check block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcInterference_CheckBlockTs(NC_EXEC_BLOCK* pBlock)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t axis;
    uint32_t movingAxes = 0U;
    int32_t minClearance = NC_INTERFERENCE_AXIS_LIMIT;

    if ((pBlock == NULL) || (g_ncInterferenceStatus.enabled == 0U)) {
        return;
    }
    if ((pBlock->motion_type != NC_MOTION_RAPID) &&
        (pBlock->motion_type != NC_MOTION_LINEAR) &&
        (pBlock->motion_type != NC_MOTION_ARC_CW) &&
        (pBlock->motion_type != NC_MOTION_ARC_CCW) &&
        (pBlock->motion_type != NC_MOTION_ADVANCED_INTERP)) {
        return;
    }

    /* Apply the next logical update for this processing stage. */
    g_ncInterferenceStatus.checked_blocks++;
    g_ncInterferenceStatus.last_line_no = pBlock->line_no;

    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        int32_t target = pBlock->axis_target[axis];
        int32_t delta = Abs32(target - pBlock->axis_start[axis]);
        int32_t clearance = NC_INTERFERENCE_AXIS_LIMIT - Abs32(target);
        g_ncInterferenceStatus.last_clearance[axis] = clearance;
        if (delta != 0) {
            movingAxes++;
        }
        if (clearance < minClearance) {
            minClearance = clearance;
        }
        if (clearance < 0) {
            g_ncInterferenceStatus.axis_limit_warnings++;
            ApplyWarning(pBlock, 3U, (int32_t)axis);
        }
    }

    if ((movingAxes >= 3U) && (minClearance < NC_INTERFERENCE_MIN_CLEARANCE)) {
        g_ncInterferenceStatus.three_d_interference_warnings++;
        ApplyWarning(pBlock, 2U, minClearance);
    } else if ((movingAxes >= 2U) &&
               (pBlock->motion_type == NC_MOTION_ARC_CW ||
                pBlock->motion_type == NC_MOTION_ARC_CCW) &&
               (pBlock->arc_radius != 0) &&
               (Abs32(pBlock->arc_radius) < NC_INTERFERENCE_MIN_CLEARANCE)) {
        g_ncInterferenceStatus.path_interference_warnings++;
        ApplyWarning(pBlock, 1U, pBlock->arc_radius);
    }
    g_ncInterferenceStatus.generation++;
}

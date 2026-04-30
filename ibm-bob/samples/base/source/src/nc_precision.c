/* ibm-bob/samples/base/source/src/nc_precision.c */
/* Implements fixed-size high-precision learning, vibration, and preview status. */
/* This exists so TS preview and RT correction remain inspectable and RT-safe. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_precision.h, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#include <string.h>
#include "nc_precision.h"
#include "nc_program.h"

static int32_t s_learning_error_memory[AXIS_MAX][NC_PRECISION_LEARNING_MEMORY_MAX];
static uint8_t s_learning_write_index;
static int32_t s_last_output_target[AXIS_MAX];
static uint8_t s_have_last_output;

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
 * @brief Handle clamp memory window for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param memoryWindow Requested learning memory window size.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t ClampMemoryWindow(uint8_t memoryWindow)
{
    if (memoryWindow == 0U) {
        return 1U;
    }
    if (memoryWindow > NC_PRECISION_LEARNING_MEMORY_MAX) {
        return NC_PRECISION_LEARNING_MEMORY_MAX;
    }
    return memoryWindow;
}

/**
 * @brief Handle nc precision reset for this module.
 */
void NcPrecision_Reset(void)
{
    (void)memset((void*)&g_ncPrecisionStatus, 0, sizeof(g_ncPrecisionStatus));
    (void)memset(s_learning_error_memory, 0, sizeof(s_learning_error_memory));
    (void)memset(s_last_output_target, 0, sizeof(s_last_output_target));
    s_learning_write_index = 0U;
    s_have_last_output = 0U;
    g_ncPrecisionStatus.learning_gain_percent = NC_PRECISION_LEARNING_GAIN_DEFAULT;
    g_ncPrecisionStatus.learning_memory_window = NC_PRECISION_LEARNING_MEMORY_DEFAULT;
    g_ncPrecisionStatus.vibration_notch_freq_hz = NC_PRECISION_VIBRATION_NOTCH_DEFAULT_HZ;
    g_ncPrecisionStatus.vibration_damping_percent = NC_PRECISION_VIBRATION_DAMPING_DEFAULT;
    g_ncPrecisionStatus.preview_lookahead_blocks = NC_PRECISION_PREVIEW_LOOKAHEAD_DEFAULT;
    g_ncPrecisionStatus.preview_corner_tolerance = NC_PRECISION_PREVIEW_CORNER_TOLERANCE_DEFAULT;
    g_ncPrecisionStatus.generation++;
}

/**
 * @brief Handle nc precision set learning control for this module.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @param gainPercent Input value for gain percent.
 * @param memoryWindow Requested learning memory window size.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcPrecision_SetLearningControl(uint8_t enabled,
                                       int32_t gainPercent,
                                       uint8_t memoryWindow)
{
    if ((gainPercent < 0) || (gainPercent > NC_PRECISION_LEARNING_GAIN_MAX)) {
        return -1;
    }
    g_ncPrecisionStatus.learning_enabled = (uint8_t)(enabled != 0U);
    g_ncPrecisionStatus.learning_gain_percent = gainPercent;
    g_ncPrecisionStatus.learning_memory_window = ClampMemoryWindow(memoryWindow);
    g_ncPrecisionStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc precision set vibration control for this module.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @param notchFreqHz Input value for notch freq hz.
 * @param dampingPercent Input value for damping percent.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcPrecision_SetVibrationControl(uint8_t enabled,
                                        uint16_t notchFreqHz,
                                        uint16_t dampingPercent)
{
    if ((notchFreqHz > NC_PRECISION_VIBRATION_FREQ_MAX_HZ) ||
        (dampingPercent > NC_PRECISION_VIBRATION_DAMPING_MAX)) {
        return -1;
    }
    g_ncPrecisionStatus.vibration_suppression_enabled = (uint8_t)(enabled != 0U);
    g_ncPrecisionStatus.vibration_notch_freq_hz = notchFreqHz;
    g_ncPrecisionStatus.vibration_damping_percent = dampingPercent;
    g_ncPrecisionStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc precision set preview control for this module.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @param lookaheadBlocks Input value for lookahead blocks.
 * @param cornerTolerance Input value for corner tolerance.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcPrecision_SetPreviewControl(uint8_t enabled,
                                      uint16_t lookaheadBlocks,
                                      uint16_t cornerTolerance)
{
    if ((lookaheadBlocks == 0U) ||
        (lookaheadBlocks > NC_PRECISION_PREVIEW_LOOKAHEAD_MAX) ||
        (cornerTolerance > NC_PRECISION_PREVIEW_CORNER_TOLERANCE_MAX)) {
        return -1;
    }
    g_ncPrecisionStatus.preview_control_enabled = (uint8_t)(enabled != 0U);
    g_ncPrecisionStatus.preview_lookahead_blocks = lookaheadBlocks;
    g_ncPrecisionStatus.preview_corner_tolerance = cornerTolerance;
    g_ncPrecisionStatus.generation++;
    return 0;
}

/**
 * @brief Update mode from feature from current inputs.
 * @param pBlock NC execution block read or updated by the helper.
 */
static void UpdateModeFromFeature(const NC_EXEC_BLOCK* pBlock)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t flags = pBlock->feature_flags;

    if (pBlock->g_code10 == (uint16_t)NC_G_CODE_WHOLE(8)) {
        if (pBlock->p_word == 0U) {
            g_ncPrecisionStatus.preview_control_enabled = 0U;
            g_ncPrecisionStatus.high_precision_contour_active = 0U;
            g_ncPrecisionStatus.active_mode_code10 = 0U;
        } else if (pBlock->p_word == 1U) {
            g_ncPrecisionStatus.preview_control_enabled = 1U;
            g_ncPrecisionStatus.high_precision_contour_active = 0U;
            g_ncPrecisionStatus.active_mode_code10 = (uint32_t)pBlock->g_code10;
        } else if (pBlock->p_word == 2U) {
            g_ncPrecisionStatus.preview_control_enabled = 1U;
            g_ncPrecisionStatus.high_precision_contour_active = 1U;
            g_ncPrecisionStatus.active_mode_code10 = (uint32_t)pBlock->g_code10;
        }
    }

    if (pBlock->g_code10 == (uint16_t)NC_G_CODE_WHOLE(5)) {
        if (pBlock->p_word == 0U) {
            g_ncPrecisionStatus.preview_control_enabled = 0U;
            g_ncPrecisionStatus.high_precision_contour_active = 0U;
            g_ncPrecisionStatus.high_speed_cycle_active = 0U;
            g_ncPrecisionStatus.active_mode_code10 = 0U;
        } else if (pBlock->p_word == 10000U) {
            g_ncPrecisionStatus.preview_control_enabled = 1U;
            g_ncPrecisionStatus.high_precision_contour_active = 1U;
            g_ncPrecisionStatus.active_mode_code10 = (uint32_t)pBlock->g_code10;
            g_ncPrecisionStatus.hpcc_mode_blocks++;
        } else if (pBlock->p_word == 3U) {
            g_ncPrecisionStatus.high_speed_cycle_active = 1U;
            g_ncPrecisionStatus.active_mode_code10 = (uint32_t)pBlock->g_code10;
        }
    }

    /* Handle the next conditional branch for this processing stage. */
    if ((pBlock->g_code10 == (uint16_t)NC_G_CODE10(5, 1)) ||
        (pBlock->g_code10 == (uint16_t)NC_G_CODE10(5, 2))) {
        g_ncPrecisionStatus.preview_control_enabled = 1U;
        g_ncPrecisionStatus.high_precision_contour_active = 1U;
        g_ncPrecisionStatus.learning_enabled = 1U;
        g_ncPrecisionStatus.vibration_suppression_enabled = 1U;
        g_ncPrecisionStatus.active_mode_code10 = (uint32_t)pBlock->g_code10;
        if (pBlock->g_code10 == (uint16_t)NC_G_CODE10(5, 2)) {
            g_ncPrecisionStatus.free_surface_mode_active = 1U;
        }
    }

    if ((flags & NC_FEATURE_FLAG_HIGH_PRECISION) != 0U) {
        g_ncPrecisionStatus.high_precision_contour_active = 1U;
    }
    if ((flags & NC_FEATURE_FLAG_SMOOTHING) != 0U) {
        g_ncPrecisionStatus.preview_control_enabled = 1U;
    }
}

/**
 * @brief Update preview shape ts from current inputs.
 * @param pBlock NC execution block read or updated by the helper.
 */
static void UpdatePreviewShapeTs(const NC_EXEC_BLOCK* pBlock)
{
    int32_t maxDelta = 0;
    uint32_t axis;

    if (pBlock->axis_mask == 0U) {
        return;
    }
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
            int32_t delta = Abs32(pBlock->axis_target[axis] - pBlock->axis_start[axis]);
            if (delta > maxDelta) {
                maxDelta = delta;
            }
        }
    }

    g_ncPrecisionStatus.previewed_blocks++;
    g_ncPrecisionStatus.last_preview_line_no = pBlock->line_no;
    g_ncPrecisionStatus.last_path_delta = maxDelta;
    if ((g_ncPrecisionStatus.preview_control_enabled != 0U) ||
        ((pBlock->feature_flags & (NC_FEATURE_FLAG_HIGH_PRECISION |
                                   NC_FEATURE_FLAG_SMOOTHING |
                                   NC_FEATURE_FLAG_HPCC |
                                   NC_FEATURE_FLAG_HIGH_SPEED_MODE)) != 0U)) {
        g_ncPrecisionStatus.preview_feed_limited_blocks++;
        if (pBlock->feed_override_percent < g_ncPrecisionStatus.last_preview_override_percent ||
            g_ncPrecisionStatus.last_preview_override_percent == 0U) {
            g_ncPrecisionStatus.last_preview_override_percent = pBlock->feed_override_percent;
        }
    }
}

/**
 * @brief Handle nc precision on parsed block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcPrecision_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock == 0) {
        return;
    }
    UpdateModeFromFeature(pBlock);
    UpdatePreviewShapeTs(pBlock);
    g_ncPrecisionStatus.generation++;
}

/**
 * @brief Build learning correction from current shared state.
 * @param axis Axis index used by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t BuildLearningCorrection(uint32_t axis)
{
    int32_t sum = 0;
    uint8_t count = g_ncPrecisionStatus.learning_memory_window;
    uint8_t i;

    count = ClampMemoryWindow(count);
    for (i = 0U; i < count; ++i) {
        sum += s_learning_error_memory[axis][i];
    }
    if (count != 0U) {
        sum /= (int32_t)count;
    }
    return (sum * g_ncPrecisionStatus.learning_gain_percent) / 100;
}

/**
 * @brief Handle nc precision on block rt for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcPrecision_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t axis;

    if (pBlock == 0) {
        return;
    }
    UpdateModeFromFeature(pBlock);
    g_ncPrecisionStatus.last_exec_line_no = pBlock->line_no;

    if (pBlock->motion_type == NC_MOTION_ADVANCED_INTERP) {
        g_ncPrecisionStatus.advanced_precision_blocks++;
    }
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_HIGH_SPEED_MODE) != 0U) {
        g_ncPrecisionStatus.high_speed_cycle_blocks++;
    }
    /* Handle the next conditional branch for this processing stage. */
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_HPCC) != 0U) {
        g_ncPrecisionStatus.hpcc_mode_blocks++;
    }

    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        if ((pBlock->axis_mask & (1UL << axis)) != 0U) {
            int32_t command = pBlock->axis_target[axis];
            int32_t output = g_ioOut.axis_target[axis];
            int32_t error = command - output;
            int32_t correction = 0;

            s_learning_error_memory[axis][s_learning_write_index] = error;
            g_ncPrecisionStatus.last_following_error[axis] = error;
            if (Abs32(error) > g_ncPrecisionStatus.max_following_error[axis]) {
                g_ncPrecisionStatus.max_following_error[axis] = Abs32(error);
            }
            if (g_ncPrecisionStatus.learning_enabled != 0U) {
                correction = BuildLearningCorrection(axis);
                g_ncPrecisionStatus.last_learning_correction[axis] = correction;
                g_ncPrecisionStatus.learning_updates++;
            }
            if (g_ncPrecisionStatus.vibration_suppression_enabled != 0U) {
                int32_t vibrationDelta = 0;
                if (s_have_last_output != 0U) {
                    vibrationDelta = command - s_last_output_target[axis];
                }
                g_ncPrecisionStatus.last_vibration_delta[axis] = vibrationDelta;
                if (Abs32(vibrationDelta) > NC_PRECISION_VIBRATION_WARN_DELTA) {
                    g_ncPrecisionStatus.vibration_warnings++;
                    NcEvent_Push(NC_EVENT_CODE_PRECISION_VIBRATION_WARN,
                                 (int32_t)axis,
                                 vibrationDelta,
                                 pBlock->line_no);
                }
                g_ncPrecisionStatus.vibration_filtered_samples++;
            }
            s_last_output_target[axis] = command + correction;
        }
    }
    s_have_last_output = 1U;
    s_learning_write_index = (uint8_t)((s_learning_write_index + 1U) %
                                       NC_PRECISION_LEARNING_MEMORY_MAX);
    g_ncPrecisionStatus.generation++;
}

/**
 * @brief Handle nc precision service rt for this module.
 */
void NcPrecision_ServiceRt(void)
{
    uint32_t axis;

    if ((g_ncPrecisionStatus.learning_enabled == 0U) &&
        (g_ncPrecisionStatus.vibration_suppression_enabled == 0U) &&
        (g_ncPrecisionStatus.preview_control_enabled == 0U)) {
        return;
    }

    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        int32_t correction = g_ncPrecisionStatus.last_learning_correction[axis];
        if ((g_ncPrecisionStatus.learning_enabled != 0U) && (correction != 0)) {
            int32_t adjusted = g_ioOut.axis_target[axis] + correction;
            if (adjusted > NC_PRECISION_AXIS_TARGET_LIMIT) {
                adjusted = NC_PRECISION_AXIS_TARGET_LIMIT;
            } else if (adjusted < -NC_PRECISION_AXIS_TARGET_LIMIT) {
                adjusted = -NC_PRECISION_AXIS_TARGET_LIMIT;
            }
            g_ioOut.axis_target[axis] = adjusted;
        }
    }
    g_ncPrecisionStatus.rt_service_ticks++;
    g_ncPrecisionStatus.generation++;
}

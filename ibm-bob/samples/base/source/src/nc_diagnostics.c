/* ibm-bob/samples/base/source/src/nc_diagnostics.c */
/* Implements RT-safe diagnostics for axis overload prognosis and tool life. */
/* This exists so monitoring logic can be exposed through fixed-size shared memory. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_diagnostics.h, ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "nc_diagnostics.h"
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
 * @brief Handle override for level for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param level Diagnostic severity level used to choose the override.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static uint16_t OverrideForLevel(uint8_t level)
{
    if (level >= 3U) {
        return 25U;
    }
    if (level == 2U) {
        return 45U;
    }
    if (level == 1U) {
        return 70U;
    }
    return NC_FEED_OVERRIDE_DEFAULT_PERCENT;
}

/**
 * @brief Handle nc diagnostics update axis load rt for this module.
 */
void NcDiagnostics_UpdateAxisLoadRt(void)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t i;
    uint32_t warningMask = 0U;
    uint32_t limitMask = 0U;
    uint32_t criticalMask = 0U;
    uint8_t level = 0U;

    for (i = 0U; i < AXIS_MAX; ++i) {
        int32_t raw = 0;
        int32_t filtered;

        if ((i + 1U) < ANALOG_CH_MAX) {
            raw = Abs32(g_ioIn.analog[i + 1U]);
        }
        filtered = ((g_ncAxisLoadStatus.filtered_load[i] *
                     (NC_AXIS_LOAD_FILTER_DIV - 1)) + raw) /
                   NC_AXIS_LOAD_FILTER_DIV;
        g_ncAxisLoadStatus.current_load[i] = raw;
        g_ncAxisLoadStatus.filtered_load[i] = filtered;
        if (filtered > g_ncAxisLoadStatus.peak_load[i]) {
            g_ncAxisLoadStatus.peak_load[i] = filtered;
        }
        if (filtered >= NC_AXIS_LOAD_WARN_THRESHOLD) {
            warningMask |= (1UL << i);
            if (level < 1U) {
                level = 1U;
            }
        }
        if (filtered >= NC_AXIS_LOAD_LIMIT_THRESHOLD) {
            limitMask |= (1UL << i);
            if (level < 2U) {
                level = 2U;
            }
        }
        if (filtered >= NC_AXIS_LOAD_CRITICAL_THRESHOLD) {
            criticalMask |= (1UL << i);
            level = 3U;
        }
    }

    if ((warningMask != 0U) && (g_ncAxisLoadStatus.warning_mask == 0U)) {
        g_ncAxisLoadStatus.warning_events++;
        g_ncAxisLoadStatus.last_warning_cycle = g_machineCtx.cycle_count;
        (void)LogQueue_Push(LOG_WARN, 7101U, (int32_t)warningMask);
    }
    if ((limitMask != 0U) && (g_ncAxisLoadStatus.limit_mask == 0U)) {
        g_ncAxisLoadStatus.limit_events++;
        (void)LogQueue_Push(LOG_WARN, 7102U, (int32_t)limitMask);
    }
    /* Handle the next conditional branch for this processing stage. */
    if ((criticalMask != 0U) && (g_ncAxisLoadStatus.critical_mask == 0U)) {
        g_ncAxisLoadStatus.critical_events++;
        (void)LogQueue_Push(LOG_ALARM, 7103U, (int32_t)criticalMask);
    }

    g_ncAxisLoadStatus.warning_mask = warningMask;
    g_ncAxisLoadStatus.limit_mask = limitMask;
    g_ncAxisLoadStatus.critical_mask = criticalMask;
    g_ncAxisLoadStatus.staged_decel_level = level;
    g_ncAxisLoadStatus.decel_request = (uint8_t)(level != 0U);
    g_ncAxisLoadStatus.decel_override_percent = OverrideForLevel(level);
    g_ncAxisLoadStatus.generation++;
}

/**
 * @brief Handle nc diagnostics set tool life limit for this module.
 * @param toolNo Tool number being updated in tool-life status.
 * @param limit Symmetric limit applied to the value.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcDiagnostics_SetToolLifeLimit(uint16_t toolNo, uint32_t limit)
{
    if ((toolNo == 0U) || (toolNo >= NC_TOOL_LIFE_TABLE_SIZE) ||
        (limit == 0U)) {
        return -1;
    }
    g_ncToolLifeStatus.tool_life_limit[toolNo] = limit;
    g_ncToolLifeStatus.generation++;
    return 0;
}

/**
 * @brief Update tool life from current inputs.
 * @param toolNo Tool number being updated in tool-life status.
 * @param lineNo NC source line number associated with the update.
 * @param mCode M-code associated with the tool-life update.
 */
static void UpdateToolLife(uint32_t toolNo, uint32_t lineNo, uint32_t mCode)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t limit;
    uint32_t count;
    uint32_t warnAt;
    uint32_t bit;

    if ((toolNo == 0U) || (toolNo >= NC_TOOL_LIFE_TABLE_SIZE)) {
        return;
    }

    bit = (1UL << toolNo);
    limit = g_ncToolLifeStatus.tool_life_limit[toolNo];
    if (limit == 0U) {
        limit = NC_TOOL_LIFE_DEFAULT_LIMIT;
        g_ncToolLifeStatus.tool_life_limit[toolNo] = limit;
    }
    count = g_ncToolLifeStatus.tool_use_count[toolNo] + 1U;
    /* Apply the next logical update for this processing stage. */
    g_ncToolLifeStatus.tool_use_count[toolNo] = count;
    g_ncToolLifeStatus.active_tool_no = toolNo;
    g_ncToolLifeStatus.last_tool_no = toolNo;
    g_ncToolLifeStatus.last_m_code = mCode;
    g_ncToolLifeStatus.last_update_line_no = lineNo;

    warnAt = (limit * NC_TOOL_LIFE_WARNING_PERCENT) / 100U;
    if (warnAt == 0U) {
        warnAt = 1U;
    }
    if ((count >= warnAt) && ((g_ncToolLifeStatus.warning_mask & bit) == 0U)) {
        g_ncToolLifeStatus.warning_mask |= bit;
        g_ncToolLifeStatus.warning_events++;
        (void)LogQueue_Push(LOG_WARN, 7201U, (int32_t)toolNo);
    }
    if ((count >= limit) && ((g_ncToolLifeStatus.expired_mask & bit) == 0U)) {
        g_ncToolLifeStatus.expired_mask |= bit;
        g_ncToolLifeStatus.expired_events++;
        (void)LogQueue_Push(LOG_WARN, 7202U, (int32_t)toolNo);
    }
    g_ncToolLifeStatus.generation++;
}

/**
 * @brief Handle nc diagnostics on block rt for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcDiagnostics_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t toolNo;

    if (pBlock == 0) {
        return;
    }

    toolNo = pBlock->tool_no;
    if ((toolNo != 0U) && (toolNo < NC_TOOL_LIFE_TABLE_SIZE)) {
        g_ncToolLifeStatus.active_tool_no = toolNo;
        g_ncToolLifeStatus.last_tool_no = toolNo;
        if (((pBlock->aux_flags & NC_AUX_FLAG_TOOL) != 0U) ||
            (pBlock->aux_m_code == 6U)) {
            UpdateToolLife(toolNo, pBlock->line_no, pBlock->aux_m_code);
            return;
        }
    }

    toolNo = g_ncToolLifeStatus.active_tool_no;
    if ((toolNo != 0U) && (pBlock->motion_type != NC_MOTION_NONE) &&
        (pBlock->motion_type != NC_MOTION_DWELL)) {
        UpdateToolLife(toolNo, pBlock->line_no, pBlock->aux_m_code);
    }
}

/**
 * @brief Handle nc diagnostics build snapshot for this module.
 */
void NcDiagnostics_BuildSnapshot(void)
{
    /* Apply the next logical update for this processing stage. */
    g_ncDiagnosticSnapshot.cycle_count = g_machineCtx.cycle_count;
    g_ncDiagnosticSnapshot.run_mode = g_machineCtx.run_mode;
    g_ncDiagnosticSnapshot.machine_state = g_machineCtx.machine_state;
    g_ncDiagnosticSnapshot.nc_state = g_ncProgramStatus.state;
    g_ncDiagnosticSnapshot.alarm_code = g_machineCtx.alarm_code;
    g_ncDiagnosticSnapshot.prestart_mask = g_prestartInterlockStatus.mask;
    g_ncDiagnosticSnapshot.buffered_blocks = g_ncProgramStatus.buffered_blocks;
    g_ncDiagnosticSnapshot.exec_line_no = g_ncProgramStatus.exec_line_no;
    g_ncDiagnosticSnapshot.read_line_no = g_ncProgramStatus.read_line_no;
    g_ncDiagnosticSnapshot.axis_warning_mask = g_ncAxisLoadStatus.warning_mask;
    g_ncDiagnosticSnapshot.axis_limit_mask = g_ncAxisLoadStatus.limit_mask;
    g_ncDiagnosticSnapshot.axis_critical_mask = g_ncAxisLoadStatus.critical_mask;
    g_ncDiagnosticSnapshot.staged_decel_level = g_ncAxisLoadStatus.staged_decel_level;
    g_ncDiagnosticSnapshot.decel_override_percent = g_ncAxisLoadStatus.decel_override_percent;
    g_ncDiagnosticSnapshot.tool_warning_mask = g_ncToolLifeStatus.warning_mask;
    g_ncDiagnosticSnapshot.tool_expired_mask = g_ncToolLifeStatus.expired_mask;
    g_ncDiagnosticSnapshot.active_tool_no = g_ncToolLifeStatus.active_tool_no;
    g_ncDiagnosticSnapshot.completed_segments = g_ncInterpStatus.completed_segments;
    g_ncDiagnosticSnapshot.completed_cycles = g_ncCycleStatus.completed_cycles;
    g_ncDiagnosticSnapshot.aux_wait_cycles = g_ncAuxStatus.last_wait_cycles;
    /* Apply the next logical update for this processing stage. */
    g_ncDiagnosticSnapshot.trace_frozen = g_ioTraceBuffer.frozen;
    g_ncDiagnosticSnapshot.generation++;
}

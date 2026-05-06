#include "nc_feature_backlog_tool_utility_monitors.h"

#include <string.h>

#define V39_BIT(index_) (1u << (uint32_t)(index_))
#define V39_DEFAULT_FEED_OVERRIDE_PERCENT       (100)
#define V39_MIN_FEED_OVERRIDE_PERCENT           (5)
#define V39_SPINDLE_TEMP_WARN_C                 (72)
#define V39_SPINDLE_TEMP_HOLD_C                 (82)
#define V39_SPINDLE_TEMP_TIMEOUT_TICKS          (2u)
#define V39_LINEAR_SCALE_DIRTY_TIMEOUT_TICKS    (2u)
#define V39_TOOL_CLAMP_TIMEOUT_TICKS            (3u)
#define V39_ROTARY_TABLE_CLAMP_TIMEOUT_TICKS    (3u)
#define V39_COOLANT_FILTER_TIMEOUT_TICKS        (3u)
#define V39_DUST_COLLECTOR_TIMEOUT_TICKS        (3u)
#define V39_SPINDLE_OVERRIDE_LOCK_TIMEOUT_TICKS (1u)
#define V39_ATC_TOOL_NO_TIMEOUT_TICKS           (2u)
#define V39_WORK_PROBE_READY_TIMEOUT_TICKS      (4u)
#define V39_UPS_VOLTAGE_LOW_TIMEOUT_TICKS       (1u)

typedef struct V39_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V39_POLICY policy;
    uint32_t category;
    const char *name;
} V39_FEATURE_DEF;

static const V39_FEATURE_DEF k_v39_defs[NC_FEATURE_BACKLOG_V39_FEATURE_COUNT] = {
    { NC_FEATURE_V39_SPINDLE_BEARING_TEMP_SLOWDOWN,          NC_FEATURE_BACKLOG_V39_POLICY_SLOWDOWN, 9u, "spindle bearing temperature slowdown" },
    { NC_FEATURE_V39_LINEAR_SCALE_DIRTY_TRACE,               NC_FEATURE_BACKLOG_V39_POLICY_TRACE,    9u, "linear scale dirty trace" },
    { NC_FEATURE_V39_TOOL_CLAMP_DRAWBAR_CONFIRM_INTERLOCK,   NC_FEATURE_BACKLOG_V39_POLICY_INTERLOCK,9u, "tool clamp drawbar confirmation interlock" },
    { NC_FEATURE_V39_ROTARY_TABLE_CLAMP_CONFIRM_GATE,        NC_FEATURE_BACKLOG_V39_POLICY_GATE,     9u, "rotary table clamp confirmation gate" },
    { NC_FEATURE_V39_COOLANT_FILTER_DIFF_PRESSURE_HOLD,      NC_FEATURE_BACKLOG_V39_POLICY_HOLD,     9u, "coolant filter differential pressure hold" },
    { NC_FEATURE_V39_DUST_COLLECTOR_AIRFLOW_LOW_GATE,        NC_FEATURE_BACKLOG_V39_POLICY_GATE,     9u, "dust collector airflow low gate" },
    { NC_FEATURE_V39_SPINDLE_OVERRIDE_LOCK_ZONE_INHIBIT,     NC_FEATURE_BACKLOG_V39_POLICY_INHIBIT,  9u, "spindle override lock zone inhibit" },
    { NC_FEATURE_V39_ATC_TOOL_NUMBER_MISMATCH_HOLD,          NC_FEATURE_BACKLOG_V39_POLICY_HOLD,     9u, "ATC tool number mismatch hold" },
    { NC_FEATURE_V39_WORK_PROBE_READY_TIMEOUT_RETRY,         NC_FEATURE_BACKLOG_V39_POLICY_RETRY,    9u, "work probe ready timeout retry" },
    { NC_FEATURE_V39_UPS_VOLTAGE_LOW_DECEL_STOP,             NC_FEATURE_BACKLOG_V39_POLICY_SLOWDOWN, 9u, "UPS voltage low deceleration stop" }
};

NC_FEATURE_BACKLOG_V39_STATUS g_ncFeatureBacklogV39Status;

static uint32_t s_spindle_temp_wait_ticks;
static uint32_t s_linear_scale_dirty_wait_ticks;
static uint32_t s_tool_clamp_wait_ticks;
static uint32_t s_rotary_table_clamp_wait_ticks;
static uint32_t s_coolant_filter_wait_ticks;
static uint32_t s_dust_collector_wait_ticks;
static uint32_t s_spindle_override_lock_wait_ticks;
static uint32_t s_atc_tool_no_wait_ticks;
static uint32_t s_work_probe_ready_wait_ticks;
static uint32_t s_ups_voltage_low_wait_ticks;

static uint32_t v39_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V39_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID;
}

static int32_t v39_clamp_i32(int32_t value, int32_t min_value, int32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int32_t v39_apply_slowdown(int32_t feed_override_percent, int32_t reduction_percent)
{
    int32_t next_value = feed_override_percent - reduction_percent;
    return v39_clamp_i32(next_value, V39_MIN_FEED_OVERRIDE_PERCENT, V39_DEFAULT_FEED_OVERRIDE_PERCENT);
}

static void v39_record_hit(uint32_t index, int32_t value)
{
    const V39_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V39_FEATURE_COUNT) {
        return;
    }

    def = &k_v39_defs[index];
    g_ncFeatureBacklogV39Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV39Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV39Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV39Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV39Status.cat_mask |= V39_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V39_POLICY_MONITOR:
        g_ncFeatureBacklogV39Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_GATE:
        g_ncFeatureBacklogV39Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_HOLD:
        g_ncFeatureBacklogV39Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_INTERLOCK:
        g_ncFeatureBacklogV39Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV39Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_TRACE:
        g_ncFeatureBacklogV39Status.trace_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_INHIBIT:
        g_ncFeatureBacklogV39Status.inhibit_count++;
        break;
    case NC_FEATURE_BACKLOG_V39_POLICY_RETRY:
        g_ncFeatureBacklogV39Status.retry_count++;
        break;
    default:
        g_ncFeatureBacklogV39Status.warning_count++;
        break;
    }
}

static void v39_record_trace(uint32_t index)
{
    uint32_t trace;

    if (index >= NC_FEATURE_BACKLOG_V39_FEATURE_COUNT) {
        return;
    }

    trace = NC_FEATURE_BACKLOG_V39_TRACE_BASE + index;
    g_ncFeatureBacklogV39Status.last_trace_event_code = trace;
}

static void v39_record_fault(uint32_t index, uint32_t fault_bit, int32_t value)
{
    g_ncFeatureBacklogV39Status.last_fault_mask |= fault_bit;
    v39_record_trace(index);
    v39_record_hit(index, value);
}

static uint32_t v39_wait_or_clear(uint32_t *wait_ticks, uint32_t active, uint32_t timeout_ticks)
{
    if (active == 0u) {
        *wait_ticks = 0u;
        return 0u;
    }

    if (*wait_ticks < timeout_ticks) {
        (*wait_ticks)++;
        return 0u;
    }

    return 1u;
}

static void v39_reset_counters(void)
{
    s_spindle_temp_wait_ticks = 0u;
    s_linear_scale_dirty_wait_ticks = 0u;
    s_tool_clamp_wait_ticks = 0u;
    s_rotary_table_clamp_wait_ticks = 0u;
    s_coolant_filter_wait_ticks = 0u;
    s_dust_collector_wait_ticks = 0u;
    s_spindle_override_lock_wait_ticks = 0u;
    s_atc_tool_no_wait_ticks = 0u;
    s_work_probe_ready_wait_ticks = 0u;
    s_ups_voltage_low_wait_ticks = 0u;
}

void NcFeatureBacklogV39_Init(void)
{
    memset(&g_ncFeatureBacklogV39Status, 0, sizeof(g_ncFeatureBacklogV39Status));
    g_ncFeatureBacklogV39Status.implemented_count = NC_FEATURE_BACKLOG_V39_FEATURE_COUNT;
    g_ncFeatureBacklogV39Status.last_feed_override_percent = V39_DEFAULT_FEED_OVERRIDE_PERCENT;
    v39_reset_counters();
}

int NcFeatureBacklogV39_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v39_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V39_FEATURE_COUNT) {
        return 0;
    }

    return ((g_ncFeatureBacklogV39Status.enabled_mask & V39_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV39Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v39_index_from_feature_id(feature_id);
    uint32_t scan;

    if (index >= NC_FEATURE_BACKLOG_V39_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V39_ERR_RANGE;
    }

    if (enabled != 0u) {
        g_ncFeatureBacklogV39Status.enabled_mask |= V39_BIT(index);
    } else {
        g_ncFeatureBacklogV39Status.enabled_mask &= ~V39_BIT(index);
    }

    g_ncFeatureBacklogV39Status.configured_count = 0u;
    for (scan = 0u; scan < NC_FEATURE_BACKLOG_V39_FEATURE_COUNT; ++scan) {
        if ((g_ncFeatureBacklogV39Status.enabled_mask & V39_BIT(scan)) != 0u) {
            g_ncFeatureBacklogV39Status.configured_count++;
        }
    }

    return NC_FEATURE_BACKLOG_V39_OK;
}

void Api_EnableAllNcFeatureBacklogV39Features(void)
{
    uint32_t index;

    g_ncFeatureBacklogV39Status.enabled_mask = 0u;
    for (index = 0u; index < NC_FEATURE_BACKLOG_V39_FEATURE_COUNT; ++index) {
        g_ncFeatureBacklogV39Status.enabled_mask |= V39_BIT(index);
    }
    g_ncFeatureBacklogV39Status.configured_count = NC_FEATURE_BACKLOG_V39_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV39Status(NC_FEATURE_BACKLOG_V39_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V39_ERR_NULL;
    }

    *out_status = g_ncFeatureBacklogV39Status;
    return NC_FEATURE_BACKLOG_V39_OK;
}

const char *NcFeatureBacklogV39_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v39_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V39_FEATURE_COUNT) {
        return "unknown";
    }

    return k_v39_defs[index].name;
}

void NcFeatureBacklogV39_ApplyBlockTs(NC_FEATURE_BACKLOG_V39_BLOCK *block)
{
    uint32_t index;

    if (block == 0) {
        g_ncFeatureBacklogV39Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV39Status.ts_apply_count++;

    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V39_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V39_FEATURE_COUNT; ++index) {
        const V39_FEATURE_DEF *def;
        uint32_t feature_bit;
        uint32_t trace;

        if ((g_ncFeatureBacklogV39Status.enabled_mask & V39_BIT(index)) == 0u) {
            continue;
        }

        def = &k_v39_defs[index];
        feature_bit = V39_BIT(index);
        trace = NC_FEATURE_BACKLOG_V39_TRACE_BASE + index;

        switch (def->feature_id) {
        case NC_FEATURE_V39_SPINDLE_BEARING_TEMP_SLOWDOWN:
            block->diagnostics_mask |= feature_bit;
            block->feed_override_percent = v39_apply_slowdown(block->feed_override_percent, 6);
            v39_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V39_LINEAR_SCALE_DIRTY_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV39Status.last_trace_event_code = trace;
            v39_record_hit(index, (int32_t)feature_bit);
            break;
        case NC_FEATURE_V39_TOOL_CLAMP_DRAWBAR_CONFIRM_INTERLOCK:
            block->interlock_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v39_record_hit(index, block->axis_target[2]);
            break;
        case NC_FEATURE_V39_ROTARY_TABLE_CLAMP_CONFIRM_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v39_record_hit(index, block->axis_target[3]);
            break;
        case NC_FEATURE_V39_COOLANT_FILTER_DIFF_PRESSURE_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            block->feed_override_percent = v39_apply_slowdown(block->feed_override_percent, 4);
            v39_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V39_DUST_COLLECTOR_AIRFLOW_LOW_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->aux_request_bits |= feature_bit;
            v39_record_hit(index, (int32_t)block->aux_request_bits);
            break;
        case NC_FEATURE_V39_SPINDLE_OVERRIDE_LOCK_ZONE_INHIBIT:
            block->inhibit_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v39_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V39_ATC_TOOL_NUMBER_MISMATCH_HOLD:
            block->hold_request |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v39_record_hit(index, (int32_t)feature_bit);
            break;
        case NC_FEATURE_V39_WORK_PROBE_READY_TIMEOUT_RETRY:
            block->retry_request_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV39Status.last_trace_event_code = trace;
            v39_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V39_UPS_VOLTAGE_LOW_DECEL_STOP:
            block->hold_request |= feature_bit;
            block->feed_override_percent = v39_apply_slowdown(block->feed_override_percent, 10);
            block->trace_event_code = trace;
            g_ncFeatureBacklogV39Status.last_trace_event_code = trace;
            v39_record_hit(index, block->feed_override_percent);
            break;
        default:
            g_ncFeatureBacklogV39Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV39Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV39Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV39_RunRtTick(const NC_FEATURE_BACKLOG_V39_RT_IO *io)
{
    uint32_t fault_bit;

    if (io == 0) {
        g_ncFeatureBacklogV39Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV39Status.rt_tick_count++;
    g_ncFeatureBacklogV39Status.last_fault_mask = 0u;

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_SPINDLE_BEARING_TEMP_SLOWDOWN) != 0) {
        uint32_t active = ((io->spindle_bearing_temp_check != 0u) && (io->spindle_bearing_temp_c >= V39_SPINDLE_TEMP_WARN_C)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_spindle_temp_wait_ticks, active, V39_SPINDLE_TEMP_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(0u);
            g_ncFeatureBacklogV39Status.last_spindle_bearing_temp_c = io->spindle_bearing_temp_c;
            v39_record_fault(0u, fault_bit, io->spindle_bearing_temp_c);
            if (io->spindle_bearing_temp_c >= V39_SPINDLE_TEMP_HOLD_C) {
                g_ncFeatureBacklogV39Status.hold_count++;
            }
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_LINEAR_SCALE_DIRTY_TRACE) != 0) {
        uint32_t dirty = io->linear_scale_monitor_mask & io->linear_scale_dirty_mask;
        if (v39_wait_or_clear(&s_linear_scale_dirty_wait_ticks, dirty != 0u, V39_LINEAR_SCALE_DIRTY_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(1u);
            g_ncFeatureBacklogV39Status.last_axis_mask = dirty;
            v39_record_fault(1u, fault_bit, (int32_t)dirty);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_TOOL_CLAMP_DRAWBAR_CONFIRM_INTERLOCK) != 0) {
        uint32_t active = ((io->tool_clamp_check_request != 0u) && (io->tool_clamp_drawbar_ok == 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_tool_clamp_wait_ticks, active, V39_TOOL_CLAMP_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(2u);
            v39_record_fault(2u, fault_bit, (int32_t)io->tool_clamp_drawbar_ok);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_ROTARY_TABLE_CLAMP_CONFIRM_GATE) != 0) {
        uint32_t active = ((io->rotary_table_motion_request != 0u) && (io->rotary_table_clamp_confirmed == 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_rotary_table_clamp_wait_ticks, active, V39_ROTARY_TABLE_CLAMP_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(3u);
            v39_record_fault(3u, fault_bit, (int32_t)io->rotary_table_clamp_confirmed);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_COOLANT_FILTER_DIFF_PRESSURE_HOLD) != 0) {
        uint32_t active = ((io->coolant_filter_monitor != 0u) && (io->coolant_filter_diff_pressure_high != 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_coolant_filter_wait_ticks, active, V39_COOLANT_FILTER_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(4u);
            v39_record_fault(4u, fault_bit, (int32_t)io->coolant_filter_diff_pressure_high);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_DUST_COLLECTOR_AIRFLOW_LOW_GATE) != 0) {
        uint32_t active = ((io->dust_collector_required != 0u) && (io->dust_collector_airflow_ok == 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_dust_collector_wait_ticks, active, V39_DUST_COLLECTOR_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(5u);
            v39_record_fault(5u, fault_bit, (int32_t)io->dust_collector_airflow_ok);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_SPINDLE_OVERRIDE_LOCK_ZONE_INHIBIT) != 0) {
        uint32_t active = ((io->spindle_override_lock_zone != 0u) && (io->spindle_override_change_request != 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_spindle_override_lock_wait_ticks, active, V39_SPINDLE_OVERRIDE_LOCK_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(6u);
            v39_record_fault(6u, fault_bit, (int32_t)io->spindle_override_change_request);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_ATC_TOOL_NUMBER_MISMATCH_HOLD) != 0) {
        uint32_t mismatch = ((io->atc_tool_number_check_request != 0u) && (io->atc_expected_tool_no != io->atc_actual_tool_no)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_atc_tool_no_wait_ticks, mismatch, V39_ATC_TOOL_NO_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(7u);
            g_ncFeatureBacklogV39Status.last_tool_expected = io->atc_expected_tool_no;
            g_ncFeatureBacklogV39Status.last_tool_actual = io->atc_actual_tool_no;
            v39_record_fault(7u, fault_bit, (int32_t)io->atc_actual_tool_no);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_WORK_PROBE_READY_TIMEOUT_RETRY) != 0) {
        uint32_t active = ((io->work_probe_measure_request != 0u) && (io->work_probe_ready == 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_work_probe_ready_wait_ticks, active, V39_WORK_PROBE_READY_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(8u);
            v39_record_fault(8u, fault_bit, (int32_t)io->work_probe_ready);
        }
    }

    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_UPS_VOLTAGE_LOW_DECEL_STOP) != 0) {
        uint32_t active = ((io->ups_monitor_enabled != 0u) && (io->ups_voltage_low != 0u)) ? 1u : 0u;
        if (v39_wait_or_clear(&s_ups_voltage_low_wait_ticks, active, V39_UPS_VOLTAGE_LOW_TIMEOUT_TICKS) != 0u) {
            fault_bit = V39_BIT(9u);
            v39_record_fault(9u, fault_bit, (int32_t)io->ups_voltage_low);
            g_ncFeatureBacklogV39Status.hold_count++;
        }
    }
}

int Api_RunNcFeatureBacklogV39SelfCheck(void)
{
    uint32_t index;

    if (NC_FEATURE_BACKLOG_V39_FEATURE_COUNT != 10u) {
        return NC_FEATURE_BACKLOG_V39_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_FEATURE_COUNT_V39 != 315u) {
        return NC_FEATURE_BACKLOG_V39_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_WORD_COUNT_V39 != 10u) {
        return NC_FEATURE_BACKLOG_V39_ERR_RANGE;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V39_FEATURE_COUNT; ++index) {
        if (k_v39_defs[index].feature_id != (NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID + index)) {
            return NC_FEATURE_BACKLOG_V39_ERR_RANGE;
        }
        if (k_v39_defs[index].name == 0) {
            return NC_FEATURE_BACKLOG_V39_ERR_NULL;
        }
    }

    return NC_FEATURE_BACKLOG_V39_OK;
}

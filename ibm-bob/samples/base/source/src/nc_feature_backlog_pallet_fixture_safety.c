/* ibm-bob/samples/base/source/src/nc_feature_backlog_pallet_fixture_safety.c */
/* Implements pallet, fixture, and safety-mat backlog checks. */
/* This exists so v38 fixture safety policies stay isolated and testable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_feature_backlog_pallet_fixture_safety.h, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_clang.bat */
#include "nc_feature_backlog_pallet_fixture_safety.h"

#include <string.h>

#define V38_BIT(index_) (1u << (uint32_t)(index_))
#define V38_DEFAULT_FEED_OVERRIDE_PERCENT       (100)
#define V38_MIN_FEED_OVERRIDE_PERCENT           (5)
#define V38_THERMAL_DRIFT_WARN_UM               (80)
#define V38_THERMAL_DRIFT_HOLD_UM               (150)
#define V38_THERMAL_STABLE_TIMEOUT_TICKS        (4u)
#define V38_HYD_FILTER_TIMEOUT_TICKS            (2u)
#define V38_ATC_POT_INDEX_TIMEOUT_TICKS         (4u)
#define V38_PALLET_ID_TIMEOUT_TICKS             (2u)
#define V38_Z_COUNTERBALANCE_TIMEOUT_TICKS      (3u)
#define V38_SERVO_ABS_MISMATCH_TIMEOUT_TICKS    (2u)
#define V38_TOOL_UNCLAMP_TIMEOUT_TICKS          (3u)
#define V38_COOLANT_LEVEL_TIMEOUT_TICKS         (3u)
#define V38_MIST_COLLECTOR_TIMEOUT_TICKS        (3u)
#define V38_SAFETY_MAT_TIMEOUT_TICKS            (1u)

typedef struct V38_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V38_POLICY policy;
    uint32_t category;
    const char *name;
} V38_FEATURE_DEF;

static const V38_FEATURE_DEF k_v38_defs[NC_FEATURE_BACKLOG_V38_FEATURE_COUNT] = {
    { NC_FEATURE_V38_MACHINE_THERMAL_STABLE_READY_INHIBIT,   NC_FEATURE_BACKLOG_V38_POLICY_INHIBIT,  9u, "machine thermal stable ready inhibit" },
    { NC_FEATURE_V38_HYDRAULIC_FILTER_CLOG_TRACE,            NC_FEATURE_BACKLOG_V38_POLICY_TRACE,    9u, "hydraulic filter clog trace" },
    { NC_FEATURE_V38_ATC_POT_INDEX_CONFIRM_INTERLOCK,        NC_FEATURE_BACKLOG_V38_POLICY_INTERLOCK,9u, "ATC pot index confirmation interlock" },
    { NC_FEATURE_V38_PALLET_ID_MISMATCH_GATE,                NC_FEATURE_BACKLOG_V38_POLICY_GATE,     9u, "pallet ID mismatch gate" },
    { NC_FEATURE_V38_Z_COUNTERBALANCE_PRESSURE_LOW_HOLD,     NC_FEATURE_BACKLOG_V38_POLICY_HOLD,     9u, "Z counterbalance pressure low hold" },
    { NC_FEATURE_V38_SERVO_ABSOLUTE_POSITION_MISMATCH_TRACE, NC_FEATURE_BACKLOG_V38_POLICY_TRACE,    9u, "servo absolute position mismatch trace" },
    { NC_FEATURE_V38_SPINDLE_TOOL_UNCLAMP_CONFIRM_RETRY,     NC_FEATURE_BACKLOG_V38_POLICY_RETRY,    9u, "spindle tool unclamp confirmation retry" },
    { NC_FEATURE_V38_COOLANT_TANK_LEVEL_LOW_GATE,            NC_FEATURE_BACKLOG_V38_POLICY_GATE,     9u, "coolant tank level low gate" },
    { NC_FEATURE_V38_MIST_COLLECTOR_READY_GATE,              NC_FEATURE_BACKLOG_V38_POLICY_GATE,     9u, "mist collector ready gate" },
    { NC_FEATURE_V38_SAFETY_MAT_INTRUSION_HOLD,              NC_FEATURE_BACKLOG_V38_POLICY_HOLD,     9u, "safety mat intrusion hold" }
};

NC_FEATURE_BACKLOG_V38_STATUS g_ncFeatureBacklogV38Status;

static uint32_t s_thermal_stable_wait_ticks;
static uint32_t s_hyd_filter_wait_ticks;
static uint32_t s_atc_pot_wait_ticks;
static uint32_t s_pallet_id_wait_ticks;
static uint32_t s_z_counterbalance_wait_ticks;
static uint32_t s_servo_abs_wait_ticks;
static uint32_t s_tool_unclamp_wait_ticks;
static uint32_t s_coolant_level_wait_ticks;
static uint32_t s_mist_collector_wait_ticks;
static uint32_t s_safety_mat_wait_ticks;

static uint32_t v38_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V38_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID;
}

static int32_t v38_abs_i32(int32_t value)
{
    if (value < 0) {
        return -value;
    }
    return value;
}

static int32_t v38_clamp_i32(int32_t value, int32_t min_value, int32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int32_t v38_apply_slowdown(int32_t feed_override_percent, int32_t reduction_percent)
{
    int32_t next_value;

    next_value = feed_override_percent - reduction_percent;
    return v38_clamp_i32(next_value, V38_MIN_FEED_OVERRIDE_PERCENT, V38_DEFAULT_FEED_OVERRIDE_PERCENT);
}

static void v38_record_hit(uint32_t index, int32_t value)
{
    const V38_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V38_FEATURE_COUNT) {
        return;
    }

    def = &k_v38_defs[index];
    g_ncFeatureBacklogV38Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV38Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV38Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV38Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV38Status.cat_mask |= V38_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V38_POLICY_MONITOR:
        g_ncFeatureBacklogV38Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_GATE:
        g_ncFeatureBacklogV38Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_HOLD:
        g_ncFeatureBacklogV38Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_INTERLOCK:
        g_ncFeatureBacklogV38Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV38Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_TRACE:
        g_ncFeatureBacklogV38Status.trace_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_INHIBIT:
        g_ncFeatureBacklogV38Status.inhibit_count++;
        break;
    case NC_FEATURE_BACKLOG_V38_POLICY_RETRY:
        g_ncFeatureBacklogV38Status.retry_count++;
        break;
    default:
        g_ncFeatureBacklogV38Status.warning_count++;
        break;
    }
}

static void v38_reset_counters(void)
{
    s_thermal_stable_wait_ticks = 0u;
    s_hyd_filter_wait_ticks = 0u;
    s_atc_pot_wait_ticks = 0u;
    s_pallet_id_wait_ticks = 0u;
    s_z_counterbalance_wait_ticks = 0u;
    s_servo_abs_wait_ticks = 0u;
    s_tool_unclamp_wait_ticks = 0u;
    s_coolant_level_wait_ticks = 0u;
    s_mist_collector_wait_ticks = 0u;
    s_safety_mat_wait_ticks = 0u;
}

void NcFeatureBacklogV38_Init(void)
{
    memset(&g_ncFeatureBacklogV38Status, 0, sizeof(g_ncFeatureBacklogV38Status));
    g_ncFeatureBacklogV38Status.implemented_count = NC_FEATURE_BACKLOG_V38_FEATURE_COUNT;
    g_ncFeatureBacklogV38Status.last_feed_override_percent = V38_DEFAULT_FEED_OVERRIDE_PERCENT;
    v38_reset_counters();
}

int NcFeatureBacklogV38_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v38_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V38_FEATURE_COUNT) {
        return 0;
    }

    return ((g_ncFeatureBacklogV38Status.enabled_mask & V38_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV38Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v38_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V38_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V38_ERR_RANGE;
    }

    if (enabled != 0u) {
        g_ncFeatureBacklogV38Status.enabled_mask |= V38_BIT(index);
    } else {
        g_ncFeatureBacklogV38Status.enabled_mask &= ~V38_BIT(index);
    }

    g_ncFeatureBacklogV38Status.configured_count = 0u;
    for (index = 0u; index < NC_FEATURE_BACKLOG_V38_FEATURE_COUNT; ++index) {
        if ((g_ncFeatureBacklogV38Status.enabled_mask & V38_BIT(index)) != 0u) {
            g_ncFeatureBacklogV38Status.configured_count++;
        }
    }

    return NC_FEATURE_BACKLOG_V38_OK;
}

void Api_EnableAllNcFeatureBacklogV38Features(void)
{
    uint32_t index;

    g_ncFeatureBacklogV38Status.enabled_mask = 0u;
    for (index = 0u; index < NC_FEATURE_BACKLOG_V38_FEATURE_COUNT; ++index) {
        g_ncFeatureBacklogV38Status.enabled_mask |= V38_BIT(index);
    }
    g_ncFeatureBacklogV38Status.configured_count = NC_FEATURE_BACKLOG_V38_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV38Status(NC_FEATURE_BACKLOG_V38_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V38_ERR_NULL;
    }

    *out_status = g_ncFeatureBacklogV38Status;
    return NC_FEATURE_BACKLOG_V38_OK;
}

const char *NcFeatureBacklogV38_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v38_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V38_FEATURE_COUNT) {
        return "unknown";
    }

    return k_v38_defs[index].name;
}

void NcFeatureBacklogV38_ApplyBlockTs(NC_FEATURE_BACKLOG_V38_BLOCK *block)
{
    uint32_t index;

    if (block == 0) {
        g_ncFeatureBacklogV38Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV38Status.ts_apply_count++;

    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V38_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V38_FEATURE_COUNT; ++index) {
        const V38_FEATURE_DEF *def;
        uint32_t feature_bit;
        uint32_t trace;

        if ((g_ncFeatureBacklogV38Status.enabled_mask & V38_BIT(index)) == 0u) {
            continue;
        }

        def = &k_v38_defs[index];
        feature_bit = V38_BIT(index);
        trace = NC_FEATURE_BACKLOG_V38_TRACE_BASE + index;

        switch (def->feature_id) {
        case NC_FEATURE_V38_MACHINE_THERMAL_STABLE_READY_INHIBIT:
            block->inhibit_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            block->feed_override_percent = v38_apply_slowdown(block->feed_override_percent, 4);
            v38_record_hit(index, block->axis_target[0]);
            break;
        case NC_FEATURE_V38_HYDRAULIC_FILTER_CLOG_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV38Status.last_trace_event_code = trace;
            v38_record_hit(index, block->axis_target[1]);
            break;
        case NC_FEATURE_V38_ATC_POT_INDEX_CONFIRM_INTERLOCK:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v38_record_hit(index, block->axis_target[2]);
            break;
        case NC_FEATURE_V38_PALLET_ID_MISMATCH_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v38_record_hit(index, block->axis_target[3]);
            break;
        case NC_FEATURE_V38_Z_COUNTERBALANCE_PRESSURE_LOW_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            block->feed_override_percent = v38_apply_slowdown(block->feed_override_percent, 6);
            v38_record_hit(index, block->axis_target[4]);
            break;
        case NC_FEATURE_V38_SERVO_ABSOLUTE_POSITION_MISMATCH_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV38Status.last_trace_event_code = trace;
            v38_record_hit(index, block->axis_target[5]);
            break;
        case NC_FEATURE_V38_SPINDLE_TOOL_UNCLAMP_CONFIRM_RETRY:
            block->sequence_gate_mask |= feature_bit;
            block->retry_request_mask |= feature_bit;
            v38_record_hit(index, block->axis_target[6]);
            break;
        case NC_FEATURE_V38_COOLANT_TANK_LEVEL_LOW_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->hold_request |= feature_bit;
            v38_record_hit(index, block->axis_target[7]);
            break;
        case NC_FEATURE_V38_MIST_COLLECTOR_READY_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v38_record_hit(index, (int32_t)block->sequence_gate_mask);
            break;
        case NC_FEATURE_V38_SAFETY_MAT_INTRUSION_HOLD:
            block->hold_request |= feature_bit;
            block->interlock_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV38Status.last_trace_event_code = trace;
            v38_record_hit(index, (int32_t)trace);
            break;
        default:
            g_ncFeatureBacklogV38Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV38Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV38Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV38_RunRtTick(const NC_FEATURE_BACKLOG_V38_RT_IO *io)
{
    if (io == 0) {
        g_ncFeatureBacklogV38Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV38Status.rt_tick_count++;

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_MACHINE_THERMAL_STABLE_READY_INHIBIT) != 0) {
        int32_t drift_abs = v38_abs_i32(io->thermal_drift_um);
        if (io->thermal_stable_request != 0u &&
            (io->thermal_temp_ready == 0u || drift_abs >= V38_THERMAL_DRIFT_WARN_UM)) {
            s_thermal_stable_wait_ticks++;
            if (s_thermal_stable_wait_ticks >= V38_THERMAL_STABLE_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(0u);
                g_ncFeatureBacklogV38Status.last_thermal_drift_um = io->thermal_drift_um;
                v38_record_hit(0u, io->thermal_drift_um);
                if (drift_abs >= V38_THERMAL_DRIFT_HOLD_UM) {
                    g_ncFeatureBacklogV38Status.hold_count++;
                }
            }
        } else {
            s_thermal_stable_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_HYDRAULIC_FILTER_CLOG_TRACE) != 0) {
        if (io->hydraulic_filter_monitored != 0u && io->hydraulic_filter_clogged != 0u) {
            s_hyd_filter_wait_ticks++;
            if (s_hyd_filter_wait_ticks >= V38_HYD_FILTER_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(1u);
                g_ncFeatureBacklogV38Status.last_trace_event_code = NC_FEATURE_BACKLOG_V38_TRACE_BASE + 1u;
                v38_record_hit(1u, (int32_t)io->hydraulic_filter_clogged);
            }
        } else {
            s_hyd_filter_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_ATC_POT_INDEX_CONFIRM_INTERLOCK) != 0) {
        if (io->atc_pot_index_request != 0u && io->atc_pot_position_ok == 0u) {
            s_atc_pot_wait_ticks++;
            if (s_atc_pot_wait_ticks >= V38_ATC_POT_INDEX_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(2u);
                v38_record_hit(2u, (int32_t)io->cycle_counter);
            }
        } else {
            s_atc_pot_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_PALLET_ID_MISMATCH_GATE) != 0) {
        if (io->pallet_id_check_request != 0u && io->pallet_present != 0u &&
            io->expected_pallet_id != io->actual_pallet_id) {
            s_pallet_id_wait_ticks++;
            if (s_pallet_id_wait_ticks >= V38_PALLET_ID_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(3u);
                g_ncFeatureBacklogV38Status.last_pallet_expected = io->expected_pallet_id;
                g_ncFeatureBacklogV38Status.last_pallet_actual = io->actual_pallet_id;
                v38_record_hit(3u, (int32_t)io->actual_pallet_id);
            }
        } else {
            s_pallet_id_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_Z_COUNTERBALANCE_PRESSURE_LOW_HOLD) != 0) {
        if (io->z_axis_enabled != 0u && io->z_counterbalance_pressure_ok == 0u) {
            s_z_counterbalance_wait_ticks++;
            if (s_z_counterbalance_wait_ticks >= V38_Z_COUNTERBALANCE_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(4u);
                g_ncFeatureBacklogV38Status.last_axis_mask |= V38_BIT(2u);
                v38_record_hit(4u, (int32_t)s_z_counterbalance_wait_ticks);
            }
        } else {
            s_z_counterbalance_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_SERVO_ABSOLUTE_POSITION_MISMATCH_TRACE) != 0) {
        uint32_t mismatch = io->servo_abs_check_mask & io->servo_abs_mismatch_mask;
        if (mismatch != 0u) {
            s_servo_abs_wait_ticks++;
            if (s_servo_abs_wait_ticks >= V38_SERVO_ABS_MISMATCH_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(5u);
                g_ncFeatureBacklogV38Status.last_axis_mask = mismatch;
                g_ncFeatureBacklogV38Status.last_trace_event_code = NC_FEATURE_BACKLOG_V38_TRACE_BASE + 5u;
                v38_record_hit(5u, (int32_t)mismatch);
            }
        } else {
            s_servo_abs_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_SPINDLE_TOOL_UNCLAMP_CONFIRM_RETRY) != 0) {
        if (io->spindle_tool_unclamp_request != 0u && io->spindle_tool_unclamp_sensor == 0u) {
            s_tool_unclamp_wait_ticks++;
            if (s_tool_unclamp_wait_ticks >= V38_TOOL_UNCLAMP_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(6u);
                v38_record_hit(6u, (int32_t)s_tool_unclamp_wait_ticks);
            }
        } else {
            s_tool_unclamp_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_COOLANT_TANK_LEVEL_LOW_GATE) != 0) {
        if (io->coolant_request != 0u && io->coolant_tank_level_low != 0u) {
            s_coolant_level_wait_ticks++;
            if (s_coolant_level_wait_ticks >= V38_COOLANT_LEVEL_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(7u);
                v38_record_hit(7u, (int32_t)s_coolant_level_wait_ticks);
            }
        } else {
            s_coolant_level_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_MIST_COLLECTOR_READY_GATE) != 0) {
        if (io->mist_collector_required != 0u && io->mist_collector_ready == 0u) {
            s_mist_collector_wait_ticks++;
            if (s_mist_collector_wait_ticks >= V38_MIST_COLLECTOR_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(8u);
                v38_record_hit(8u, (int32_t)s_mist_collector_wait_ticks);
            }
        } else {
            s_mist_collector_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_SAFETY_MAT_INTRUSION_HOLD) != 0) {
        if (io->safety_mat_enabled != 0u && io->safety_mat_intrusion != 0u) {
            s_safety_mat_wait_ticks++;
            if (s_safety_mat_wait_ticks >= V38_SAFETY_MAT_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV38Status.last_fault_mask |= V38_BIT(9u);
                g_ncFeatureBacklogV38Status.last_trace_event_code = NC_FEATURE_BACKLOG_V38_TRACE_BASE + 9u;
                v38_record_hit(9u, (int32_t)io->safety_mat_intrusion);
            }
        } else {
            s_safety_mat_wait_ticks = 0u;
        }
    }
}

int Api_RunNcFeatureBacklogV38SelfCheck(void)
{
    uint32_t index;

    if (g_ncFeatureBacklogV38Status.implemented_count != NC_FEATURE_BACKLOG_V38_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V38_ERR_RANGE;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V38_FEATURE_COUNT; ++index) {
        if (k_v38_defs[index].feature_id != (NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID + index)) {
            return NC_FEATURE_BACKLOG_V38_ERR_RANGE;
        }
        if (k_v38_defs[index].name == 0) {
            return NC_FEATURE_BACKLOG_V38_ERR_NULL;
        }
    }

    return NC_FEATURE_BACKLOG_V38_OK;
}

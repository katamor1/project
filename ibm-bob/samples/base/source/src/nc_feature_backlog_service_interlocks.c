/* ibm-bob/samples/base/source/src/nc_feature_backlog_service_interlocks.c */
/* Implements service interlock backlog checks. */
/* This exists so v41 service interlock policies stay isolated and testable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_feature_backlog_service_interlocks.h, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_clang.bat */
#include "nc_feature_backlog_service_interlocks.h"

#include <string.h>

#define V41_BIT(index_) (1u << (uint32_t)(index_))
#define V41_DEFAULT_FEED_OVERRIDE_PERCENT      (100)
#define V41_MIN_FEED_OVERRIDE_PERCENT          (5)
#define V41_SERVO_ERROR_WARN_UM                (80)
#define V41_SERVO_ERROR_LIMIT_UM               (150)
#define V41_SPINDLE_LOAD_HOLD_PERCENT          (135)
#define V41_FIXTURE_PRESSURE_MIN_KPA           (420)
#define V41_TOOL_WASH_FLOW_MIN_PERCENT         (35)

#define V41_SERVO_ERROR_TIMEOUT_TICKS          (2u)
#define V41_SPINDLE_LOAD_TIMEOUT_TICKS         (1u)
#define V41_COOLANT_FLOW_TIMEOUT_TICKS         (2u)
#define V41_FIXTURE_PRESSURE_TIMEOUT_TICKS     (2u)
#define V41_ROBOT_DOOR_TIMEOUT_TICKS           (3u)
#define V41_EXHAUST_FAN_TIMEOUT_TICKS          (3u)
#define V41_TOOL_WASH_TIMEOUT_TICKS            (2u)
#define V41_AXIS_HOME_TIMEOUT_TICKS            (2u)
#define V41_LICENSE_TIMEOUT_TICKS              (1u)
#define V41_MAINT_KEY_TIMEOUT_TICKS            (1u)

typedef struct V41_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V41_POLICY policy;
    uint32_t category;
    const char *name;
} V41_FEATURE_DEF;

static const V41_FEATURE_DEF k_v41_defs[NC_FEATURE_BACKLOG_V41_FEATURE_COUNT] = {
    { NC_FEATURE_V41_SERVO_POSITION_ERROR_SLOWDOWN, NC_FEATURE_BACKLOG_V41_POLICY_SLOWDOWN, 11u, "servo position error staged slowdown" },
    { NC_FEATURE_V41_SPINDLE_LOAD_SPIKE_HOLD, NC_FEATURE_BACKLOG_V41_POLICY_HOLD, 11u, "spindle load spike hold" },
    { NC_FEATURE_V41_COOLANT_FLOW_SENSOR_OPEN_TRACE, NC_FEATURE_BACKLOG_V41_POLICY_TRACE, 11u, "coolant flow sensor open trace" },
    { NC_FEATURE_V41_FIXTURE_CLAMP_PRESSURE_INTERLOCK, NC_FEATURE_BACKLOG_V41_POLICY_INTERLOCK, 11u, "fixture clamp pressure low interlock" },
    { NC_FEATURE_V41_ROBOT_DOOR_CLOSED_READY_GATE, NC_FEATURE_BACKLOG_V41_POLICY_GATE, 11u, "robot door closed ready gate" },
    { NC_FEATURE_V41_CUTTING_ROOM_EXHAUST_FAN_READY_GATE, NC_FEATURE_BACKLOG_V41_POLICY_GATE, 11u, "cutting room exhaust fan ready gate" },
    { NC_FEATURE_V41_TOOL_WASH_NOZZLE_CLOG_RETRY, NC_FEATURE_BACKLOG_V41_POLICY_RETRY, 11u, "tool wash nozzle clog retry" },
    { NC_FEATURE_V41_AXIS_HOME_SENSOR_DUAL_MISMATCH_TRACE, NC_FEATURE_BACKLOG_V41_POLICY_TRACE, 11u, "axis home sensor dual mismatch trace" },
    { NC_FEATURE_V41_NC_OPTION_LICENSE_MISMATCH_INHIBIT, NC_FEATURE_BACKLOG_V41_POLICY_INHIBIT, 11u, "NC option license mismatch inhibit" },
    { NC_FEATURE_V41_MAINTENANCE_MODE_KEY_REMOVED_HOLD, NC_FEATURE_BACKLOG_V41_POLICY_HOLD, 11u, "maintenance mode key removed hold" }
};

NC_FEATURE_BACKLOG_V41_STATUS g_ncFeatureBacklogV41Status;

static uint32_t s_servo_error_wait_ticks;
static uint32_t s_spindle_load_wait_ticks;
static uint32_t s_coolant_flow_wait_ticks;
static uint32_t s_fixture_pressure_wait_ticks;
static uint32_t s_robot_door_wait_ticks;
static uint32_t s_exhaust_fan_wait_ticks;
static uint32_t s_tool_wash_wait_ticks;
static uint32_t s_axis_home_wait_ticks;
static uint32_t s_license_wait_ticks;
static uint32_t s_maint_key_wait_ticks;

static uint32_t v41_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V41_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID;
}

static int32_t v41_clamp_i32(int32_t value, int32_t min_value, int32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int32_t v41_apply_slowdown(int32_t feed_override_percent, int32_t reduction_percent)
{
    int32_t next_value = feed_override_percent - reduction_percent;
    return v41_clamp_i32(next_value, V41_MIN_FEED_OVERRIDE_PERCENT, V41_DEFAULT_FEED_OVERRIDE_PERCENT);
}

static void v41_record_hit(uint32_t index, int32_t value)
{
    const V41_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V41_FEATURE_COUNT) {
        return;
    }

    def = &k_v41_defs[index];
    g_ncFeatureBacklogV41Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV41Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV41Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV41Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV41Status.cat_mask |= V41_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V41_POLICY_MONITOR:
        g_ncFeatureBacklogV41Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_GATE:
        g_ncFeatureBacklogV41Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_HOLD:
        g_ncFeatureBacklogV41Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_INTERLOCK:
        g_ncFeatureBacklogV41Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV41Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_TRACE:
        g_ncFeatureBacklogV41Status.trace_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_INHIBIT:
        g_ncFeatureBacklogV41Status.inhibit_count++;
        break;
    case NC_FEATURE_BACKLOG_V41_POLICY_RETRY:
        g_ncFeatureBacklogV41Status.retry_count++;
        break;
    default:
        g_ncFeatureBacklogV41Status.warning_count++;
        break;
    }
}

static void v41_record_trace(uint32_t index)
{
    if (index < NC_FEATURE_BACKLOG_V41_FEATURE_COUNT) {
        g_ncFeatureBacklogV41Status.last_trace_event_code = NC_FEATURE_BACKLOG_V41_TRACE_BASE + index;
    }
}

static void v41_record_fault(uint32_t index, uint32_t fault_bit, int32_t value)
{
    g_ncFeatureBacklogV41Status.last_fault_mask |= fault_bit;
    v41_record_trace(index);
    v41_record_hit(index, value);
}

static uint32_t v41_wait_or_clear(uint32_t *wait_ticks, uint32_t active, uint32_t timeout_ticks)
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

static void v41_reset_counters(void)
{
    s_servo_error_wait_ticks = 0u;
    s_spindle_load_wait_ticks = 0u;
    s_coolant_flow_wait_ticks = 0u;
    s_fixture_pressure_wait_ticks = 0u;
    s_robot_door_wait_ticks = 0u;
    s_exhaust_fan_wait_ticks = 0u;
    s_tool_wash_wait_ticks = 0u;
    s_axis_home_wait_ticks = 0u;
    s_license_wait_ticks = 0u;
    s_maint_key_wait_ticks = 0u;
}

void NcFeatureBacklogV41_Init(void)
{
    memset(&g_ncFeatureBacklogV41Status, 0, sizeof(g_ncFeatureBacklogV41Status));
    g_ncFeatureBacklogV41Status.implemented_count = NC_FEATURE_BACKLOG_V41_FEATURE_COUNT;
    g_ncFeatureBacklogV41Status.last_feed_override_percent = V41_DEFAULT_FEED_OVERRIDE_PERCENT;
    v41_reset_counters();
}

int NcFeatureBacklogV41_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v41_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V41_FEATURE_COUNT) {
        return 0;
    }
    return ((g_ncFeatureBacklogV41Status.enabled_mask & V41_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV41Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v41_index_from_feature_id(feature_id);
    uint32_t scan;
    if (index >= NC_FEATURE_BACKLOG_V41_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V41_ERR_RANGE;
    }
    if (enabled != 0u) {
        g_ncFeatureBacklogV41Status.enabled_mask |= V41_BIT(index);
    } else {
        g_ncFeatureBacklogV41Status.enabled_mask &= ~V41_BIT(index);
    }
    g_ncFeatureBacklogV41Status.configured_count = 0u;
    for (scan = 0u; scan < NC_FEATURE_BACKLOG_V41_FEATURE_COUNT; ++scan) {
        if ((g_ncFeatureBacklogV41Status.enabled_mask & V41_BIT(scan)) != 0u) {
            g_ncFeatureBacklogV41Status.configured_count++;
        }
    }
    return NC_FEATURE_BACKLOG_V41_OK;
}

void Api_EnableAllNcFeatureBacklogV41Features(void)
{
    uint32_t index;
    g_ncFeatureBacklogV41Status.enabled_mask = 0u;
    for (index = 0u; index < NC_FEATURE_BACKLOG_V41_FEATURE_COUNT; ++index) {
        g_ncFeatureBacklogV41Status.enabled_mask |= V41_BIT(index);
    }
    g_ncFeatureBacklogV41Status.configured_count = NC_FEATURE_BACKLOG_V41_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV41Status(NC_FEATURE_BACKLOG_V41_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V41_ERR_NULL;
    }
    *out_status = g_ncFeatureBacklogV41Status;
    return NC_FEATURE_BACKLOG_V41_OK;
}

const char *NcFeatureBacklogV41_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v41_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V41_FEATURE_COUNT) {
        return "unknown";
    }
    return k_v41_defs[index].name;
}

void NcFeatureBacklogV41_ApplyBlockTs(NC_FEATURE_BACKLOG_V41_BLOCK *block)
{
    uint32_t index;
    if (block == 0) {
        g_ncFeatureBacklogV41Status.warning_count++;
        return;
    }
    g_ncFeatureBacklogV41Status.ts_apply_count++;
    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V41_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V41_FEATURE_COUNT; ++index) {
        const V41_FEATURE_DEF *def;
        uint32_t feature_bit;
        uint32_t trace;
        if ((g_ncFeatureBacklogV41Status.enabled_mask & V41_BIT(index)) == 0u) {
            continue;
        }
        def = &k_v41_defs[index];
        feature_bit = V41_BIT(index);
        trace = NC_FEATURE_BACKLOG_V41_TRACE_BASE + index;

        switch (def->feature_id) {
        case NC_FEATURE_V41_SERVO_POSITION_ERROR_SLOWDOWN:
            block->feed_override_percent = v41_apply_slowdown(block->feed_override_percent, 9);
            block->diagnostics_mask |= feature_bit;
            v41_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V41_SPINDLE_LOAD_SPIKE_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v41_record_hit(index, (int32_t)block->hold_request);
            break;
        case NC_FEATURE_V41_COOLANT_FLOW_SENSOR_OPEN_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV41Status.last_trace_event_code = trace;
            v41_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V41_FIXTURE_CLAMP_PRESSURE_INTERLOCK:
            block->interlock_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v41_record_hit(index, (int32_t)block->interlock_mask);
            break;
        case NC_FEATURE_V41_ROBOT_DOOR_CLOSED_READY_GATE:
        case NC_FEATURE_V41_CUTTING_ROOM_EXHAUST_FAN_READY_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->aux_request_bits |= feature_bit;
            v41_record_hit(index, (int32_t)block->sequence_gate_mask);
            break;
        case NC_FEATURE_V41_TOOL_WASH_NOZZLE_CLOG_RETRY:
            block->retry_request_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v41_record_hit(index, (int32_t)block->retry_request_mask);
            break;
        case NC_FEATURE_V41_AXIS_HOME_SENSOR_DUAL_MISMATCH_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV41Status.last_trace_event_code = trace;
            v41_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V41_NC_OPTION_LICENSE_MISMATCH_INHIBIT:
            block->inhibit_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v41_record_hit(index, (int32_t)block->inhibit_mask);
            break;
        case NC_FEATURE_V41_MAINTENANCE_MODE_KEY_REMOVED_HOLD:
            block->hold_request |= feature_bit;
            block->interlock_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV41Status.last_trace_event_code = trace;
            v41_record_hit(index, (int32_t)block->hold_request);
            break;
        default:
            g_ncFeatureBacklogV41Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV41Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV41Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV41_RunRtTick(const NC_FEATURE_BACKLOG_V41_RT_IO *io)
{
    uint32_t fault_bit;
    if (io == 0) {
        g_ncFeatureBacklogV41Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV41Status.rt_tick_count++;
    g_ncFeatureBacklogV41Status.last_fault_mask = 0u;

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_SERVO_POSITION_ERROR_SLOWDOWN) != 0) {
        uint32_t active = ((io->servo_position_error_monitor != 0u) && (io->servo_position_error_um >= V41_SERVO_ERROR_WARN_UM)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_servo_error_wait_ticks, active, V41_SERVO_ERROR_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(0u);
            g_ncFeatureBacklogV41Status.last_axis_mask = io->servo_position_error_axis_mask;
            g_ncFeatureBacklogV41Status.last_servo_error_um = io->servo_position_error_um;
            v41_record_fault(0u, fault_bit, io->servo_position_error_um);
            if (io->servo_position_error_um >= V41_SERVO_ERROR_LIMIT_UM) {
                g_ncFeatureBacklogV41Status.slowdown_count++;
            }
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_SPINDLE_LOAD_SPIKE_HOLD) != 0) {
        uint32_t active = ((io->spindle_load_monitor != 0u) && (io->spindle_load_percent >= V41_SPINDLE_LOAD_HOLD_PERCENT)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_spindle_load_wait_ticks, active, V41_SPINDLE_LOAD_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(1u);
            g_ncFeatureBacklogV41Status.last_spindle_load_percent = io->spindle_load_percent;
            v41_record_fault(1u, fault_bit, io->spindle_load_percent);
            g_ncFeatureBacklogV41Status.hold_count++;
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_COOLANT_FLOW_SENSOR_OPEN_TRACE) != 0) {
        uint32_t active = ((io->coolant_flow_sensor_monitor != 0u) && (io->coolant_flow_sensor_open != 0u)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_coolant_flow_wait_ticks, active, V41_COOLANT_FLOW_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(2u);
            v41_record_fault(2u, fault_bit, (int32_t)io->coolant_flow_sensor_open);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_FIXTURE_CLAMP_PRESSURE_INTERLOCK) != 0) {
        uint32_t active = ((io->fixture_clamp_pressure_required != 0u) && (io->fixture_clamp_pressure_kpa < V41_FIXTURE_PRESSURE_MIN_KPA)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_fixture_pressure_wait_ticks, active, V41_FIXTURE_PRESSURE_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(3u);
            g_ncFeatureBacklogV41Status.last_fixture_pressure_kpa = io->fixture_clamp_pressure_kpa;
            v41_record_fault(3u, fault_bit, io->fixture_clamp_pressure_kpa);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_ROBOT_DOOR_CLOSED_READY_GATE) != 0) {
        uint32_t active = ((io->robot_door_closed_required != 0u) && (io->robot_door_closed == 0u)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_robot_door_wait_ticks, active, V41_ROBOT_DOOR_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(4u);
            v41_record_fault(4u, fault_bit, (int32_t)io->robot_door_closed);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_CUTTING_ROOM_EXHAUST_FAN_READY_GATE) != 0) {
        uint32_t active = ((io->exhaust_fan_ready_required != 0u) && (io->exhaust_fan_ready == 0u)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_exhaust_fan_wait_ticks, active, V41_EXHAUST_FAN_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(5u);
            v41_record_fault(5u, fault_bit, (int32_t)io->exhaust_fan_ready);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_TOOL_WASH_NOZZLE_CLOG_RETRY) != 0) {
        uint32_t active = ((io->tool_wash_nozzle_monitor != 0u) && (io->tool_wash_flow_percent < V41_TOOL_WASH_FLOW_MIN_PERCENT)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_tool_wash_wait_ticks, active, V41_TOOL_WASH_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(6u);
            g_ncFeatureBacklogV41Status.last_tool_wash_flow_percent = io->tool_wash_flow_percent;
            v41_record_fault(6u, fault_bit, io->tool_wash_flow_percent);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_AXIS_HOME_SENSOR_DUAL_MISMATCH_TRACE) != 0) {
        uint32_t mismatch = (io->axis_home_sensor_a_mask ^ io->axis_home_sensor_b_mask);
        uint32_t active = ((io->axis_home_sensor_check_request != 0u) && (mismatch != 0u)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_axis_home_wait_ticks, active, V41_AXIS_HOME_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(7u);
            g_ncFeatureBacklogV41Status.last_axis_mask = mismatch;
            v41_record_fault(7u, fault_bit, (int32_t)mismatch);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_NC_OPTION_LICENSE_MISMATCH_INHIBIT) != 0) {
        uint32_t missing = io->nc_option_required_mask & ~io->nc_option_licensed_mask;
        uint32_t active = ((io->nc_option_license_check_request != 0u) && (missing != 0u)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_license_wait_ticks, active, V41_LICENSE_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(8u);
            g_ncFeatureBacklogV41Status.last_license_missing_mask = missing;
            v41_record_fault(8u, fault_bit, (int32_t)missing);
        }
    }

    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_MAINTENANCE_MODE_KEY_REMOVED_HOLD) != 0) {
        uint32_t active = ((io->maintenance_mode_monitor != 0u) && (io->maintenance_mode_active != 0u) && (io->maintenance_key_present == 0u)) ? 1u : 0u;
        if (v41_wait_or_clear(&s_maint_key_wait_ticks, active, V41_MAINT_KEY_TIMEOUT_TICKS) != 0u) {
            fault_bit = V41_BIT(9u);
            v41_record_fault(9u, fault_bit, (int32_t)io->maintenance_key_present);
            g_ncFeatureBacklogV41Status.hold_count++;
        }
    }
}

int Api_RunNcFeatureBacklogV41SelfCheck(void)
{
    uint32_t index;
    if (NC_FEATURE_BACKLOG_V41_FEATURE_COUNT != 10u) {
        return NC_FEATURE_BACKLOG_V41_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_FEATURE_COUNT_V41 != 335u) {
        return NC_FEATURE_BACKLOG_V41_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_WORD_COUNT_V41 != 11u) {
        return NC_FEATURE_BACKLOG_V41_ERR_RANGE;
    }
    for (index = 0u; index < NC_FEATURE_BACKLOG_V41_FEATURE_COUNT; ++index) {
        if (k_v41_defs[index].feature_id != (NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID + index)) {
            return NC_FEATURE_BACKLOG_V41_ERR_RANGE;
        }
        if (k_v41_defs[index].name == 0) {
            return NC_FEATURE_BACKLOG_V41_ERR_NULL;
        }
    }
    return NC_FEATURE_BACKLOG_V41_OK;
}

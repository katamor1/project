#include "nc_feature_backlog_loader_environment.h"

#include <string.h>

#define V42_BIT(index_) (1u << (uint32_t)(index_))
#define V42_DEFAULT_FEED_OVERRIDE_PERCENT      (100)
#define V42_MIN_FEED_OVERRIDE_PERCENT          (5)
#define V42_SPINDLE_AIR_PRESSURE_MIN_KPA       (450)
#define V42_OIL_MIST_PRESSURE_MIN_KPA          (90)
#define V42_SPINDLE_ROTATION_RPM_MIN           (30)
#define V42_SERVO_TEMPERATURE_WARN_C           (72)
#define V42_SERVO_TEMPERATURE_LIMIT_C          (82)

#define V42_SPINDLE_AIR_TIMEOUT_TICKS          (2u)
#define V42_AXIS_LIMIT_TIMEOUT_TICKS           (2u)
#define V42_OIL_MIST_TIMEOUT_TICKS             (2u)
#define V42_LOADER_CHUCK_TIMEOUT_TICKS         (2u)
#define V42_AUTO_DOOR_TIMEOUT_TICKS            (4u)
#define V42_TABLE_SEAT_TIMEOUT_TICKS           (2u)
#define V42_DOOR_UNLOCK_TIMEOUT_TICKS          (1u)
#define V42_TOOL_RFID_TIMEOUT_TICKS            (3u)
#define V42_ROOM_LIGHT_TIMEOUT_TICKS           (2u)
#define V42_SERVO_TEMP_TIMEOUT_TICKS           (2u)

typedef struct V42_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V42_POLICY policy;
    uint32_t category;
    const char *name;
} V42_FEATURE_DEF;

static const V42_FEATURE_DEF k_v42_defs[NC_FEATURE_BACKLOG_V42_FEATURE_COUNT] = {
    { NC_FEATURE_V42_SPINDLE_AIR_PURGE_PRESSURE_GATE, NC_FEATURE_BACKLOG_V42_POLICY_GATE, 12u, "spindle air purge pressure low gate" },
    { NC_FEATURE_V42_AXIS_LIMIT_SWITCH_STUCK_TRACE, NC_FEATURE_BACKLOG_V42_POLICY_TRACE, 12u, "axis limit switch stuck trace" },
    { NC_FEATURE_V42_OIL_MIST_SUPPLY_LOW_HOLD, NC_FEATURE_BACKLOG_V42_POLICY_HOLD, 12u, "oil mist supply low hold" },
    { NC_FEATURE_V42_LOADER_CHUCK_CONFIRM_INTERLOCK, NC_FEATURE_BACKLOG_V42_POLICY_INTERLOCK, 12u, "loader chuck confirm interlock" },
    { NC_FEATURE_V42_AUTO_DOOR_MOTION_TIMEOUT_RETRY, NC_FEATURE_BACKLOG_V42_POLICY_RETRY, 12u, "auto door motion timeout retry" },
    { NC_FEATURE_V42_TABLE_SEATING_SENSOR_MISMATCH_HOLD, NC_FEATURE_BACKLOG_V42_POLICY_HOLD, 12u, "table seating sensor mismatch hold" },
    { NC_FEATURE_V42_DOOR_UNLOCK_DURING_SPINDLE_INHIBIT, NC_FEATURE_BACKLOG_V42_POLICY_INHIBIT, 12u, "door unlock during spindle rotation inhibit" },
    { NC_FEATURE_V42_TOOL_RFID_READ_FAIL_GATE, NC_FEATURE_BACKLOG_V42_POLICY_GATE, 12u, "tool RFID read fail gate" },
    { NC_FEATURE_V42_MACHINING_ROOM_LIGHT_FAULT_TRACE, NC_FEATURE_BACKLOG_V42_POLICY_TRACE, 12u, "machining room light fault trace" },
    { NC_FEATURE_V42_SERVO_TEMPERATURE_RISE_SLOWDOWN, NC_FEATURE_BACKLOG_V42_POLICY_SLOWDOWN, 12u, "servo temperature rise staged slowdown" }
};

NC_FEATURE_BACKLOG_V42_STATUS g_ncFeatureBacklogV42Status;

static uint32_t s_spindle_air_wait_ticks;
static uint32_t s_axis_limit_wait_ticks;
static uint32_t s_oil_mist_wait_ticks;
static uint32_t s_loader_chuck_wait_ticks;
static uint32_t s_auto_door_wait_ticks;
static uint32_t s_table_seat_wait_ticks;
static uint32_t s_door_unlock_wait_ticks;
static uint32_t s_tool_rfid_wait_ticks;
static uint32_t s_room_light_wait_ticks;
static uint32_t s_servo_temp_wait_ticks;

static uint32_t v42_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V42_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V42_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V42_FEATURE_BASE_ID;
}

static int32_t v42_clamp_i32(int32_t value, int32_t min_value, int32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int32_t v42_apply_slowdown(int32_t feed_override_percent, int32_t reduction_percent)
{
    int32_t next_value = feed_override_percent - reduction_percent;
    return v42_clamp_i32(next_value, V42_MIN_FEED_OVERRIDE_PERCENT, V42_DEFAULT_FEED_OVERRIDE_PERCENT);
}

static void v42_record_hit(uint32_t index, int32_t value)
{
    const V42_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V42_FEATURE_COUNT) {
        return;
    }

    def = &k_v42_defs[index];
    g_ncFeatureBacklogV42Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV42Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV42Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV42Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV42Status.cat_mask |= V42_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V42_POLICY_MONITOR:
        g_ncFeatureBacklogV42Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_GATE:
        g_ncFeatureBacklogV42Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_HOLD:
        g_ncFeatureBacklogV42Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_INTERLOCK:
        g_ncFeatureBacklogV42Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV42Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_TRACE:
        g_ncFeatureBacklogV42Status.trace_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_INHIBIT:
        g_ncFeatureBacklogV42Status.inhibit_count++;
        break;
    case NC_FEATURE_BACKLOG_V42_POLICY_RETRY:
        g_ncFeatureBacklogV42Status.retry_count++;
        break;
    default:
        g_ncFeatureBacklogV42Status.warning_count++;
        break;
    }
}

static void v42_record_fault(uint32_t index, uint32_t fault_bit, int32_t value)
{
    if (index >= NC_FEATURE_BACKLOG_V42_FEATURE_COUNT) {
        g_ncFeatureBacklogV42Status.warning_count++;
        return;
    }
    g_ncFeatureBacklogV42Status.last_fault_mask |= fault_bit;
    g_ncFeatureBacklogV42Status.last_trace_event_code = NC_FEATURE_BACKLOG_V42_TRACE_BASE + index;
    v42_record_hit(index, value);
}

static uint32_t v42_wait_or_clear(uint32_t *counter, uint32_t active, uint32_t threshold)
{
    if (counter == 0) {
        return 0u;
    }
    if (active == 0u) {
        *counter = 0u;
        return 0u;
    }
    if (*counter < threshold) {
        (*counter)++;
    }
    return (*counter >= threshold) ? 1u : 0u;
}

static void v42_reset_counters(void)
{
    s_spindle_air_wait_ticks = 0u;
    s_axis_limit_wait_ticks = 0u;
    s_oil_mist_wait_ticks = 0u;
    s_loader_chuck_wait_ticks = 0u;
    s_auto_door_wait_ticks = 0u;
    s_table_seat_wait_ticks = 0u;
    s_door_unlock_wait_ticks = 0u;
    s_tool_rfid_wait_ticks = 0u;
    s_room_light_wait_ticks = 0u;
    s_servo_temp_wait_ticks = 0u;
}

void NcFeatureBacklogV42_Init(void)
{
    memset(&g_ncFeatureBacklogV42Status, 0, sizeof(g_ncFeatureBacklogV42Status));
    g_ncFeatureBacklogV42Status.implemented_count = NC_FEATURE_BACKLOG_V42_FEATURE_COUNT;
    g_ncFeatureBacklogV42Status.last_feed_override_percent = V42_DEFAULT_FEED_OVERRIDE_PERCENT;
    v42_reset_counters();
}

int NcFeatureBacklogV42_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v42_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V42_FEATURE_COUNT) {
        return 0;
    }
    return ((g_ncFeatureBacklogV42Status.enabled_mask & V42_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV42Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v42_index_from_feature_id(feature_id);
    uint32_t scan;
    if (index >= NC_FEATURE_BACKLOG_V42_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V42_ERR_RANGE;
    }
    if (enabled != 0u) {
        g_ncFeatureBacklogV42Status.enabled_mask |= V42_BIT(index);
    } else {
        g_ncFeatureBacklogV42Status.enabled_mask &= ~V42_BIT(index);
    }
    g_ncFeatureBacklogV42Status.configured_count = 0u;
    for (scan = 0u; scan < NC_FEATURE_BACKLOG_V42_FEATURE_COUNT; ++scan) {
        if ((g_ncFeatureBacklogV42Status.enabled_mask & V42_BIT(scan)) != 0u) {
            g_ncFeatureBacklogV42Status.configured_count++;
        }
    }
    return NC_FEATURE_BACKLOG_V42_OK;
}

void Api_EnableAllNcFeatureBacklogV42Features(void)
{
    uint32_t index;
    g_ncFeatureBacklogV42Status.enabled_mask = 0u;
    for (index = 0u; index < NC_FEATURE_BACKLOG_V42_FEATURE_COUNT; ++index) {
        g_ncFeatureBacklogV42Status.enabled_mask |= V42_BIT(index);
    }
    g_ncFeatureBacklogV42Status.configured_count = NC_FEATURE_BACKLOG_V42_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV42Status(NC_FEATURE_BACKLOG_V42_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V42_ERR_NULL;
    }
    *out_status = g_ncFeatureBacklogV42Status;
    return NC_FEATURE_BACKLOG_V42_OK;
}

const char *NcFeatureBacklogV42_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v42_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V42_FEATURE_COUNT) {
        return "unknown";
    }
    return k_v42_defs[index].name;
}

void NcFeatureBacklogV42_ApplyBlockTs(NC_FEATURE_BACKLOG_V42_BLOCK *block)
{
    uint32_t index;
    if (block == 0) {
        g_ncFeatureBacklogV42Status.warning_count++;
        return;
    }
    g_ncFeatureBacklogV42Status.ts_apply_count++;
    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V42_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V42_FEATURE_COUNT; ++index) {
        const V42_FEATURE_DEF *def;
        uint32_t feature_bit;
        uint32_t trace;
        if ((g_ncFeatureBacklogV42Status.enabled_mask & V42_BIT(index)) == 0u) {
            continue;
        }
        def = &k_v42_defs[index];
        feature_bit = V42_BIT(index);
        trace = NC_FEATURE_BACKLOG_V42_TRACE_BASE + index;

        switch (def->feature_id) {
        case NC_FEATURE_V42_SPINDLE_AIR_PURGE_PRESSURE_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->aux_request_bits |= feature_bit;
            v42_record_hit(index, (int32_t)block->sequence_gate_mask);
            break;
        case NC_FEATURE_V42_AXIS_LIMIT_SWITCH_STUCK_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV42Status.last_trace_event_code = trace;
            v42_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V42_OIL_MIST_SUPPLY_LOW_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v42_record_hit(index, (int32_t)block->hold_request);
            break;
        case NC_FEATURE_V42_LOADER_CHUCK_CONFIRM_INTERLOCK:
            block->interlock_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v42_record_hit(index, (int32_t)block->interlock_mask);
            break;
        case NC_FEATURE_V42_AUTO_DOOR_MOTION_TIMEOUT_RETRY:
            block->retry_request_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v42_record_hit(index, (int32_t)block->retry_request_mask);
            break;
        case NC_FEATURE_V42_TABLE_SEATING_SENSOR_MISMATCH_HOLD:
            block->hold_request |= feature_bit;
            block->interlock_mask |= feature_bit;
            v42_record_hit(index, (int32_t)block->hold_request);
            break;
        case NC_FEATURE_V42_DOOR_UNLOCK_DURING_SPINDLE_INHIBIT:
            block->inhibit_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v42_record_hit(index, (int32_t)block->inhibit_mask);
            break;
        case NC_FEATURE_V42_TOOL_RFID_READ_FAIL_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->aux_request_bits |= feature_bit;
            v42_record_hit(index, (int32_t)block->sequence_gate_mask);
            break;
        case NC_FEATURE_V42_MACHINING_ROOM_LIGHT_FAULT_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV42Status.last_trace_event_code = trace;
            v42_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V42_SERVO_TEMPERATURE_RISE_SLOWDOWN:
            block->feed_override_percent = v42_apply_slowdown(block->feed_override_percent, 11);
            block->diagnostics_mask |= feature_bit;
            v42_record_hit(index, block->feed_override_percent);
            break;
        default:
            g_ncFeatureBacklogV42Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV42Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV42Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV42_RunRtTick(const NC_FEATURE_BACKLOG_V42_RT_IO *io)
{
    uint32_t fault_bit;
    if (io == 0) {
        g_ncFeatureBacklogV42Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV42Status.rt_tick_count++;
    g_ncFeatureBacklogV42Status.last_fault_mask = 0u;

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_SPINDLE_AIR_PURGE_PRESSURE_GATE) != 0) {
        uint32_t active = ((io->spindle_air_purge_required != 0u) && (io->spindle_air_pressure_kpa < V42_SPINDLE_AIR_PRESSURE_MIN_KPA)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_spindle_air_wait_ticks, active, V42_SPINDLE_AIR_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(0u);
            g_ncFeatureBacklogV42Status.last_pressure_kpa = io->spindle_air_pressure_kpa;
            v42_record_fault(0u, fault_bit, io->spindle_air_pressure_kpa);
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_AXIS_LIMIT_SWITCH_STUCK_TRACE) != 0) {
        uint32_t stuck = io->commanded_axis_mask & io->limit_switch_active_mask;
        uint32_t active = ((io->axis_limit_switch_monitor != 0u) && (stuck != 0u)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_axis_limit_wait_ticks, active, V42_AXIS_LIMIT_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(1u);
            g_ncFeatureBacklogV42Status.last_axis_mask = stuck;
            v42_record_fault(1u, fault_bit, (int32_t)stuck);
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_OIL_MIST_SUPPLY_LOW_HOLD) != 0) {
        uint32_t active = ((io->oil_mist_monitor != 0u) && (io->oil_mist_pressure_kpa < V42_OIL_MIST_PRESSURE_MIN_KPA)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_oil_mist_wait_ticks, active, V42_OIL_MIST_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(2u);
            g_ncFeatureBacklogV42Status.last_pressure_kpa = io->oil_mist_pressure_kpa;
            v42_record_fault(2u, fault_bit, io->oil_mist_pressure_kpa);
            g_ncFeatureBacklogV42Status.hold_count++;
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_LOADER_CHUCK_CONFIRM_INTERLOCK) != 0) {
        uint32_t invalid_pair = ((io->loader_chuck_open_signal != 0u) && (io->loader_chuck_closed_signal != 0u)) ? 1u : 0u;
        uint32_t missing_confirm = ((io->loader_chuck_open_signal == 0u) && (io->loader_chuck_closed_signal == 0u)) ? 1u : 0u;
        uint32_t active = ((io->loader_chuck_check_request != 0u) && ((invalid_pair != 0u) || (missing_confirm != 0u))) ? 1u : 0u;
        if (v42_wait_or_clear(&s_loader_chuck_wait_ticks, active, V42_LOADER_CHUCK_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(3u);
            v42_record_fault(3u, fault_bit, (int32_t)((io->loader_chuck_open_signal << 1u) | io->loader_chuck_closed_signal));
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_AUTO_DOOR_MOTION_TIMEOUT_RETRY) != 0) {
        uint32_t active = ((io->auto_door_command_active != 0u) && (io->auto_door_done == 0u)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_auto_door_wait_ticks, active, V42_AUTO_DOOR_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(4u);
            v42_record_fault(4u, fault_bit, (int32_t)io->auto_door_done);
            g_ncFeatureBacklogV42Status.retry_count++;
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_TABLE_SEATING_SENSOR_MISMATCH_HOLD) != 0) {
        uint32_t mismatch = io->table_seat_a_mask ^ io->table_seat_b_mask;
        uint32_t active = ((io->table_seat_check_request != 0u) && (mismatch != 0u)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_table_seat_wait_ticks, active, V42_TABLE_SEAT_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(5u);
            g_ncFeatureBacklogV42Status.last_table_mismatch_mask = mismatch;
            v42_record_fault(5u, fault_bit, (int32_t)mismatch);
            g_ncFeatureBacklogV42Status.hold_count++;
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_DOOR_UNLOCK_DURING_SPINDLE_INHIBIT) != 0) {
        uint32_t active = ((io->door_unlock_request != 0u) && (io->spindle_speed_rpm >= V42_SPINDLE_ROTATION_RPM_MIN)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_door_unlock_wait_ticks, active, V42_DOOR_UNLOCK_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(6u);
            g_ncFeatureBacklogV42Status.last_spindle_speed_rpm = io->spindle_speed_rpm;
            v42_record_fault(6u, fault_bit, io->spindle_speed_rpm);
            g_ncFeatureBacklogV42Status.inhibit_count++;
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_TOOL_RFID_READ_FAIL_GATE) != 0) {
        uint32_t active = ((io->tool_rfid_required != 0u) && (io->tool_rfid_read_ok == 0u)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_tool_rfid_wait_ticks, active, V42_TOOL_RFID_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(7u);
            g_ncFeatureBacklogV42Status.last_tool_rfid_ok = io->tool_rfid_read_ok;
            v42_record_fault(7u, fault_bit, (int32_t)io->tool_rfid_read_ok);
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_MACHINING_ROOM_LIGHT_FAULT_TRACE) != 0) {
        uint32_t active = ((io->machining_room_light_monitor != 0u) && (io->machining_room_light_ok == 0u)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_room_light_wait_ticks, active, V42_ROOM_LIGHT_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(8u);
            v42_record_fault(8u, fault_bit, (int32_t)io->machining_room_light_ok);
        }
    }

    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_SERVO_TEMPERATURE_RISE_SLOWDOWN) != 0) {
        uint32_t active = ((io->servo_temperature_monitor != 0u) && (io->servo_temperature_c >= V42_SERVO_TEMPERATURE_WARN_C)) ? 1u : 0u;
        if (v42_wait_or_clear(&s_servo_temp_wait_ticks, active, V42_SERVO_TEMP_TIMEOUT_TICKS) != 0u) {
            fault_bit = V42_BIT(9u);
            g_ncFeatureBacklogV42Status.last_axis_mask = io->servo_temperature_axis_mask;
            g_ncFeatureBacklogV42Status.last_temperature_c = io->servo_temperature_c;
            v42_record_fault(9u, fault_bit, io->servo_temperature_c);
            if (io->servo_temperature_c >= V42_SERVO_TEMPERATURE_LIMIT_C) {
                g_ncFeatureBacklogV42Status.slowdown_count++;
            }
        }
    }
}

int Api_RunNcFeatureBacklogV42SelfCheck(void)
{
    uint32_t index;
    if (NC_FEATURE_BACKLOG_V42_FEATURE_COUNT != 10u) {
        return NC_FEATURE_BACKLOG_V42_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_FEATURE_COUNT_V42 != 345u) {
        return NC_FEATURE_BACKLOG_V42_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_WORD_COUNT_V42 != 11u) {
        return NC_FEATURE_BACKLOG_V42_ERR_RANGE;
    }
    for (index = 0u; index < NC_FEATURE_BACKLOG_V42_FEATURE_COUNT; ++index) {
        if (k_v42_defs[index].feature_id != (NC_FEATURE_BACKLOG_V42_FEATURE_BASE_ID + index)) {
            return NC_FEATURE_BACKLOG_V42_ERR_RANGE;
        }
        if (k_v42_defs[index].name == 0) {
            return NC_FEATURE_BACKLOG_V42_ERR_NULL;
        }
    }
    return NC_FEATURE_BACKLOG_V42_OK;
}

#include "nc_feature_backlog_process_safety.h"

#include <string.h>

#define V40_BIT(index_) (1u << (uint32_t)(index_))
#define V40_DEFAULT_FEED_OVERRIDE_PERCENT           (100)
#define V40_MIN_FEED_OVERRIDE_PERCENT               (5)
#define V40_SPINDLE_VIBRATION_WARN_UM               (45)
#define V40_SPINDLE_VIBRATION_TRACE_UM              (70)
#define V40_TOOL_LENGTH_DELTA_HOLD_UM               (120)
#define V40_COOLANT_TEMP_WARN_C                     (42)
#define V40_COOLANT_TEMP_HIGH_C                     (48)
#define V40_LASER_INTRUSION_TIMEOUT_TICKS           (1u)
#define V40_SPINDLE_VIBRATION_TIMEOUT_TICKS         (2u)
#define V40_FEED_AXIS_LUBE_TIMEOUT_TICKS            (3u)
#define V40_PROGRAM_SIGNATURE_TIMEOUT_TICKS         (1u)
#define V40_CHUCK_SENSOR_TIMEOUT_TICKS              (2u)
#define V40_TOOL_BREAK_TIMEOUT_TICKS                (1u)
#define V40_TOOL_LENGTH_JUMP_TIMEOUT_TICKS          (1u)
#define V40_COOLANT_TEMP_TIMEOUT_TICKS              (2u)
#define V40_PALLET_ROBOT_READY_TIMEOUT_TICKS        (4u)
#define V40_SAFETY_PLC_HEARTBEAT_TIMEOUT_TICKS      (3u)

typedef struct V40_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V40_POLICY policy;
    uint32_t category;
    const char *name;
} V40_FEATURE_DEF;

static const V40_FEATURE_DEF k_v40_defs[NC_FEATURE_BACKLOG_V40_FEATURE_COUNT] = {
    { NC_FEATURE_V40_LASER_AREA_SCANNER_INTRUSION_HOLD,       NC_FEATURE_BACKLOG_V40_POLICY_HOLD,      10u, "laser area scanner intrusion hold" },
    { NC_FEATURE_V40_SPINDLE_VIBRATION_RISE_TRACE,            NC_FEATURE_BACKLOG_V40_POLICY_TRACE,     10u, "spindle vibration rise trace" },
    { NC_FEATURE_V40_FEED_AXIS_LUBE_PUMP_READY_GATE,          NC_FEATURE_BACKLOG_V40_POLICY_GATE,      10u, "feed axis lubrication pump ready gate" },
    { NC_FEATURE_V40_NC_PROGRAM_SIGNATURE_MISMATCH_INHIBIT,   NC_FEATURE_BACKLOG_V40_POLICY_INHIBIT,   10u, "NC program signature mismatch inhibit" },
    { NC_FEATURE_V40_CHUCK_SENSOR_DUAL_MISMATCH_INTERLOCK,    NC_FEATURE_BACKLOG_V40_POLICY_INTERLOCK, 10u, "chuck dual sensor mismatch interlock" },
    { NC_FEATURE_V40_TOOL_BREAK_AUTO_RETRACT_REQUEST,         NC_FEATURE_BACKLOG_V40_POLICY_RETRACT,   10u, "tool break auto retract request" },
    { NC_FEATURE_V40_TOOL_LENGTH_MEASURE_JUMP_HOLD,           NC_FEATURE_BACKLOG_V40_POLICY_HOLD,      10u, "tool length measurement jump hold" },
    { NC_FEATURE_V40_COOLANT_TEMPERATURE_HIGH_SLOWDOWN,       NC_FEATURE_BACKLOG_V40_POLICY_SLOWDOWN,  10u, "coolant temperature high slowdown" },
    { NC_FEATURE_V40_PALLET_ROBOT_READY_TIMEOUT_RETRY,        NC_FEATURE_BACKLOG_V40_POLICY_RETRY,     10u, "pallet robot ready timeout retry" },
    { NC_FEATURE_V40_SAFETY_PLC_HEARTBEAT_LOST_HOLD,          NC_FEATURE_BACKLOG_V40_POLICY_HOLD,      10u, "safety PLC heartbeat lost hold" }
};

NC_FEATURE_BACKLOG_V40_STATUS g_ncFeatureBacklogV40Status;

static uint32_t s_laser_intrusion_wait_ticks;
static uint32_t s_spindle_vibration_wait_ticks;
static uint32_t s_feed_axis_lube_wait_ticks;
static uint32_t s_program_signature_wait_ticks;
static uint32_t s_chuck_sensor_wait_ticks;
static uint32_t s_tool_break_wait_ticks;
static uint32_t s_tool_length_jump_wait_ticks;
static uint32_t s_coolant_temp_wait_ticks;
static uint32_t s_pallet_robot_ready_wait_ticks;
static uint32_t s_safety_plc_heartbeat_wait_ticks;
static uint32_t s_last_safety_plc_heartbeat_counter;
static uint32_t s_heartbeat_initialized;

static uint32_t v40_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V40_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID;
}

static int32_t v40_abs_i32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static int32_t v40_clamp_i32(int32_t value, int32_t min_value, int32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int32_t v40_apply_slowdown(int32_t feed_override_percent, int32_t reduction_percent)
{
    int32_t next_value = feed_override_percent - reduction_percent;
    return v40_clamp_i32(next_value, V40_MIN_FEED_OVERRIDE_PERCENT, V40_DEFAULT_FEED_OVERRIDE_PERCENT);
}

static void v40_record_hit(uint32_t index, int32_t value)
{
    const V40_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V40_FEATURE_COUNT) {
        return;
    }

    def = &k_v40_defs[index];
    g_ncFeatureBacklogV40Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV40Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV40Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV40Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV40Status.cat_mask |= V40_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V40_POLICY_MONITOR:
        g_ncFeatureBacklogV40Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_GATE:
        g_ncFeatureBacklogV40Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_HOLD:
        g_ncFeatureBacklogV40Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_INTERLOCK:
        g_ncFeatureBacklogV40Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV40Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_TRACE:
        g_ncFeatureBacklogV40Status.trace_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_INHIBIT:
        g_ncFeatureBacklogV40Status.inhibit_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_RETRY:
        g_ncFeatureBacklogV40Status.retry_count++;
        break;
    case NC_FEATURE_BACKLOG_V40_POLICY_RETRACT:
        g_ncFeatureBacklogV40Status.retract_count++;
        break;
    default:
        g_ncFeatureBacklogV40Status.warning_count++;
        break;
    }
}

static void v40_record_trace(uint32_t index)
{
    uint32_t trace;

    if (index >= NC_FEATURE_BACKLOG_V40_FEATURE_COUNT) {
        return;
    }

    trace = NC_FEATURE_BACKLOG_V40_TRACE_BASE + index;
    g_ncFeatureBacklogV40Status.last_trace_event_code = trace;
}

static void v40_record_fault(uint32_t index, uint32_t fault_bit, int32_t value)
{
    g_ncFeatureBacklogV40Status.last_fault_mask |= fault_bit;
    v40_record_trace(index);
    v40_record_hit(index, value);
}

static uint32_t v40_wait_or_clear(uint32_t *wait_ticks, uint32_t active, uint32_t timeout_ticks)
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

static void v40_reset_counters(void)
{
    s_laser_intrusion_wait_ticks = 0u;
    s_spindle_vibration_wait_ticks = 0u;
    s_feed_axis_lube_wait_ticks = 0u;
    s_program_signature_wait_ticks = 0u;
    s_chuck_sensor_wait_ticks = 0u;
    s_tool_break_wait_ticks = 0u;
    s_tool_length_jump_wait_ticks = 0u;
    s_coolant_temp_wait_ticks = 0u;
    s_pallet_robot_ready_wait_ticks = 0u;
    s_safety_plc_heartbeat_wait_ticks = 0u;
    s_last_safety_plc_heartbeat_counter = 0u;
    s_heartbeat_initialized = 0u;
}

void NcFeatureBacklogV40_Init(void)
{
    memset(&g_ncFeatureBacklogV40Status, 0, sizeof(g_ncFeatureBacklogV40Status));
    g_ncFeatureBacklogV40Status.implemented_count = NC_FEATURE_BACKLOG_V40_FEATURE_COUNT;
    g_ncFeatureBacklogV40Status.last_feed_override_percent = V40_DEFAULT_FEED_OVERRIDE_PERCENT;
    v40_reset_counters();
}

int NcFeatureBacklogV40_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v40_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V40_FEATURE_COUNT) {
        return 0;
    }

    return ((g_ncFeatureBacklogV40Status.enabled_mask & V40_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV40Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v40_index_from_feature_id(feature_id);
    uint32_t scan;

    if (index >= NC_FEATURE_BACKLOG_V40_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V40_ERR_RANGE;
    }

    if (enabled != 0u) {
        g_ncFeatureBacklogV40Status.enabled_mask |= V40_BIT(index);
    } else {
        g_ncFeatureBacklogV40Status.enabled_mask &= ~V40_BIT(index);
    }

    g_ncFeatureBacklogV40Status.configured_count = 0u;
    for (scan = 0u; scan < NC_FEATURE_BACKLOG_V40_FEATURE_COUNT; ++scan) {
        if ((g_ncFeatureBacklogV40Status.enabled_mask & V40_BIT(scan)) != 0u) {
            g_ncFeatureBacklogV40Status.configured_count++;
        }
    }

    return NC_FEATURE_BACKLOG_V40_OK;
}

void Api_EnableAllNcFeatureBacklogV40Features(void)
{
    uint32_t index;

    g_ncFeatureBacklogV40Status.enabled_mask = 0u;
    for (index = 0u; index < NC_FEATURE_BACKLOG_V40_FEATURE_COUNT; ++index) {
        g_ncFeatureBacklogV40Status.enabled_mask |= V40_BIT(index);
    }
    g_ncFeatureBacklogV40Status.configured_count = NC_FEATURE_BACKLOG_V40_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV40Status(NC_FEATURE_BACKLOG_V40_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V40_ERR_NULL;
    }

    *out_status = g_ncFeatureBacklogV40Status;
    return NC_FEATURE_BACKLOG_V40_OK;
}

const char *NcFeatureBacklogV40_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v40_index_from_feature_id(feature_id);

    if (index >= NC_FEATURE_BACKLOG_V40_FEATURE_COUNT) {
        return "unknown";
    }

    return k_v40_defs[index].name;
}

void NcFeatureBacklogV40_ApplyBlockTs(NC_FEATURE_BACKLOG_V40_BLOCK *block)
{
    uint32_t index;

    if (block == 0) {
        g_ncFeatureBacklogV40Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV40Status.ts_apply_count++;

    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V40_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V40_FEATURE_COUNT; ++index) {
        const V40_FEATURE_DEF *def;
        uint32_t feature_bit;
        uint32_t trace;

        if ((g_ncFeatureBacklogV40Status.enabled_mask & V40_BIT(index)) == 0u) {
            continue;
        }

        def = &k_v40_defs[index];
        feature_bit = V40_BIT(index);
        trace = NC_FEATURE_BACKLOG_V40_TRACE_BASE + index;

        switch (def->feature_id) {
        case NC_FEATURE_V40_LASER_AREA_SCANNER_INTRUSION_HOLD:
            block->hold_request |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v40_record_hit(index, (int32_t)feature_bit);
            break;
        case NC_FEATURE_V40_SPINDLE_VIBRATION_RISE_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV40Status.last_trace_event_code = trace;
            v40_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V40_FEED_AXIS_LUBE_PUMP_READY_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->aux_request_bits |= feature_bit;
            v40_record_hit(index, (int32_t)block->aux_request_bits);
            break;
        case NC_FEATURE_V40_NC_PROGRAM_SIGNATURE_MISMATCH_INHIBIT:
            block->inhibit_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v40_record_hit(index, (int32_t)block->inhibit_mask);
            break;
        case NC_FEATURE_V40_CHUCK_SENSOR_DUAL_MISMATCH_INTERLOCK:
            block->interlock_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v40_record_hit(index, block->axis_target[0]);
            break;
        case NC_FEATURE_V40_TOOL_BREAK_AUTO_RETRACT_REQUEST:
            block->retract_request_mask |= feature_bit;
            block->hold_request |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV40Status.last_trace_event_code = trace;
            v40_record_hit(index, (int32_t)block->retract_request_mask);
            break;
        case NC_FEATURE_V40_TOOL_LENGTH_MEASURE_JUMP_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v40_record_hit(index, block->axis_target[2]);
            break;
        case NC_FEATURE_V40_COOLANT_TEMPERATURE_HIGH_SLOWDOWN:
            block->feed_override_percent = v40_apply_slowdown(block->feed_override_percent, 7);
            block->diagnostics_mask |= feature_bit;
            v40_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V40_PALLET_ROBOT_READY_TIMEOUT_RETRY:
            block->retry_request_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v40_record_hit(index, (int32_t)block->retry_request_mask);
            break;
        case NC_FEATURE_V40_SAFETY_PLC_HEARTBEAT_LOST_HOLD:
            block->hold_request |= feature_bit;
            block->interlock_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV40Status.last_trace_event_code = trace;
            block->feed_override_percent = v40_apply_slowdown(block->feed_override_percent, 12);
            v40_record_hit(index, block->feed_override_percent);
            break;
        default:
            g_ncFeatureBacklogV40Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV40Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV40Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV40_RunRtTick(const NC_FEATURE_BACKLOG_V40_RT_IO *io)
{
    uint32_t fault_bit;

    if (io == 0) {
        g_ncFeatureBacklogV40Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV40Status.rt_tick_count++;
    g_ncFeatureBacklogV40Status.last_fault_mask = 0u;

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_LASER_AREA_SCANNER_INTRUSION_HOLD) != 0) {
        uint32_t active = ((io->laser_area_scanner_required != 0u) && (io->laser_area_intrusion_detected != 0u)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_laser_intrusion_wait_ticks, active, V40_LASER_INTRUSION_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(0u);
            v40_record_fault(0u, fault_bit, (int32_t)io->laser_area_intrusion_detected);
            g_ncFeatureBacklogV40Status.hold_count++;
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_SPINDLE_VIBRATION_RISE_TRACE) != 0) {
        uint32_t active = ((io->spindle_vibration_monitor != 0u) && (io->spindle_vibration_um >= V40_SPINDLE_VIBRATION_WARN_UM)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_spindle_vibration_wait_ticks, active, V40_SPINDLE_VIBRATION_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(1u);
            g_ncFeatureBacklogV40Status.last_spindle_vibration_um = io->spindle_vibration_um;
            v40_record_fault(1u, fault_bit, io->spindle_vibration_um);
            if (io->spindle_vibration_um >= V40_SPINDLE_VIBRATION_TRACE_UM) {
                g_ncFeatureBacklogV40Status.trace_count++;
            }
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_FEED_AXIS_LUBE_PUMP_READY_GATE) != 0) {
        uint32_t missing = io->feed_axis_lube_required_mask & ~io->feed_axis_lube_ready_mask;
        if (v40_wait_or_clear(&s_feed_axis_lube_wait_ticks, missing != 0u, V40_FEED_AXIS_LUBE_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(2u);
            g_ncFeatureBacklogV40Status.last_axis_mask = missing;
            v40_record_fault(2u, fault_bit, (int32_t)missing);
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_NC_PROGRAM_SIGNATURE_MISMATCH_INHIBIT) != 0) {
        uint32_t active = ((io->nc_program_signature_check_request != 0u) && (io->nc_program_signature_ok == 0u)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_program_signature_wait_ticks, active, V40_PROGRAM_SIGNATURE_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(3u);
            v40_record_fault(3u, fault_bit, (int32_t)io->nc_program_signature_ok);
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_CHUCK_SENSOR_DUAL_MISMATCH_INTERLOCK) != 0) {
        uint32_t mismatch = ((io->chuck_dual_sensor_check_request != 0u) && (io->chuck_sensor_a_closed != io->chuck_sensor_b_closed)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_chuck_sensor_wait_ticks, mismatch, V40_CHUCK_SENSOR_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(4u);
            g_ncFeatureBacklogV40Status.last_sensor_a = io->chuck_sensor_a_closed;
            g_ncFeatureBacklogV40Status.last_sensor_b = io->chuck_sensor_b_closed;
            v40_record_fault(4u, fault_bit, (int32_t)io->chuck_sensor_a_closed);
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_TOOL_BREAK_AUTO_RETRACT_REQUEST) != 0) {
        uint32_t active = ((io->tool_break_monitor_request != 0u) && (io->tool_break_detected != 0u)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_tool_break_wait_ticks, active, V40_TOOL_BREAK_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(5u);
            v40_record_fault(5u, fault_bit, (int32_t)io->tool_break_detected);
            g_ncFeatureBacklogV40Status.retract_count++;
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_TOOL_LENGTH_MEASURE_JUMP_HOLD) != 0) {
        int32_t delta = v40_abs_i32(io->tool_length_current_um - io->tool_length_last_um);
        uint32_t active = ((io->tool_length_measure_check_request != 0u) && (delta >= V40_TOOL_LENGTH_DELTA_HOLD_UM)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_tool_length_jump_wait_ticks, active, V40_TOOL_LENGTH_JUMP_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(6u);
            g_ncFeatureBacklogV40Status.last_tool_length_delta_um = delta;
            v40_record_fault(6u, fault_bit, delta);
            g_ncFeatureBacklogV40Status.hold_count++;
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_COOLANT_TEMPERATURE_HIGH_SLOWDOWN) != 0) {
        uint32_t active = ((io->coolant_temperature_monitor != 0u) && (io->coolant_temperature_c >= V40_COOLANT_TEMP_WARN_C)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_coolant_temp_wait_ticks, active, V40_COOLANT_TEMP_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(7u);
            g_ncFeatureBacklogV40Status.last_coolant_temperature_c = io->coolant_temperature_c;
            v40_record_fault(7u, fault_bit, io->coolant_temperature_c);
            if (io->coolant_temperature_c >= V40_COOLANT_TEMP_HIGH_C) {
                g_ncFeatureBacklogV40Status.slowdown_count++;
            }
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_PALLET_ROBOT_READY_TIMEOUT_RETRY) != 0) {
        uint32_t active = ((io->pallet_robot_ready_required != 0u) && (io->pallet_robot_ready == 0u)) ? 1u : 0u;
        if (v40_wait_or_clear(&s_pallet_robot_ready_wait_ticks, active, V40_PALLET_ROBOT_READY_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(8u);
            v40_record_fault(8u, fault_bit, (int32_t)io->pallet_robot_ready);
        }
    }

    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_SAFETY_PLC_HEARTBEAT_LOST_HOLD) != 0) {
        uint32_t heartbeat_lost = 0u;
        if (io->safety_plc_heartbeat_monitor != 0u) {
            if (s_heartbeat_initialized == 0u) {
                s_last_safety_plc_heartbeat_counter = io->safety_plc_heartbeat_counter;
                s_heartbeat_initialized = 1u;
            } else if (s_last_safety_plc_heartbeat_counter == io->safety_plc_heartbeat_counter) {
                heartbeat_lost = 1u;
            } else {
                s_last_safety_plc_heartbeat_counter = io->safety_plc_heartbeat_counter;
            }
        }
        if (v40_wait_or_clear(&s_safety_plc_heartbeat_wait_ticks, heartbeat_lost, V40_SAFETY_PLC_HEARTBEAT_TIMEOUT_TICKS) != 0u) {
            fault_bit = V40_BIT(9u);
            g_ncFeatureBacklogV40Status.last_heartbeat_counter = io->safety_plc_heartbeat_counter;
            v40_record_fault(9u, fault_bit, (int32_t)io->safety_plc_heartbeat_counter);
            g_ncFeatureBacklogV40Status.hold_count++;
        }
    }
}

int Api_RunNcFeatureBacklogV40SelfCheck(void)
{
    uint32_t index;

    if (NC_FEATURE_BACKLOG_V40_FEATURE_COUNT != 10u) {
        return NC_FEATURE_BACKLOG_V40_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_FEATURE_COUNT_V40 != 325u) {
        return NC_FEATURE_BACKLOG_V40_ERR_RANGE;
    }
    if (NC_IMPL_BACKLOG_WORD_COUNT_V40 != 11u) {
        return NC_FEATURE_BACKLOG_V40_ERR_RANGE;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V40_FEATURE_COUNT; ++index) {
        if (k_v40_defs[index].feature_id != (NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID + index)) {
            return NC_FEATURE_BACKLOG_V40_ERR_RANGE;
        }
        if (k_v40_defs[index].name == 0) {
            return NC_FEATURE_BACKLOG_V40_ERR_NULL;
        }
    }

    return NC_FEATURE_BACKLOG_V40_OK;
}

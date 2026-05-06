#include "nc_feature_backlog_readiness_sensors.h"

#include <string.h>

#define V37_BIT(index_) (1u << (uint32_t)(index_))
#define V37_DEFAULT_FEED_OVERRIDE_PERCENT (100)
#define V37_MIN_FEED_OVERRIDE_PERCENT     (5)
#define V37_SPINDLE_TEMP_WARN_C_X10       (550)
#define V37_SPINDLE_TEMP_HOLD_C_X10       (700)
#define V37_SPINDLE_FLOW_TIMEOUT_TICKS    (3u)
#define V37_SERVO_FAN_TIMEOUT_TICKS       (3u)
#define V37_DOOR_MISMATCH_TIMEOUT_TICKS   (2u)
#define V37_MAGAZINE_HOME_TIMEOUT_TICKS   (5u)
#define V37_HYDRAULIC_READY_TIMEOUT_TICKS (4u)
#define V37_WORK_AIR_RETRY_TIMEOUT_TICKS  (3u)
#define V37_CHIP_FLUSH_TIMEOUT_TICKS      (3u)
#define V37_ENABLE_SWITCH_TIMEOUT_TICKS   (2u)
#define V37_PROBE_BATTERY_TIMEOUT_TICKS   (1u)
#define V37_ABSENC_WARN_TIMEOUT_TICKS     (2u)

typedef struct V37_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V37_POLICY policy;
    uint32_t category;
    const char *name;
} V37_FEATURE_DEF;

static const V37_FEATURE_DEF k_v37_defs[NC_FEATURE_BACKLOG_V37_FEATURE_COUNT] = {
    { NC_FEATURE_V37_SPINDLE_CHILLER_FLOW_LOW_SLOWDOWN,      NC_FEATURE_BACKLOG_V37_POLICY_SLOWDOWN, 9u, "spindle chiller flow low staged slowdown" },
    { NC_FEATURE_V37_AXIS_SERVO_FAN_STOP_HOLD,               NC_FEATURE_BACKLOG_V37_POLICY_HOLD,     9u, "axis servo fan stop hold" },
    { NC_FEATURE_V37_DOOR_LOCK_DUAL_CHANNEL_MISMATCH_TRACE,  NC_FEATURE_BACKLOG_V37_POLICY_TRACE,    9u, "door lock dual channel mismatch trace" },
    { NC_FEATURE_V37_TOOL_MAGAZINE_HOME_TIMEOUT_INTERLOCK,   NC_FEATURE_BACKLOG_V37_POLICY_INTERLOCK,9u, "tool magazine home timeout interlock" },
    { NC_FEATURE_V37_HYDRAULIC_PRESSURE_RISE_READY_GATE,     NC_FEATURE_BACKLOG_V37_POLICY_GATE,     9u, "hydraulic pressure rise ready gate" },
    { NC_FEATURE_V37_WORK_AIR_SEAT_CHECK_RETRY_GATE,         NC_FEATURE_BACKLOG_V37_POLICY_RETRY,    9u, "work air seat check retry gate" },
    { NC_FEATURE_V37_CHIP_FLUSH_PRESSURE_LOW_HOLD,           NC_FEATURE_BACKLOG_V37_POLICY_HOLD,     9u, "chip flush pressure low hold" },
    { NC_FEATURE_V37_OPERATOR_PANEL_ENABLE_SWITCH_GUARD,     NC_FEATURE_BACKLOG_V37_POLICY_INTERLOCK,9u, "operator panel enable switch guard" },
    { NC_FEATURE_V37_PROBE_BATTERY_LOW_MEASURE_INHIBIT,      NC_FEATURE_BACKLOG_V37_POLICY_INHIBIT,  9u, "probe battery low measure inhibit" },
    { NC_FEATURE_V37_ABSOLUTE_ENCODER_BATTERY_WARN_TRACE,    NC_FEATURE_BACKLOG_V37_POLICY_TRACE,    9u, "absolute encoder battery warning trace" }
};

NC_FEATURE_BACKLOG_V37_STATUS g_ncFeatureBacklogV37Status;

static uint32_t s_spindle_flow_wait_ticks;
static uint32_t s_servo_fan_wait_ticks;
static uint32_t s_door_mismatch_wait_ticks;
static uint32_t s_magazine_home_wait_ticks;
static uint32_t s_hydraulic_ready_wait_ticks;
static uint32_t s_work_air_retry_wait_ticks;
static uint32_t s_chip_flush_wait_ticks;
static uint32_t s_enable_switch_wait_ticks;
static uint32_t s_probe_battery_wait_ticks;
static uint32_t s_absenc_warn_wait_ticks;

static uint32_t v37_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V37_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V37_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V37_FEATURE_BASE_ID;
}

static void v37_record_hit(uint32_t index, int32_t value)
{
    const V37_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) {
        return;
    }

    def = &k_v37_defs[index];
    g_ncFeatureBacklogV37Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV37Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV37Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV37Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV37Status.cat_mask |= V37_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V37_POLICY_MONITOR:
        g_ncFeatureBacklogV37Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_GATE:
        g_ncFeatureBacklogV37Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_HOLD:
        g_ncFeatureBacklogV37Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_INTERLOCK:
        g_ncFeatureBacklogV37Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV37Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_TRACE:
        g_ncFeatureBacklogV37Status.trace_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_INHIBIT:
        g_ncFeatureBacklogV37Status.inhibit_count++;
        break;
    case NC_FEATURE_BACKLOG_V37_POLICY_RETRY:
        g_ncFeatureBacklogV37Status.retry_count++;
        break;
    default:
        g_ncFeatureBacklogV37Status.warning_count++;
        break;
    }
}

static int32_t v37_clamp_feed_override(int32_t value)
{
    if (value < V37_MIN_FEED_OVERRIDE_PERCENT) {
        return V37_MIN_FEED_OVERRIDE_PERCENT;
    }
    if (value > V37_DEFAULT_FEED_OVERRIDE_PERCENT) {
        return V37_DEFAULT_FEED_OVERRIDE_PERCENT;
    }
    return value;
}

void NcFeatureBacklogV37_Init(void)
{
    uint32_t i;

    memset(&g_ncFeatureBacklogV37Status, 0, sizeof(g_ncFeatureBacklogV37Status));
    g_ncFeatureBacklogV37Status.implemented_count = NC_FEATURE_BACKLOG_V37_FEATURE_COUNT;
    g_ncFeatureBacklogV37Status.configured_count = 0u;
    g_ncFeatureBacklogV37Status.last_feed_override_percent = V37_DEFAULT_FEED_OVERRIDE_PERCENT;

    for (i = 0u; i < NC_FEATURE_BACKLOG_V37_FEATURE_COUNT; ++i) {
        g_ncFeatureBacklogV37Status.per_feature_last_value[i] = 0;
    }

    s_spindle_flow_wait_ticks = 0u;
    s_servo_fan_wait_ticks = 0u;
    s_door_mismatch_wait_ticks = 0u;
    s_magazine_home_wait_ticks = 0u;
    s_hydraulic_ready_wait_ticks = 0u;
    s_work_air_retry_wait_ticks = 0u;
    s_chip_flush_wait_ticks = 0u;
    s_enable_switch_wait_ticks = 0u;
    s_probe_battery_wait_ticks = 0u;
    s_absenc_warn_wait_ticks = 0u;
}

int NcFeatureBacklogV37_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v37_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) {
        return 0;
    }
    return ((g_ncFeatureBacklogV37Status.enabled_mask & V37_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV37Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v37_index_from_feature_id(feature_id);
    uint32_t mask;

    if (index >= NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V37_ERR_RANGE;
    }

    mask = V37_BIT(index);
    if (enabled != 0u) {
        if ((g_ncFeatureBacklogV37Status.enabled_mask & mask) == 0u) {
            g_ncFeatureBacklogV37Status.configured_count++;
        }
        g_ncFeatureBacklogV37Status.enabled_mask |= mask;
    } else {
        if ((g_ncFeatureBacklogV37Status.enabled_mask & mask) != 0u &&
            g_ncFeatureBacklogV37Status.configured_count > 0u) {
            g_ncFeatureBacklogV37Status.configured_count--;
        }
        g_ncFeatureBacklogV37Status.enabled_mask &= ~mask;
    }

    return NC_FEATURE_BACKLOG_V37_OK;
}

void Api_EnableAllNcFeatureBacklogV37Features(void)
{
    g_ncFeatureBacklogV37Status.enabled_mask = (1u << NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) - 1u;
    g_ncFeatureBacklogV37Status.configured_count = NC_FEATURE_BACKLOG_V37_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV37Status(NC_FEATURE_BACKLOG_V37_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V37_ERR_NULL;
    }
    *out_status = g_ncFeatureBacklogV37Status;
    return NC_FEATURE_BACKLOG_V37_OK;
}

const char *NcFeatureBacklogV37_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v37_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) {
        return "unknown";
    }
    return k_v37_defs[index].name;
}

void NcFeatureBacklogV37_ApplyBlockTs(NC_FEATURE_BACKLOG_V37_BLOCK *block)
{
    uint32_t index;

    if (block == 0) {
        g_ncFeatureBacklogV37Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV37Status.ts_apply_count++;
    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V37_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V37_FEATURE_COUNT; ++index) {
        uint32_t feature_bit = V37_BIT(index);
        uint32_t trace = NC_FEATURE_BACKLOG_V37_TRACE_BASE + index;

        if ((g_ncFeatureBacklogV37Status.enabled_mask & feature_bit) == 0u) {
            continue;
        }

        switch (k_v37_defs[index].feature_id) {
        case NC_FEATURE_V37_SPINDLE_CHILLER_FLOW_LOW_SLOWDOWN:
            block->diagnostics_mask |= feature_bit;
            block->feed_override_percent = v37_clamp_feed_override(block->feed_override_percent - 7);
            v37_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V37_AXIS_SERVO_FAN_STOP_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v37_record_hit(index, block->axis_target[1]);
            break;
        case NC_FEATURE_V37_DOOR_LOCK_DUAL_CHANNEL_MISMATCH_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV37Status.last_trace_event_code = trace;
            v37_record_hit(index, block->axis_target[2]);
            break;
        case NC_FEATURE_V37_TOOL_MAGAZINE_HOME_TIMEOUT_INTERLOCK:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v37_record_hit(index, block->axis_target[3]);
            break;
        case NC_FEATURE_V37_HYDRAULIC_PRESSURE_RISE_READY_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v37_record_hit(index, block->axis_target[4]);
            break;
        case NC_FEATURE_V37_WORK_AIR_SEAT_CHECK_RETRY_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->retry_request_mask |= feature_bit;
            v37_record_hit(index, block->axis_target[5]);
            break;
        case NC_FEATURE_V37_CHIP_FLUSH_PRESSURE_LOW_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v37_record_hit(index, block->axis_target[6]);
            break;
        case NC_FEATURE_V37_OPERATOR_PANEL_ENABLE_SWITCH_GUARD:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v37_record_hit(index, block->axis_target[7]);
            break;
        case NC_FEATURE_V37_PROBE_BATTERY_LOW_MEASURE_INHIBIT:
            block->inhibit_mask |= feature_bit;
            block->sequence_gate_mask |= feature_bit;
            v37_record_hit(index, (int32_t)block->inhibit_mask);
            break;
        case NC_FEATURE_V37_ABSOLUTE_ENCODER_BATTERY_WARN_TRACE:
            block->diagnostics_mask |= feature_bit;
            block->trace_event_code = trace;
            g_ncFeatureBacklogV37Status.last_trace_event_code = trace;
            v37_record_hit(index, (int32_t)trace);
            break;
        default:
            g_ncFeatureBacklogV37Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV37Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV37Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV37_RunRtTick(const NC_FEATURE_BACKLOG_V37_RT_IO *io)
{
    if (io == 0) {
        g_ncFeatureBacklogV37Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV37Status.rt_tick_count++;

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_SPINDLE_CHILLER_FLOW_LOW_SLOWDOWN) != 0) {
        if (io->spindle_chiller_on != 0u &&
            (io->spindle_chiller_flow_ok == 0u || io->spindle_temp_c_x10 >= V37_SPINDLE_TEMP_WARN_C_X10)) {
            s_spindle_flow_wait_ticks++;
            if (s_spindle_flow_wait_ticks >= V37_SPINDLE_FLOW_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(0u);
                v37_record_hit(0u, io->spindle_temp_c_x10);
                if (io->spindle_temp_c_x10 >= V37_SPINDLE_TEMP_HOLD_C_X10) {
                    g_ncFeatureBacklogV37Status.hold_count++;
                }
            }
        } else {
            s_spindle_flow_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_AXIS_SERVO_FAN_STOP_HOLD) != 0) {
        uint32_t monitored = io->servo_ready_mask & io->servo_monitored_mask;
        uint32_t fan_ng = monitored & ~io->servo_fan_ok_mask;
        if (fan_ng != 0u) {
            s_servo_fan_wait_ticks++;
            if (s_servo_fan_wait_ticks >= V37_SERVO_FAN_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(1u);
                g_ncFeatureBacklogV37Status.last_axis_mask = fan_ng;
                v37_record_hit(1u, (int32_t)fan_ng);
            }
        } else {
            s_servo_fan_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_DOOR_LOCK_DUAL_CHANNEL_MISMATCH_TRACE) != 0) {
        if (io->door_lock_command != 0u && io->door_lock_ch1 != io->door_lock_ch2) {
            s_door_mismatch_wait_ticks++;
            if (s_door_mismatch_wait_ticks >= V37_DOOR_MISMATCH_TIMEOUT_TICKS) {
                uint32_t trace = NC_FEATURE_BACKLOG_V37_TRACE_BASE + 2u;
                g_ncFeatureBacklogV37Status.last_trace_event_code = trace;
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(2u);
                v37_record_hit(2u, (int32_t)trace);
            }
        } else {
            s_door_mismatch_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_TOOL_MAGAZINE_HOME_TIMEOUT_INTERLOCK) != 0) {
        if (io->tool_magazine_home_request != 0u && io->tool_magazine_home_sensor == 0u) {
            s_magazine_home_wait_ticks++;
            if (s_magazine_home_wait_ticks >= V37_MAGAZINE_HOME_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(3u);
                v37_record_hit(3u, (int32_t)s_magazine_home_wait_ticks);
            }
        } else {
            s_magazine_home_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_HYDRAULIC_PRESSURE_RISE_READY_GATE) != 0) {
        if (io->hydraulic_ready_request != 0u &&
            (io->hydraulic_pump_on == 0u || io->hydraulic_pressure_ok == 0u)) {
            s_hydraulic_ready_wait_ticks++;
            if (s_hydraulic_ready_wait_ticks >= V37_HYDRAULIC_READY_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(4u);
                v37_record_hit(4u, (int32_t)s_hydraulic_ready_wait_ticks);
            }
        } else {
            s_hydraulic_ready_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_WORK_AIR_SEAT_CHECK_RETRY_GATE) != 0) {
        if (io->work_air_seat_check_request != 0u &&
            (io->work_air_blow_on == 0u || io->work_air_seat_ok == 0u)) {
            s_work_air_retry_wait_ticks++;
            if (s_work_air_retry_wait_ticks >= V37_WORK_AIR_RETRY_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(5u);
                v37_record_hit(5u, (int32_t)s_work_air_retry_wait_ticks);
            }
        } else {
            s_work_air_retry_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_CHIP_FLUSH_PRESSURE_LOW_HOLD) != 0) {
        if (io->chip_flush_pump_on != 0u && io->chip_flush_pressure_ok == 0u) {
            s_chip_flush_wait_ticks++;
            if (s_chip_flush_wait_ticks >= V37_CHIP_FLUSH_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(6u);
                v37_record_hit(6u, (int32_t)s_chip_flush_wait_ticks);
            }
        } else {
            s_chip_flush_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_OPERATOR_PANEL_ENABLE_SWITCH_GUARD) != 0) {
        if (io->operator_panel_active != 0u && io->operator_enable_required != 0u && io->operator_enable_switch_on == 0u) {
            s_enable_switch_wait_ticks++;
            if (s_enable_switch_wait_ticks >= V37_ENABLE_SWITCH_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(7u);
                v37_record_hit(7u, (int32_t)s_enable_switch_wait_ticks);
            }
        } else {
            s_enable_switch_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_PROBE_BATTERY_LOW_MEASURE_INHIBIT) != 0) {
        if (io->probe_measure_request != 0u && io->probe_battery_low != 0u) {
            s_probe_battery_wait_ticks++;
            if (s_probe_battery_wait_ticks >= V37_PROBE_BATTERY_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(8u);
                v37_record_hit(8u, (int32_t)s_probe_battery_wait_ticks);
            }
        } else {
            s_probe_battery_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_ABSOLUTE_ENCODER_BATTERY_WARN_TRACE) != 0) {
        if (io->abs_encoder_battery_low_mask != 0u) {
            s_absenc_warn_wait_ticks++;
            if (s_absenc_warn_wait_ticks >= V37_ABSENC_WARN_TIMEOUT_TICKS) {
                uint32_t trace = NC_FEATURE_BACKLOG_V37_TRACE_BASE + 9u;
                g_ncFeatureBacklogV37Status.last_axis_mask = io->abs_encoder_battery_low_mask;
                g_ncFeatureBacklogV37Status.last_trace_event_code = trace;
                g_ncFeatureBacklogV37Status.last_fault_mask |= V37_BIT(9u);
                v37_record_hit(9u, (int32_t)io->abs_encoder_battery_low_mask);
            }
        } else {
            s_absenc_warn_wait_ticks = 0u;
        }
    }
}

int Api_RunNcFeatureBacklogV37SelfCheck(void)
{
    NC_FEATURE_BACKLOG_V37_BLOCK block;
    NC_FEATURE_BACKLOG_V37_RT_IO io;
    uint32_t i;

    NcFeatureBacklogV37_Init();
    Api_EnableAllNcFeatureBacklogV37Features();

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V37_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(3700 + (int32_t)i);
    }
    block.feed_override_percent = V37_DEFAULT_FEED_OVERRIDE_PERCENT;
    NcFeatureBacklogV37_ApplyBlockTs(&block);

    memset(&io, 0, sizeof(io));
    io.spindle_chiller_on = 1u;
    io.spindle_chiller_flow_ok = 0u;
    io.spindle_temp_c_x10 = V37_SPINDLE_TEMP_HOLD_C_X10;
    io.servo_ready_mask = 0x0000000Fu;
    io.servo_monitored_mask = 0x0000000Fu;
    io.servo_fan_ok_mask = 0x00000007u;
    io.door_lock_command = 1u;
    io.door_lock_ch1 = 1u;
    io.door_lock_ch2 = 0u;
    io.tool_magazine_home_request = 1u;
    io.tool_magazine_home_sensor = 0u;
    io.hydraulic_ready_request = 1u;
    io.hydraulic_pump_on = 1u;
    io.hydraulic_pressure_ok = 0u;
    io.work_air_seat_check_request = 1u;
    io.work_air_blow_on = 1u;
    io.work_air_seat_ok = 0u;
    io.chip_flush_pump_on = 1u;
    io.chip_flush_pressure_ok = 0u;
    io.operator_panel_active = 1u;
    io.operator_enable_required = 1u;
    io.operator_enable_switch_on = 0u;
    io.probe_measure_request = 1u;
    io.probe_battery_low = 1u;
    io.abs_encoder_battery_low_mask = 0x00000005u;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV37_RunRtTick(&io);
    }

    if (g_ncFeatureBacklogV37Status.implemented_count != NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V37_ERR_RANGE;
    }
    if (g_ncFeatureBacklogV37Status.configured_count != NC_FEATURE_BACKLOG_V37_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V37_ERR_RANGE;
    }
    if (g_ncFeatureBacklogV37Status.ts_apply_count == 0u ||
        g_ncFeatureBacklogV37Status.rt_tick_count == 0u ||
        g_ncFeatureBacklogV37Status.hold_count == 0u ||
        g_ncFeatureBacklogV37Status.interlock_count == 0u ||
        g_ncFeatureBacklogV37Status.gate_count == 0u ||
        g_ncFeatureBacklogV37Status.slowdown_count == 0u ||
        g_ncFeatureBacklogV37Status.trace_count == 0u ||
        g_ncFeatureBacklogV37Status.retry_count == 0u ||
        g_ncFeatureBacklogV37Status.inhibit_count == 0u) {
        return NC_FEATURE_BACKLOG_V37_ERR_RANGE;
    }
    return NC_FEATURE_BACKLOG_V37_OK;
}

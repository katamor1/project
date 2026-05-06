/* ibm-bob/samples/base/source/src/nc_feature_backlog_aux_safety.c */
/* Implements auxiliary safety backlog monitors and interlocks. */
/* This exists so v35 safety backlog checks stay isolated from the core backlog table. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_feature_backlog_aux_safety.h, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_clang.bat */
#include "nc_feature_backlog_aux_safety.h"

#include <string.h>

#define V35_BIT(index_) (1u << (uint32_t)(index_))
#define V35_DEFAULT_FEED_OVERRIDE_PERCENT (100)
#define V35_MIN_FEED_OVERRIDE_PERCENT     (5)
#define V35_SPINDLE_ORIENT_TIMEOUT_TICKS  (5u)
#define V35_ATC_HOME_TIMEOUT_TICKS        (4u)
#define V35_WORK_SEAT_TIMEOUT_TICKS       (3u)
#define V35_COVER_LOCK_TIMEOUT_TICKS      (5u)
#define V35_SPINDLE_TEMP_WARN_C_X10       (600)
#define V35_SPINDLE_TEMP_HOLD_C_X10       (700)

typedef struct V35_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V35_POLICY policy;
    uint32_t category;
    const char *name;
} V35_FEATURE_DEF;

static const V35_FEATURE_DEF k_v35_defs[NC_FEATURE_BACKLOG_V35_FEATURE_COUNT] = {
    { NC_FEATURE_V35_SPINDLE_ORIENT_DONE_WATCHDOG, NC_FEATURE_BACKLOG_V35_POLICY_HOLD,      9u, "spindle orientation done watchdog" },
    { NC_FEATURE_V35_ATC_ARM_HOME_MISMATCH_GUARD,  NC_FEATURE_BACKLOG_V35_POLICY_INTERLOCK, 9u, "ATC arm home mismatch guard" },
    { NC_FEATURE_V35_CLAMP_UNCLAMP_DOUBLE_CONFIRM, NC_FEATURE_BACKLOG_V35_POLICY_INTERLOCK, 9u, "clamp/unclamp double confirmation" },
    { NC_FEATURE_V35_WORK_SEATING_AIR_BLOW_CONFIRM,NC_FEATURE_BACKLOG_V35_POLICY_GATE,      9u, "work seating air-blow confirmation" },
    { NC_FEATURE_V35_CHIP_CONVEYOR_OVERLOAD_HOLD,  NC_FEATURE_BACKLOG_V35_POLICY_HOLD,      9u, "chip conveyor overload hold" },
    { NC_FEATURE_V35_COOLANT_PUMP_DRY_RUN_PROTECT, NC_FEATURE_BACKLOG_V35_POLICY_HOLD,      9u, "coolant pump dry-run protection" },
    { NC_FEATURE_V35_SPINDLE_COOLANT_TEMP_STAGED_SLOWDOWN, NC_FEATURE_BACKLOG_V35_POLICY_SLOWDOWN, 9u, "spindle coolant temperature staged slowdown" },
    { NC_FEATURE_V35_SERVO_AMP_READY_LOSS_HOLD,    NC_FEATURE_BACKLOG_V35_POLICY_HOLD,      9u, "servo amp READY loss hold" },
    { NC_FEATURE_V35_ESTOP_CHAIN_DISCREPANCY_TRACE,NC_FEATURE_BACKLOG_V35_POLICY_TRACE,     9u, "emergency-stop chain discrepancy trace" },
    { NC_FEATURE_V35_PROTECTIVE_COVER_LOCK_TIMEOUT,NC_FEATURE_BACKLOG_V35_POLICY_GATE,      9u, "protective cover lock timeout" }
};

NC_FEATURE_BACKLOG_V35_STATUS g_ncFeatureBacklogV35Status;

static uint32_t s_orient_wait_ticks;
static uint32_t s_atc_home_wait_ticks;
static uint32_t s_work_seat_wait_ticks;
static uint32_t s_cover_lock_wait_ticks;

static uint32_t v35_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V35_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V35_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V35_FEATURE_BASE_ID;
}

static void v35_record_hit(uint32_t index, int32_t value)
{
    const V35_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) {
        return;
    }

    def = &k_v35_defs[index];
    g_ncFeatureBacklogV35Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV35Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV35Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV35Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV35Status.cat_mask |= V35_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V35_POLICY_MONITOR:
        g_ncFeatureBacklogV35Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V35_POLICY_GATE:
        g_ncFeatureBacklogV35Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V35_POLICY_HOLD:
        g_ncFeatureBacklogV35Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V35_POLICY_INTERLOCK:
        g_ncFeatureBacklogV35Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V35_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV35Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V35_POLICY_TRACE:
        g_ncFeatureBacklogV35Status.trace_count++;
        break;
    default:
        g_ncFeatureBacklogV35Status.warning_count++;
        break;
    }
}

static int32_t v35_clamp_feed_override(int32_t value)
{
    if (value < V35_MIN_FEED_OVERRIDE_PERCENT) {
        return V35_MIN_FEED_OVERRIDE_PERCENT;
    }
    if (value > V35_DEFAULT_FEED_OVERRIDE_PERCENT) {
        return V35_DEFAULT_FEED_OVERRIDE_PERCENT;
    }
    return value;
}

void NcFeatureBacklogV35_Init(void)
{
    uint32_t i;

    memset(&g_ncFeatureBacklogV35Status, 0, sizeof(g_ncFeatureBacklogV35Status));
    g_ncFeatureBacklogV35Status.implemented_count = NC_FEATURE_BACKLOG_V35_FEATURE_COUNT;
    g_ncFeatureBacklogV35Status.configured_count = 0u;
    g_ncFeatureBacklogV35Status.last_feed_override_percent = V35_DEFAULT_FEED_OVERRIDE_PERCENT;

    for (i = 0u; i < NC_FEATURE_BACKLOG_V35_FEATURE_COUNT; ++i) {
        g_ncFeatureBacklogV35Status.per_feature_last_value[i] = 0;
    }

    s_orient_wait_ticks = 0u;
    s_atc_home_wait_ticks = 0u;
    s_work_seat_wait_ticks = 0u;
    s_cover_lock_wait_ticks = 0u;
}

int NcFeatureBacklogV35_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v35_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) {
        return 0;
    }
    return ((g_ncFeatureBacklogV35Status.enabled_mask & V35_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV35Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v35_index_from_feature_id(feature_id);
    uint32_t mask;

    if (index >= NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V35_ERR_RANGE;
    }

    mask = V35_BIT(index);
    if (enabled != 0u) {
        if ((g_ncFeatureBacklogV35Status.enabled_mask & mask) == 0u) {
            g_ncFeatureBacklogV35Status.configured_count++;
        }
        g_ncFeatureBacklogV35Status.enabled_mask |= mask;
    } else {
        if ((g_ncFeatureBacklogV35Status.enabled_mask & mask) != 0u &&
            g_ncFeatureBacklogV35Status.configured_count > 0u) {
            g_ncFeatureBacklogV35Status.configured_count--;
        }
        g_ncFeatureBacklogV35Status.enabled_mask &= ~mask;
    }

    return NC_FEATURE_BACKLOG_V35_OK;
}

void Api_EnableAllNcFeatureBacklogV35Features(void)
{
    g_ncFeatureBacklogV35Status.enabled_mask = (1u << NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) - 1u;
    g_ncFeatureBacklogV35Status.configured_count = NC_FEATURE_BACKLOG_V35_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV35Status(NC_FEATURE_BACKLOG_V35_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V35_ERR_NULL;
    }
    *out_status = g_ncFeatureBacklogV35Status;
    return NC_FEATURE_BACKLOG_V35_OK;
}

const char *NcFeatureBacklogV35_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v35_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) {
        return "unknown";
    }
    return k_v35_defs[index].name;
}

void NcFeatureBacklogV35_ApplyBlockTs(NC_FEATURE_BACKLOG_V35_BLOCK *block)
{
    uint32_t index;

    if (block == 0) {
        g_ncFeatureBacklogV35Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV35Status.ts_apply_count++;
    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V35_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V35_FEATURE_COUNT; ++index) {
        uint32_t feature_bit = V35_BIT(index);
        uint32_t trace = NC_FEATURE_BACKLOG_V35_TRACE_BASE + index;

        if ((g_ncFeatureBacklogV35Status.enabled_mask & feature_bit) == 0u) {
            continue;
        }

        switch (k_v35_defs[index].feature_id) {
        case NC_FEATURE_V35_SPINDLE_ORIENT_DONE_WATCHDOG:
            block->aux_request_bits |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[0]);
            break;
        case NC_FEATURE_V35_ATC_ARM_HOME_MISMATCH_GUARD:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[1]);
            break;
        case NC_FEATURE_V35_CLAMP_UNCLAMP_DOUBLE_CONFIRM:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[2]);
            break;
        case NC_FEATURE_V35_WORK_SEATING_AIR_BLOW_CONFIRM:
            block->sequence_gate_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[3]);
            break;
        case NC_FEATURE_V35_CHIP_CONVEYOR_OVERLOAD_HOLD:
            block->hold_request |= feature_bit;
            v35_record_hit(index, block->axis_target[4]);
            break;
        case NC_FEATURE_V35_COOLANT_PUMP_DRY_RUN_PROTECT:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[5]);
            break;
        case NC_FEATURE_V35_SPINDLE_COOLANT_TEMP_STAGED_SLOWDOWN:
            block->feed_override_percent = v35_clamp_feed_override(block->feed_override_percent - 5);
            v35_record_hit(index, block->feed_override_percent);
            break;
        case NC_FEATURE_V35_SERVO_AMP_READY_LOSS_HOLD:
            block->hold_request |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[6]);
            break;
        case NC_FEATURE_V35_ESTOP_CHAIN_DISCREPANCY_TRACE:
            block->trace_event_code = trace;
            g_ncFeatureBacklogV35Status.last_trace_event_code = trace;
            v35_record_hit(index, (int32_t)trace);
            break;
        case NC_FEATURE_V35_PROTECTIVE_COVER_LOCK_TIMEOUT:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v35_record_hit(index, block->axis_target[7]);
            break;
        default:
            g_ncFeatureBacklogV35Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV35Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV35Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV35_RunRtTick(const NC_FEATURE_BACKLOG_V35_RT_IO *io)
{
    if (io == 0) {
        g_ncFeatureBacklogV35Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV35Status.rt_tick_count++;

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_SPINDLE_ORIENT_DONE_WATCHDOG) != 0) {
        if (io->spindle_orient_request != 0u && io->spindle_orient_done == 0u) {
            s_orient_wait_ticks++;
            if (s_orient_wait_ticks >= V35_SPINDLE_ORIENT_TIMEOUT_TICKS) {
                v35_record_hit(0u, (int32_t)s_orient_wait_ticks);
            }
        } else {
            s_orient_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_ATC_ARM_HOME_MISMATCH_GUARD) != 0) {
        if (io->atc_arm_home_request != 0u && io->atc_arm_home_sensor == 0u) {
            s_atc_home_wait_ticks++;
            if (s_atc_home_wait_ticks >= V35_ATC_HOME_TIMEOUT_TICKS) {
                v35_record_hit(1u, (int32_t)s_atc_home_wait_ticks);
            }
        } else {
            s_atc_home_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_CLAMP_UNCLAMP_DOUBLE_CONFIRM) != 0) {
        if ((io->clamp_command != 0u && io->unclamp_command != 0u) ||
            (io->clamp_command != 0u && io->clamp_confirm_sensor == 0u) ||
            (io->unclamp_command != 0u && io->unclamp_confirm_sensor == 0u)) {
            v35_record_hit(2u, (int32_t)(io->clamp_command | (io->unclamp_command << 1u)));
        }
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_WORK_SEATING_AIR_BLOW_CONFIRM) != 0) {
        if (io->work_seat_check_request != 0u && io->work_seated_sensor == 0u && io->air_pressure_ok != 0u) {
            s_work_seat_wait_ticks++;
            if (s_work_seat_wait_ticks >= V35_WORK_SEAT_TIMEOUT_TICKS) {
                v35_record_hit(3u, (int32_t)s_work_seat_wait_ticks);
            }
        } else {
            s_work_seat_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_CHIP_CONVEYOR_OVERLOAD_HOLD) != 0 &&
        io->chip_conveyor_overload != 0u) {
        v35_record_hit(4u, 1);
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_COOLANT_PUMP_DRY_RUN_PROTECT) != 0 &&
        io->coolant_pump_on != 0u && io->coolant_flow_ok == 0u) {
        v35_record_hit(5u, 1);
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_SPINDLE_COOLANT_TEMP_STAGED_SLOWDOWN) != 0 &&
        io->spindle_coolant_temp_c_x10 >= V35_SPINDLE_TEMP_WARN_C_X10) {
        v35_record_hit(6u, io->spindle_coolant_temp_c_x10);
        if (io->spindle_coolant_temp_c_x10 >= V35_SPINDLE_TEMP_HOLD_C_X10) {
            g_ncFeatureBacklogV35Status.hold_count++;
        }
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_SERVO_AMP_READY_LOSS_HOLD) != 0 &&
        (io->servo_amp_ready_mask & io->servo_amp_expected_mask) != io->servo_amp_expected_mask) {
        v35_record_hit(7u, (int32_t)(io->servo_amp_expected_mask ^ io->servo_amp_ready_mask));
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_ESTOP_CHAIN_DISCREPANCY_TRACE) != 0 &&
        io->emergency_stop_input != io->safety_relay_feedback) {
        uint32_t trace = NC_FEATURE_BACKLOG_V35_TRACE_BASE + 8u;
        g_ncFeatureBacklogV35Status.last_trace_event_code = trace;
        v35_record_hit(8u, (int32_t)trace);
    }

    if (NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_V35_PROTECTIVE_COVER_LOCK_TIMEOUT) != 0) {
        if (io->protective_cover_lock_request != 0u && io->protective_cover_locked == 0u) {
            s_cover_lock_wait_ticks++;
            if (s_cover_lock_wait_ticks >= V35_COVER_LOCK_TIMEOUT_TICKS) {
                v35_record_hit(9u, (int32_t)s_cover_lock_wait_ticks);
            }
        } else {
            s_cover_lock_wait_ticks = 0u;
        }
    }
}

int Api_RunNcFeatureBacklogV35SelfCheck(void)
{
    NC_FEATURE_BACKLOG_V35_BLOCK block;
    NC_FEATURE_BACKLOG_V35_RT_IO io;
    uint32_t i;

    NcFeatureBacklogV35_Init();
    Api_EnableAllNcFeatureBacklogV35Features();

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V35_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(2700 + (int32_t)i);
    }
    block.feed_override_percent = V35_DEFAULT_FEED_OVERRIDE_PERCENT;
    NcFeatureBacklogV35_ApplyBlockTs(&block);

    memset(&io, 0, sizeof(io));
    io.spindle_orient_request = 1u;
    io.spindle_orient_done = 0u;
    io.atc_arm_home_request = 1u;
    io.atc_arm_home_sensor = 0u;
    io.clamp_command = 1u;
    io.unclamp_command = 1u;
    io.clamp_confirm_sensor = 0u;
    io.work_seat_check_request = 1u;
    io.work_seated_sensor = 0u;
    io.air_pressure_ok = 1u;
    io.chip_conveyor_overload = 1u;
    io.coolant_pump_on = 1u;
    io.coolant_flow_ok = 0u;
    io.spindle_coolant_temp_c_x10 = V35_SPINDLE_TEMP_HOLD_C_X10;
    io.servo_amp_expected_mask = 0x0000000fu;
    io.servo_amp_ready_mask = 0x00000007u;
    io.emergency_stop_input = 1u;
    io.safety_relay_feedback = 0u;
    io.protective_cover_lock_request = 1u;
    io.protective_cover_locked = 0u;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV35_RunRtTick(&io);
    }

    if (g_ncFeatureBacklogV35Status.implemented_count != NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V35_ERR_RANGE;
    }
    if (g_ncFeatureBacklogV35Status.configured_count != NC_FEATURE_BACKLOG_V35_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V35_ERR_RANGE;
    }
    if (g_ncFeatureBacklogV35Status.ts_apply_count == 0u ||
        g_ncFeatureBacklogV35Status.rt_tick_count == 0u ||
        g_ncFeatureBacklogV35Status.hold_count == 0u ||
        g_ncFeatureBacklogV35Status.interlock_count == 0u ||
        g_ncFeatureBacklogV35Status.trace_count == 0u) {
        return NC_FEATURE_BACKLOG_V35_ERR_RANGE;
    }
    return NC_FEATURE_BACKLOG_V35_OK;
}

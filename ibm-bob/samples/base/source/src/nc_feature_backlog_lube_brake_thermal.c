/* ibm-bob/samples/base/source/src/nc_feature_backlog_lube_brake_thermal.c */
/* Implements lubrication, brake, and thermal backlog checks. */
/* This exists so v36 machine-readiness checks stay in a focused module. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_feature_backlog_lube_brake_thermal.h, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_clang.bat */
#include "nc_feature_backlog_lube_brake_thermal.h"

#include <string.h>

#define V36_BIT(index_) (1u << (uint32_t)(index_))
#define V36_DEFAULT_FEED_OVERRIDE_PERCENT (100)
#define V36_MIN_FEED_OVERRIDE_PERCENT     (5)
#define V36_LUBE_PRESSURE_TIMEOUT_TICKS   (3u)
#define V36_BRAKE_RELEASE_TIMEOUT_TICKS   (4u)
#define V36_CHUCK_PRESSURE_TIMEOUT_TICKS  (3u)
#define V36_PALLET_TIMEOUT_TICKS          (5u)
#define V36_THERMAL_READY_TIMEOUT_TICKS   (4u)
#define V36_THERMAL_WARN_DELTA_C_X10      (80)
#define V36_THERMAL_HOLD_DELTA_C_X10      (120)
#define V36_MAX_THERMAL_COMP_UM           (50)

typedef struct V36_FEATURE_DEF_TAG {
    uint32_t feature_id;
    NC_FEATURE_BACKLOG_V36_POLICY policy;
    uint32_t category;
    const char *name;
} V36_FEATURE_DEF;

static const V36_FEATURE_DEF k_v36_defs[NC_FEATURE_BACKLOG_V36_FEATURE_COUNT] = {
    { NC_FEATURE_V36_LUBE_PRESSURE_LOW_STAGED_HOLD,       NC_FEATURE_BACKLOG_V36_POLICY_HOLD,      9u, "lubrication pressure low staged hold" },
    { NC_FEATURE_V36_AXIS_BRAKE_RELEASE_FEEDBACK_GUARD,   NC_FEATURE_BACKLOG_V36_POLICY_INTERLOCK, 9u, "axis brake release feedback guard" },
    { NC_FEATURE_V36_CHUCK_CLAMP_PRESSURE_CONFIRM,        NC_FEATURE_BACKLOG_V36_POLICY_GATE,      9u, "chuck clamp pressure confirmation" },
    { NC_FEATURE_V36_PALLET_CLAMP_UNCLAMP_TIMEOUT,        NC_FEATURE_BACKLOG_V36_POLICY_INTERLOCK, 9u, "pallet clamp/unclamp timeout monitor" },
    { NC_FEATURE_V36_THERMAL_DRIFT_COMP_READY_GATE,       NC_FEATURE_BACKLOG_V36_POLICY_SLOWDOWN,  9u, "thermal drift compensation ready gate" }
};

NC_FEATURE_BACKLOG_V36_STATUS g_ncFeatureBacklogV36Status;

static uint32_t s_lube_wait_ticks;
static uint32_t s_brake_release_wait_ticks;
static uint32_t s_chuck_pressure_wait_ticks;
static uint32_t s_pallet_wait_ticks;
static uint32_t s_thermal_ready_wait_ticks;

static uint32_t v36_index_from_feature_id(uint32_t feature_id)
{
    if (feature_id < NC_FEATURE_BACKLOG_V36_FEATURE_BASE_ID) {
        return NC_FEATURE_BACKLOG_V36_FEATURE_COUNT;
    }
    return feature_id - NC_FEATURE_BACKLOG_V36_FEATURE_BASE_ID;
}

static void v36_record_hit(uint32_t index, int32_t value)
{
    const V36_FEATURE_DEF *def;

    if (index >= NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) {
        return;
    }

    def = &k_v36_defs[index];
    g_ncFeatureBacklogV36Status.per_feature_hits[index]++;
    g_ncFeatureBacklogV36Status.per_feature_last_value[index] = value;
    g_ncFeatureBacklogV36Status.last_feature_id = def->feature_id;
    g_ncFeatureBacklogV36Status.last_policy = (uint32_t)def->policy;
    g_ncFeatureBacklogV36Status.cat_mask |= V36_BIT(def->category);

    switch (def->policy) {
    case NC_FEATURE_BACKLOG_V36_POLICY_MONITOR:
        g_ncFeatureBacklogV36Status.monitor_count++;
        break;
    case NC_FEATURE_BACKLOG_V36_POLICY_GATE:
        g_ncFeatureBacklogV36Status.gate_count++;
        break;
    case NC_FEATURE_BACKLOG_V36_POLICY_HOLD:
        g_ncFeatureBacklogV36Status.hold_count++;
        break;
    case NC_FEATURE_BACKLOG_V36_POLICY_INTERLOCK:
        g_ncFeatureBacklogV36Status.interlock_count++;
        break;
    case NC_FEATURE_BACKLOG_V36_POLICY_SLOWDOWN:
        g_ncFeatureBacklogV36Status.slowdown_count++;
        break;
    case NC_FEATURE_BACKLOG_V36_POLICY_TRACE:
        g_ncFeatureBacklogV36Status.trace_count++;
        break;
    default:
        g_ncFeatureBacklogV36Status.warning_count++;
        break;
    }
}

static int32_t v36_clamp_feed_override(int32_t value)
{
    if (value < V36_MIN_FEED_OVERRIDE_PERCENT) {
        return V36_MIN_FEED_OVERRIDE_PERCENT;
    }
    if (value > V36_DEFAULT_FEED_OVERRIDE_PERCENT) {
        return V36_DEFAULT_FEED_OVERRIDE_PERCENT;
    }
    return value;
}

static int32_t v36_clamp_thermal_comp(int32_t value)
{
    if (value > V36_MAX_THERMAL_COMP_UM) {
        return V36_MAX_THERMAL_COMP_UM;
    }
    if (value < -V36_MAX_THERMAL_COMP_UM) {
        return -V36_MAX_THERMAL_COMP_UM;
    }
    return value;
}

void NcFeatureBacklogV36_Init(void)
{
    uint32_t i;

    memset(&g_ncFeatureBacklogV36Status, 0, sizeof(g_ncFeatureBacklogV36Status));
    g_ncFeatureBacklogV36Status.implemented_count = NC_FEATURE_BACKLOG_V36_FEATURE_COUNT;
    g_ncFeatureBacklogV36Status.configured_count = 0u;
    g_ncFeatureBacklogV36Status.last_feed_override_percent = V36_DEFAULT_FEED_OVERRIDE_PERCENT;

    for (i = 0u; i < NC_FEATURE_BACKLOG_V36_FEATURE_COUNT; ++i) {
        g_ncFeatureBacklogV36Status.per_feature_last_value[i] = 0;
    }

    s_lube_wait_ticks = 0u;
    s_brake_release_wait_ticks = 0u;
    s_chuck_pressure_wait_ticks = 0u;
    s_pallet_wait_ticks = 0u;
    s_thermal_ready_wait_ticks = 0u;
}

int NcFeatureBacklogV36_IsFeatureEnabled(uint32_t feature_id)
{
    uint32_t index = v36_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) {
        return 0;
    }
    return ((g_ncFeatureBacklogV36Status.enabled_mask & V36_BIT(index)) != 0u) ? 1 : 0;
}

int Api_SetNcFeatureBacklogV36Feature(uint32_t feature_id, uint32_t enabled)
{
    uint32_t index = v36_index_from_feature_id(feature_id);
    uint32_t mask;

    if (index >= NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V36_ERR_RANGE;
    }

    mask = V36_BIT(index);
    if (enabled != 0u) {
        if ((g_ncFeatureBacklogV36Status.enabled_mask & mask) == 0u) {
            g_ncFeatureBacklogV36Status.configured_count++;
        }
        g_ncFeatureBacklogV36Status.enabled_mask |= mask;
    } else {
        if ((g_ncFeatureBacklogV36Status.enabled_mask & mask) != 0u &&
            g_ncFeatureBacklogV36Status.configured_count > 0u) {
            g_ncFeatureBacklogV36Status.configured_count--;
        }
        g_ncFeatureBacklogV36Status.enabled_mask &= ~mask;
    }

    return NC_FEATURE_BACKLOG_V36_OK;
}

void Api_EnableAllNcFeatureBacklogV36Features(void)
{
    g_ncFeatureBacklogV36Status.enabled_mask = (1u << NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) - 1u;
    g_ncFeatureBacklogV36Status.configured_count = NC_FEATURE_BACKLOG_V36_FEATURE_COUNT;
}

int Api_GetNcFeatureBacklogV36Status(NC_FEATURE_BACKLOG_V36_STATUS *out_status)
{
    if (out_status == 0) {
        return NC_FEATURE_BACKLOG_V36_ERR_NULL;
    }
    *out_status = g_ncFeatureBacklogV36Status;
    return NC_FEATURE_BACKLOG_V36_OK;
}

const char *NcFeatureBacklogV36_GetFeatureName(uint32_t feature_id)
{
    uint32_t index = v36_index_from_feature_id(feature_id);
    if (index >= NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) {
        return "unknown";
    }
    return k_v36_defs[index].name;
}

void NcFeatureBacklogV36_ApplyBlockTs(NC_FEATURE_BACKLOG_V36_BLOCK *block)
{
    uint32_t index;

    if (block == 0) {
        g_ncFeatureBacklogV36Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV36Status.ts_apply_count++;
    if (block->feed_override_percent <= 0) {
        block->feed_override_percent = V36_DEFAULT_FEED_OVERRIDE_PERCENT;
    }

    for (index = 0u; index < NC_FEATURE_BACKLOG_V36_FEATURE_COUNT; ++index) {
        uint32_t feature_bit = V36_BIT(index);
        uint32_t trace = NC_FEATURE_BACKLOG_V36_TRACE_BASE + index;

        if ((g_ncFeatureBacklogV36Status.enabled_mask & feature_bit) == 0u) {
            continue;
        }

        switch (k_v36_defs[index].feature_id) {
        case NC_FEATURE_V36_LUBE_PRESSURE_LOW_STAGED_HOLD:
            block->diagnostics_mask |= feature_bit;
            block->hold_request |= feature_bit;
            v36_record_hit(index, block->axis_target[0]);
            break;
        case NC_FEATURE_V36_AXIS_BRAKE_RELEASE_FEEDBACK_GUARD:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v36_record_hit(index, block->axis_target[1]);
            break;
        case NC_FEATURE_V36_CHUCK_CLAMP_PRESSURE_CONFIRM:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            v36_record_hit(index, block->axis_target[2]);
            break;
        case NC_FEATURE_V36_PALLET_CLAMP_UNCLAMP_TIMEOUT:
            block->sequence_gate_mask |= feature_bit;
            block->interlock_mask |= feature_bit;
            v36_record_hit(index, block->axis_target[3]);
            break;
        case NC_FEATURE_V36_THERMAL_DRIFT_COMP_READY_GATE:
            block->sequence_gate_mask |= feature_bit;
            block->diagnostics_mask |= feature_bit;
            block->feed_override_percent = v36_clamp_feed_override(block->feed_override_percent - 3);
            block->thermal_comp_offset_um[0] = v36_clamp_thermal_comp(block->thermal_comp_offset_um[0]);
            block->axis_target[0] += block->thermal_comp_offset_um[0];
            block->trace_event_code = trace;
            g_ncFeatureBacklogV36Status.last_trace_event_code = trace;
            v36_record_hit(index, block->feed_override_percent);
            break;
        default:
            g_ncFeatureBacklogV36Status.warning_count++;
            break;
        }
    }

    g_ncFeatureBacklogV36Status.last_feed_override_percent = block->feed_override_percent;
    g_ncFeatureBacklogV36Status.last_x = block->axis_target[0];
}

void NcFeatureBacklogV36_RunRtTick(const NC_FEATURE_BACKLOG_V36_RT_IO *io)
{
    if (io == 0) {
        g_ncFeatureBacklogV36Status.warning_count++;
        return;
    }

    g_ncFeatureBacklogV36Status.rt_tick_count++;

    if (NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_V36_LUBE_PRESSURE_LOW_STAGED_HOLD) != 0) {
        if (io->lube_pump_on != 0u && (io->lube_pressure_ok == 0u || io->lube_level_ok == 0u)) {
            s_lube_wait_ticks++;
            if (s_lube_wait_ticks >= V36_LUBE_PRESSURE_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV36Status.last_fault_mask |= V36_BIT(0u);
                v36_record_hit(0u, (int32_t)s_lube_wait_ticks);
            }
        } else {
            s_lube_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_V36_AXIS_BRAKE_RELEASE_FEEDBACK_GUARD) != 0) {
        uint32_t expected = io->axis_brake_release_command_mask & io->axis_brake_expected_mask;
        uint32_t missing = expected & ~io->axis_brake_released_feedback_mask;
        if (missing != 0u) {
            s_brake_release_wait_ticks++;
            if (s_brake_release_wait_ticks >= V36_BRAKE_RELEASE_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV36Status.last_fault_mask |= missing;
                v36_record_hit(1u, (int32_t)missing);
            }
        } else {
            s_brake_release_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_V36_CHUCK_CLAMP_PRESSURE_CONFIRM) != 0) {
        if ((io->chuck_clamp_command != 0u && io->chuck_pressure_ok == 0u) ||
            (io->chuck_clamp_command != 0u && io->chuck_unclamp_command != 0u)) {
            s_chuck_pressure_wait_ticks++;
            if (s_chuck_pressure_wait_ticks >= V36_CHUCK_PRESSURE_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV36Status.last_fault_mask |= V36_BIT(2u);
                v36_record_hit(2u, (int32_t)s_chuck_pressure_wait_ticks);
            }
        } else {
            s_chuck_pressure_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_V36_PALLET_CLAMP_UNCLAMP_TIMEOUT) != 0) {
        if ((io->pallet_clamp_request != 0u && io->pallet_clamped_sensor == 0u) ||
            (io->pallet_unclamp_request != 0u && io->pallet_unclamped_sensor == 0u) ||
            (io->pallet_clamp_request != 0u && io->pallet_unclamp_request != 0u)) {
            s_pallet_wait_ticks++;
            if (s_pallet_wait_ticks >= V36_PALLET_TIMEOUT_TICKS) {
                g_ncFeatureBacklogV36Status.last_fault_mask |= V36_BIT(3u);
                v36_record_hit(3u, (int32_t)s_pallet_wait_ticks);
            }
        } else {
            s_pallet_wait_ticks = 0u;
        }
    }

    if (NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_V36_THERMAL_DRIFT_COMP_READY_GATE) != 0) {
        if (io->thermal_comp_request != 0u &&
            (io->thermal_comp_ready == 0u || io->thermal_delta_c_x10 >= V36_THERMAL_WARN_DELTA_C_X10)) {
            s_thermal_ready_wait_ticks++;
            if (s_thermal_ready_wait_ticks >= V36_THERMAL_READY_TIMEOUT_TICKS) {
                uint32_t trace = NC_FEATURE_BACKLOG_V36_TRACE_BASE + 4u;
                g_ncFeatureBacklogV36Status.last_trace_event_code = trace;
                g_ncFeatureBacklogV36Status.last_fault_mask |= V36_BIT(4u);
                v36_record_hit(4u, io->thermal_delta_c_x10);
                if (io->thermal_delta_c_x10 >= V36_THERMAL_HOLD_DELTA_C_X10) {
                    g_ncFeatureBacklogV36Status.hold_count++;
                }
            }
        } else {
            s_thermal_ready_wait_ticks = 0u;
        }
    }
}

int Api_RunNcFeatureBacklogV36SelfCheck(void)
{
    NC_FEATURE_BACKLOG_V36_BLOCK block;
    NC_FEATURE_BACKLOG_V36_RT_IO io;
    uint32_t i;

    NcFeatureBacklogV36_Init();
    Api_EnableAllNcFeatureBacklogV36Features();

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V36_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(2800 + (int32_t)i);
    }
    block.feed_override_percent = V36_DEFAULT_FEED_OVERRIDE_PERCENT;
    block.thermal_comp_offset_um[0] = 12;
    NcFeatureBacklogV36_ApplyBlockTs(&block);

    memset(&io, 0, sizeof(io));
    io.lube_pump_on = 1u;
    io.lube_pressure_ok = 0u;
    io.lube_level_ok = 1u;
    io.axis_brake_release_command_mask = 0x00000003u;
    io.axis_brake_expected_mask = 0x00000003u;
    io.axis_brake_released_feedback_mask = 0x00000001u;
    io.chuck_clamp_command = 1u;
    io.chuck_pressure_ok = 0u;
    io.pallet_clamp_request = 1u;
    io.pallet_clamped_sensor = 0u;
    io.thermal_comp_request = 1u;
    io.thermal_comp_ready = 0u;
    io.thermal_delta_c_x10 = V36_THERMAL_HOLD_DELTA_C_X10;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV36_RunRtTick(&io);
    }

    if (g_ncFeatureBacklogV36Status.implemented_count != NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V36_ERR_RANGE;
    }
    if (g_ncFeatureBacklogV36Status.configured_count != NC_FEATURE_BACKLOG_V36_FEATURE_COUNT) {
        return NC_FEATURE_BACKLOG_V36_ERR_RANGE;
    }
    if (g_ncFeatureBacklogV36Status.ts_apply_count == 0u ||
        g_ncFeatureBacklogV36Status.rt_tick_count == 0u ||
        g_ncFeatureBacklogV36Status.hold_count == 0u ||
        g_ncFeatureBacklogV36Status.interlock_count == 0u ||
        g_ncFeatureBacklogV36Status.gate_count == 0u ||
        g_ncFeatureBacklogV36Status.slowdown_count == 0u) {
        return NC_FEATURE_BACKLOG_V36_ERR_RANGE;
    }
    return NC_FEATURE_BACKLOG_V36_OK;
}

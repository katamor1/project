#include "nc_feature_backlog_aux_safety.h"

#include <stdio.h>
#include <string.h>

static int expect_u32(const char *name, uint32_t actual, uint32_t expected)
{
    if (actual != expected) {
        printf("FAIL %s actual=%u expected=%u\n", name, actual, expected);
        return 1;
    }
    return 0;
}

static int expect_nonzero(const char *name, uint32_t actual)
{
    if (actual == 0u) {
        printf("FAIL %s actual=0\n", name);
        return 1;
    }
    return 0;
}

int main(void)
{
    NC_FEATURE_BACKLOG_V35_BLOCK block;
    NC_FEATURE_BACKLOG_V35_RT_IO io;
    NC_FEATURE_BACKLOG_V35_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV35_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV35Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV35Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV35Feature(NC_FEATURE_V35_SPINDLE_ORIENT_DONE_WATCHDOG, 1u) != 0) {
        printf("FAIL enable feature 270\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV35Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV35Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV35Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V35_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(1200 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV35_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV35Status.ts_apply_count);
    failures += expect_nonzero("aux_request_bits", block.aux_request_bits);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate_mask", block.sequence_gate_mask);
    failures += expect_nonzero("trace_event_code", block.trace_event_code);

    memset(&io, 0, sizeof(io));
    io.spindle_orient_request = 1u;
    io.atc_arm_home_request = 1u;
    io.clamp_command = 1u;
    io.unclamp_command = 1u;
    io.work_seat_check_request = 1u;
    io.air_pressure_ok = 1u;
    io.chip_conveyor_overload = 1u;
    io.coolant_pump_on = 1u;
    io.coolant_flow_ok = 0u;
    io.spindle_coolant_temp_c_x10 = 720;
    io.servo_amp_expected_mask = 0x0fu;
    io.servo_amp_ready_mask = 0x07u;
    io.emergency_stop_input = 1u;
    io.safety_relay_feedback = 0u;
    io.protective_cover_lock_request = 1u;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV35_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV35Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV35Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV35Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV35Status.gate_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV35Status.trace_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV35Status.slowdown_count);

    if (Api_GetNcFeatureBacklogV35Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV35Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV35SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV35Status(&status);
    printf("implementation_backlog_v35 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x last_x=%d\n",
           status.implemented_count,
           status.configured_count,
           status.ts_apply_count,
           status.rt_tick_count,
           status.hold_count,
           status.interlock_count,
           status.gate_count,
           status.slowdown_count,
           status.trace_count,
           status.cat_mask,
           status.last_feature_id,
           status.last_policy,
           status.last_trace_event_code,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_aux_safety\n");
    return 0;
}

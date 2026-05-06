#include "nc_feature_backlog_lube_brake_thermal.h"

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

static int expect_less_than(const char *name, int32_t actual, int32_t threshold)
{
    if (actual >= threshold) {
        printf("FAIL %s actual=%d threshold=%d\n", name, actual, threshold);
        return 1;
    }
    return 0;
}

int main(void)
{
    NC_FEATURE_BACKLOG_V36_BLOCK block;
    NC_FEATURE_BACKLOG_V36_RT_IO io;
    NC_FEATURE_BACKLOG_V36_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV36_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV36Status.implemented_count, 5u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV36Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV36Feature(NC_FEATURE_V36_LUBE_PRESSURE_LOW_STAGED_HOLD, 1u) != 0) {
        printf("FAIL enable feature 280\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV36Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV36Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV36Status.configured_count, 5u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V36_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(1300 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    block.thermal_comp_offset_um[0] = 15;
    NcFeatureBacklogV36_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV36Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate_mask", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics_mask", block.diagnostics_mask);
    failures += expect_nonzero("trace_event_code", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);
    if (block.axis_target[0] != 1315) {
        printf("FAIL thermal compensated x actual=%d expected=1315\n", block.axis_target[0]);
        failures++;
    }

    memset(&io, 0, sizeof(io));
    io.lube_pump_on = 1u;
    io.lube_pressure_ok = 0u;
    io.lube_level_ok = 1u;
    io.axis_brake_release_command_mask = 0x03u;
    io.axis_brake_expected_mask = 0x03u;
    io.axis_brake_released_feedback_mask = 0x01u;
    io.chuck_clamp_command = 1u;
    io.chuck_pressure_ok = 0u;
    io.pallet_clamp_request = 1u;
    io.pallet_clamped_sensor = 0u;
    io.thermal_comp_request = 1u;
    io.thermal_comp_ready = 0u;
    io.thermal_delta_c_x10 = 130;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV36_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV36Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV36Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV36Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV36Status.gate_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV36Status.slowdown_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV36Status.last_fault_mask);

    if (Api_GetNcFeatureBacklogV36Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV36Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV36SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV36Status(&status);
    printf("implementation_backlog_v36 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x last_x=%d\n",
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
           status.last_fault_mask,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_lube_brake_thermal\n");
    return 0;
}

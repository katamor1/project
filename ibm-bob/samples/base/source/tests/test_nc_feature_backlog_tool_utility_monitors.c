#include "nc_feature_backlog_tool_utility_monitors.h"

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
    NC_FEATURE_BACKLOG_V39_BLOCK block;
    NC_FEATURE_BACKLOG_V39_RT_IO io;
    NC_FEATURE_BACKLOG_V39_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV39_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV39Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV39Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV39Feature(NC_FEATURE_V39_SPINDLE_BEARING_TEMP_SLOWDOWN, 1u) != 0) {
        printf("FAIL enable feature 305\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV39Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    if (NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_V39_SPINDLE_BEARING_TEMP_SLOWDOWN) == 0) {
        printf("FAIL enabled feature was not visible\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV39Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV39Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V39_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(3900 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV39_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV39Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate_mask", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics_mask", block.diagnostics_mask);
    failures += expect_nonzero("retry_request", block.retry_request_mask);
    failures += expect_nonzero("inhibit_mask", block.inhibit_mask);
    failures += expect_nonzero("trace_event_code", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);
    if (block.axis_target[0] != 3900) {
        printf("FAIL axis target unexpectedly changed actual=%d expected=3900\n", block.axis_target[0]);
        failures++;
    }

    memset(&io, 0, sizeof(io));
    io.spindle_bearing_temp_check = 1u;
    io.spindle_bearing_temp_c = 86;
    io.linear_scale_monitor_mask = 0x0Fu;
    io.linear_scale_dirty_mask = 0x05u;
    io.tool_clamp_check_request = 1u;
    io.tool_clamp_drawbar_ok = 0u;
    io.rotary_table_motion_request = 1u;
    io.rotary_table_clamp_confirmed = 0u;
    io.coolant_filter_monitor = 1u;
    io.coolant_filter_diff_pressure_high = 1u;
    io.dust_collector_required = 1u;
    io.dust_collector_airflow_ok = 0u;
    io.spindle_override_lock_zone = 1u;
    io.spindle_override_change_request = 1u;
    io.atc_tool_number_check_request = 1u;
    io.atc_expected_tool_no = 12u;
    io.atc_actual_tool_no = 14u;
    io.work_probe_measure_request = 1u;
    io.work_probe_ready = 0u;
    io.ups_monitor_enabled = 1u;
    io.ups_voltage_low = 1u;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV39_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV39Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV39Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV39Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV39Status.gate_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV39Status.trace_count);
    failures += expect_nonzero("retry_count", g_ncFeatureBacklogV39Status.retry_count);
    failures += expect_nonzero("inhibit_count", g_ncFeatureBacklogV39Status.inhibit_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV39Status.slowdown_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV39Status.last_fault_mask);
    failures += expect_nonzero("axis_mask", g_ncFeatureBacklogV39Status.last_axis_mask);
    failures += expect_u32("tool_expected", g_ncFeatureBacklogV39Status.last_tool_expected, 12u);
    failures += expect_u32("tool_actual", g_ncFeatureBacklogV39Status.last_tool_actual, 14u);

    if (Api_GetNcFeatureBacklogV39Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV39Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV39SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV39Status(&status);
    printf("implementation_backlog_v39 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u retry=%u inhibit=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x axis=0x%08x tool=%u/%u spindle_temp=%d last_x=%d\n",
           status.implemented_count,
           status.configured_count,
           status.ts_apply_count,
           status.rt_tick_count,
           status.hold_count,
           status.interlock_count,
           status.gate_count,
           status.slowdown_count,
           status.trace_count,
           status.retry_count,
           status.inhibit_count,
           status.cat_mask,
           status.last_feature_id,
           status.last_policy,
           status.last_trace_event_code,
           status.last_fault_mask,
           status.last_axis_mask,
           status.last_tool_expected,
           status.last_tool_actual,
           status.last_spindle_bearing_temp_c,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_tool_utility_monitors\n");
    return 0;
}

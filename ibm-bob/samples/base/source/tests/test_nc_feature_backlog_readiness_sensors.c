#include "nc_feature_backlog_readiness_sensors.h"

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
    NC_FEATURE_BACKLOG_V37_BLOCK block;
    NC_FEATURE_BACKLOG_V37_RT_IO io;
    NC_FEATURE_BACKLOG_V37_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV37_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV37Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV37Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV37Feature(NC_FEATURE_V37_SPINDLE_CHILLER_FLOW_LOW_SLOWDOWN, 1u) != 0) {
        printf("FAIL enable feature 285\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV37Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    if (NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_V37_SPINDLE_CHILLER_FLOW_LOW_SLOWDOWN) == 0) {
        printf("FAIL enabled feature was not visible\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV37Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV37Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V37_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(1700 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV37_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV37Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate_mask", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics_mask", block.diagnostics_mask);
    failures += expect_nonzero("retry_request", block.retry_request_mask);
    failures += expect_nonzero("inhibit_mask", block.inhibit_mask);
    failures += expect_nonzero("trace_event_code", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);
    if (block.axis_target[0] != 1700) {
        printf("FAIL axis target unexpectedly changed actual=%d expected=1700\n", block.axis_target[0]);
        failures++;
    }

    memset(&io, 0, sizeof(io));
    io.spindle_chiller_on = 1u;
    io.spindle_chiller_flow_ok = 0u;
    io.spindle_temp_c_x10 = 710;
    io.servo_ready_mask = 0x0Fu;
    io.servo_monitored_mask = 0x0Fu;
    io.servo_fan_ok_mask = 0x07u;
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
    io.abs_encoder_battery_low_mask = 0x05u;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV37_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV37Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV37Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV37Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV37Status.gate_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV37Status.slowdown_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV37Status.trace_count);
    failures += expect_nonzero("retry_count", g_ncFeatureBacklogV37Status.retry_count);
    failures += expect_nonzero("inhibit_count", g_ncFeatureBacklogV37Status.inhibit_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV37Status.last_fault_mask);
    failures += expect_nonzero("axis_mask", g_ncFeatureBacklogV37Status.last_axis_mask);

    if (Api_GetNcFeatureBacklogV37Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV37Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV37SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV37Status(&status);
    printf("implementation_backlog_v37 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u retry=%u inhibit=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x axis=0x%08x last_x=%d\n",
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
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_readiness_sensors\n");
    return 0;
}

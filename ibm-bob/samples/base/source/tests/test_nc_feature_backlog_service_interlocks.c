#include "nc_feature_backlog_service_interlocks.h"

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
    NC_FEATURE_BACKLOG_V41_BLOCK block;
    NC_FEATURE_BACKLOG_V41_RT_IO io;
    NC_FEATURE_BACKLOG_V41_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV41_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV41Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV41Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV41Feature(NC_FEATURE_V41_SERVO_POSITION_ERROR_SLOWDOWN, 1u) != 0) {
        printf("FAIL enable feature 325\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV41Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    if (NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_V41_SERVO_POSITION_ERROR_SLOWDOWN) == 0) {
        printf("FAIL enabled feature was not visible\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV41Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV41Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V41_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(4100 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV41_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV41Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics", block.diagnostics_mask);
    failures += expect_nonzero("retry", block.retry_request_mask);
    failures += expect_nonzero("inhibit", block.inhibit_mask);
    failures += expect_nonzero("trace", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);

    memset(&io, 0, sizeof(io));
    io.servo_position_error_monitor = 1u;
    io.servo_position_error_axis_mask = 0x00000005u;
    io.servo_position_error_um = 175;
    io.spindle_load_monitor = 1u;
    io.spindle_load_percent = 148;
    io.coolant_flow_sensor_monitor = 1u;
    io.coolant_flow_sensor_open = 1u;
    io.fixture_clamp_pressure_required = 1u;
    io.fixture_clamp_pressure_kpa = 360;
    io.robot_door_closed_required = 1u;
    io.robot_door_closed = 0u;
    io.exhaust_fan_ready_required = 1u;
    io.exhaust_fan_ready = 0u;
    io.tool_wash_nozzle_monitor = 1u;
    io.tool_wash_flow_percent = 20;
    io.axis_home_sensor_check_request = 1u;
    io.axis_home_sensor_a_mask = 0x00000003u;
    io.axis_home_sensor_b_mask = 0x00000001u;
    io.nc_option_license_check_request = 1u;
    io.nc_option_required_mask = 0x0000000Fu;
    io.nc_option_licensed_mask = 0x00000007u;
    io.maintenance_mode_monitor = 1u;
    io.maintenance_mode_active = 1u;
    io.maintenance_key_present = 0u;

    for (i = 0u; i < 8u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV41_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV41Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV41Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV41Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV41Status.gate_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV41Status.trace_count);
    failures += expect_nonzero("retry_count", g_ncFeatureBacklogV41Status.retry_count);
    failures += expect_nonzero("inhibit_count", g_ncFeatureBacklogV41Status.inhibit_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV41Status.slowdown_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV41Status.last_fault_mask);
    failures += expect_u32("axis_mask", g_ncFeatureBacklogV41Status.last_axis_mask, 0x00000002u);
    failures += expect_u32("license_missing", g_ncFeatureBacklogV41Status.last_license_missing_mask, 0x00000008u);

    if (Api_GetNcFeatureBacklogV41Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV41Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV41SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV41Status(&status);
    printf("implementation_backlog_v41 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u retry=%u inhibit=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x axis=0x%08x servo_error=%d spindle_load=%d fixture_kpa=%d wash_flow=%d license_missing=0x%08x last_x=%d\n",
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
           status.last_servo_error_um,
           status.last_spindle_load_percent,
           status.last_fixture_pressure_kpa,
           status.last_tool_wash_flow_percent,
           status.last_license_missing_mask,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_service_interlocks\n");
    return 0;
}

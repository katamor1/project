#include "nc_feature_backlog_process_safety.h"

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
    NC_FEATURE_BACKLOG_V40_BLOCK block;
    NC_FEATURE_BACKLOG_V40_RT_IO io;
    NC_FEATURE_BACKLOG_V40_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV40_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV40Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV40Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV40Feature(NC_FEATURE_V40_LASER_AREA_SCANNER_INTRUSION_HOLD, 1u) != 0) {
        printf("FAIL enable feature 315\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV40Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    if (NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_V40_LASER_AREA_SCANNER_INTRUSION_HOLD) == 0) {
        printf("FAIL enabled feature was not visible\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV40Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV40Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V40_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(4000 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV40_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV40Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate_mask", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics_mask", block.diagnostics_mask);
    failures += expect_nonzero("retry_request", block.retry_request_mask);
    failures += expect_nonzero("inhibit_mask", block.inhibit_mask);
    failures += expect_nonzero("retract_request", block.retract_request_mask);
    failures += expect_nonzero("trace_event_code", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);
    if (block.axis_target[0] != 4000) {
        printf("FAIL axis target unexpectedly changed actual=%d expected=4000\n", block.axis_target[0]);
        failures++;
    }

    memset(&io, 0, sizeof(io));
    io.laser_area_scanner_required = 1u;
    io.laser_area_intrusion_detected = 1u;
    io.spindle_vibration_monitor = 1u;
    io.spindle_vibration_um = 76;
    io.feed_axis_lube_required_mask = 0x0Fu;
    io.feed_axis_lube_ready_mask = 0x0Au;
    io.nc_program_signature_check_request = 1u;
    io.nc_program_signature_ok = 0u;
    io.chuck_dual_sensor_check_request = 1u;
    io.chuck_sensor_a_closed = 1u;
    io.chuck_sensor_b_closed = 0u;
    io.tool_break_monitor_request = 1u;
    io.tool_break_detected = 1u;
    io.tool_length_measure_check_request = 1u;
    io.tool_length_last_um = 1200;
    io.tool_length_current_um = 1385;
    io.coolant_temperature_monitor = 1u;
    io.coolant_temperature_c = 51;
    io.pallet_robot_ready_required = 1u;
    io.pallet_robot_ready = 0u;
    io.safety_plc_heartbeat_monitor = 1u;
    io.safety_plc_heartbeat_counter = 77u;

    for (i = 0u; i < 8u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV40_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV40Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV40Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV40Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV40Status.gate_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV40Status.trace_count);
    failures += expect_nonzero("retry_count", g_ncFeatureBacklogV40Status.retry_count);
    failures += expect_nonzero("inhibit_count", g_ncFeatureBacklogV40Status.inhibit_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV40Status.slowdown_count);
    failures += expect_nonzero("retract_count", g_ncFeatureBacklogV40Status.retract_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV40Status.last_fault_mask);
    failures += expect_nonzero("axis_mask", g_ncFeatureBacklogV40Status.last_axis_mask);
    failures += expect_u32("sensor_a", g_ncFeatureBacklogV40Status.last_sensor_a, 1u);
    failures += expect_u32("sensor_b", g_ncFeatureBacklogV40Status.last_sensor_b, 0u);

    if (Api_GetNcFeatureBacklogV40Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV40Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV40SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV40Status(&status);
    printf("implementation_backlog_v40 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u retry=%u inhibit=%u retract=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x axis=0x%08x chuck=%u/%u vibration=%d tool_delta=%d coolant=%d heartbeat=%u last_x=%d\n",
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
           status.retract_count,
           status.cat_mask,
           status.last_feature_id,
           status.last_policy,
           status.last_trace_event_code,
           status.last_fault_mask,
           status.last_axis_mask,
           status.last_sensor_a,
           status.last_sensor_b,
           status.last_spindle_vibration_um,
           status.last_tool_length_delta_um,
           status.last_coolant_temperature_c,
           status.last_heartbeat_counter,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_process_safety\n");
    return 0;
}

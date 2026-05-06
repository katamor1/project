#include "nc_feature_backlog_loader_environment.h"

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
    NC_FEATURE_BACKLOG_V42_BLOCK block;
    NC_FEATURE_BACKLOG_V42_RT_IO io;
    NC_FEATURE_BACKLOG_V42_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV42_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV42Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV42Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV42Feature(NC_FEATURE_V42_SPINDLE_AIR_PURGE_PRESSURE_GATE, 1u) != 0) {
        printf("FAIL enable feature 335\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV42Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    if (NcFeatureBacklogV42_IsFeatureEnabled(NC_FEATURE_V42_SPINDLE_AIR_PURGE_PRESSURE_GATE) == 0) {
        printf("FAIL enabled feature was not visible\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV42Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV42Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V42_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(4200 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV42_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV42Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics", block.diagnostics_mask);
    failures += expect_nonzero("retry", block.retry_request_mask);
    failures += expect_nonzero("inhibit", block.inhibit_mask);
    failures += expect_nonzero("trace", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);

    memset(&io, 0, sizeof(io));
    io.spindle_air_purge_required = 1u;
    io.spindle_air_pressure_kpa = 350;
    io.axis_limit_switch_monitor = 1u;
    io.commanded_axis_mask = 0x00000005u;
    io.limit_switch_active_mask = 0x00000004u;
    io.oil_mist_monitor = 1u;
    io.oil_mist_pressure_kpa = 60;
    io.loader_chuck_check_request = 1u;
    io.loader_chuck_open_signal = 1u;
    io.loader_chuck_closed_signal = 1u;
    io.auto_door_command_active = 1u;
    io.auto_door_done = 0u;
    io.table_seat_check_request = 1u;
    io.table_seat_a_mask = 0x00000003u;
    io.table_seat_b_mask = 0x00000001u;
    io.door_unlock_request = 1u;
    io.spindle_speed_rpm = 2400;
    io.tool_rfid_required = 1u;
    io.tool_rfid_read_ok = 0u;
    io.machining_room_light_monitor = 1u;
    io.machining_room_light_ok = 0u;
    io.servo_temperature_monitor = 1u;
    io.servo_temperature_axis_mask = 0x00000002u;
    io.servo_temperature_c = 86;

    for (i = 0u; i < 8u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV42_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV42Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV42Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV42Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV42Status.gate_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV42Status.trace_count);
    failures += expect_nonzero("retry_count", g_ncFeatureBacklogV42Status.retry_count);
    failures += expect_nonzero("inhibit_count", g_ncFeatureBacklogV42Status.inhibit_count);
    failures += expect_nonzero("slowdown_count", g_ncFeatureBacklogV42Status.slowdown_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV42Status.last_fault_mask);
    failures += expect_u32("axis_mask", g_ncFeatureBacklogV42Status.last_axis_mask, 0x00000002u);
    failures += expect_u32("table_mismatch", g_ncFeatureBacklogV42Status.last_table_mismatch_mask, 0x00000002u);
    failures += expect_u32("tool_rfid", g_ncFeatureBacklogV42Status.last_tool_rfid_ok, 0u);

    if (Api_GetNcFeatureBacklogV42Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV42Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV42SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV42Status(&status);
    printf("implementation_backlog_v42 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u slowdown=%u trace=%u retry=%u inhibit=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x axis=0x%08x pressure=%d spindle_rpm=%d temp=%d table_mismatch=0x%08x tool_rfid=%u last_x=%d\n",
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
           status.last_pressure_kpa,
           status.last_spindle_speed_rpm,
           status.last_temperature_c,
           status.last_table_mismatch_mask,
           status.last_tool_rfid_ok,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_loader_environment\n");
    return 0;
}

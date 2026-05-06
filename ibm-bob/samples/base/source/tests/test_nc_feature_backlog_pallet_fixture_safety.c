#include "nc_feature_backlog_pallet_fixture_safety.h"

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
    NC_FEATURE_BACKLOG_V38_BLOCK block;
    NC_FEATURE_BACKLOG_V38_RT_IO io;
    NC_FEATURE_BACKLOG_V38_STATUS status;
    uint32_t i;
    int failures = 0;

    NcFeatureBacklogV38_Init();
    failures += expect_u32("implemented", g_ncFeatureBacklogV38Status.implemented_count, 10u);
    failures += expect_u32("configured_init", g_ncFeatureBacklogV38Status.configured_count, 0u);

    if (Api_SetNcFeatureBacklogV38Feature(NC_FEATURE_V38_MACHINE_THERMAL_STABLE_READY_INHIBIT, 1u) != 0) {
        printf("FAIL enable feature 295\n");
        failures++;
    }
    if (Api_SetNcFeatureBacklogV38Feature(999u, 1u) == 0) {
        printf("FAIL out-of-range feature accepted\n");
        failures++;
    }
    if (NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_V38_MACHINE_THERMAL_STABLE_READY_INHIBIT) == 0) {
        printf("FAIL enabled feature was not visible\n");
        failures++;
    }
    Api_EnableAllNcFeatureBacklogV38Features();
    failures += expect_u32("configured_all", g_ncFeatureBacklogV38Status.configured_count, 10u);

    memset(&block, 0, sizeof(block));
    for (i = 0u; i < NC_FEATURE_BACKLOG_V38_AXIS_COUNT; ++i) {
        block.axis_target[i] = (int32_t)(1800 + (int32_t)i);
    }
    block.feed_override_percent = 100;
    NcFeatureBacklogV38_ApplyBlockTs(&block);

    failures += expect_nonzero("ts_apply", g_ncFeatureBacklogV38Status.ts_apply_count);
    failures += expect_nonzero("hold_request", block.hold_request);
    failures += expect_nonzero("interlock_mask", block.interlock_mask);
    failures += expect_nonzero("sequence_gate_mask", block.sequence_gate_mask);
    failures += expect_nonzero("diagnostics_mask", block.diagnostics_mask);
    failures += expect_nonzero("retry_request", block.retry_request_mask);
    failures += expect_nonzero("inhibit_mask", block.inhibit_mask);
    failures += expect_nonzero("trace_event_code", block.trace_event_code);
    failures += expect_less_than("feed_override", block.feed_override_percent, 100);
    if (block.axis_target[0] != 1800) {
        printf("FAIL axis target unexpectedly changed actual=%d expected=1800\n", block.axis_target[0]);
        failures++;
    }

    memset(&io, 0, sizeof(io));
    io.thermal_stable_request = 1u;
    io.thermal_temp_ready = 0u;
    io.thermal_drift_um = 190;
    io.hydraulic_filter_monitored = 1u;
    io.hydraulic_filter_clogged = 1u;
    io.atc_pot_index_request = 1u;
    io.atc_pot_position_ok = 0u;
    io.pallet_id_check_request = 1u;
    io.pallet_present = 1u;
    io.expected_pallet_id = 17u;
    io.actual_pallet_id = 23u;
    io.z_axis_enabled = 1u;
    io.z_counterbalance_pressure_ok = 0u;
    io.servo_abs_check_mask = 0x0Fu;
    io.servo_abs_mismatch_mask = 0x05u;
    io.spindle_tool_unclamp_request = 1u;
    io.spindle_tool_unclamp_sensor = 0u;
    io.coolant_request = 1u;
    io.coolant_tank_level_low = 1u;
    io.mist_collector_required = 1u;
    io.mist_collector_ready = 0u;
    io.safety_mat_enabled = 1u;
    io.safety_mat_intrusion = 1u;

    for (i = 0u; i < 6u; ++i) {
        io.cycle_counter = i;
        NcFeatureBacklogV38_RunRtTick(&io);
    }

    failures += expect_nonzero("rt_tick", g_ncFeatureBacklogV38Status.rt_tick_count);
    failures += expect_nonzero("hold_count", g_ncFeatureBacklogV38Status.hold_count);
    failures += expect_nonzero("interlock_count", g_ncFeatureBacklogV38Status.interlock_count);
    failures += expect_nonzero("gate_count", g_ncFeatureBacklogV38Status.gate_count);
    failures += expect_nonzero("trace_count", g_ncFeatureBacklogV38Status.trace_count);
    failures += expect_nonzero("retry_count", g_ncFeatureBacklogV38Status.retry_count);
    failures += expect_nonzero("inhibit_count", g_ncFeatureBacklogV38Status.inhibit_count);
    failures += expect_nonzero("fault_mask", g_ncFeatureBacklogV38Status.last_fault_mask);
    failures += expect_nonzero("axis_mask", g_ncFeatureBacklogV38Status.last_axis_mask);
    failures += expect_u32("pallet_expected", g_ncFeatureBacklogV38Status.last_pallet_expected, 17u);
    failures += expect_u32("pallet_actual", g_ncFeatureBacklogV38Status.last_pallet_actual, 23u);

    if (Api_GetNcFeatureBacklogV38Status(&status) != 0) {
        printf("FAIL status get\n");
        failures++;
    }
    if (Api_GetNcFeatureBacklogV38Status(0) == 0) {
        printf("FAIL null status accepted\n");
        failures++;
    }
    if (Api_RunNcFeatureBacklogV38SelfCheck() != 0) {
        printf("FAIL selfcheck\n");
        failures++;
    }

    Api_GetNcFeatureBacklogV38Status(&status);
    printf("implementation_backlog_v38 implemented=%u configured=%u ts=%u rt=%u hold=%u interlock=%u gate=%u trace=%u retry=%u inhibit=%u cat_mask=0x%08x last_id=%u last_policy=%u trace=0x%04x fault=0x%08x axis=0x%08x pallet=%u/%u thermal=%d last_x=%d\n",
           status.implemented_count,
           status.configured_count,
           status.ts_apply_count,
           status.rt_tick_count,
           status.hold_count,
           status.interlock_count,
           status.gate_count,
           status.trace_count,
           status.retry_count,
           status.inhibit_count,
           status.cat_mask,
           status.last_feature_id,
           status.last_policy,
           status.last_trace_event_code,
           status.last_fault_mask,
           status.last_axis_mask,
           status.last_pallet_expected,
           status.last_pallet_actual,
           status.last_thermal_drift_um,
           status.last_x);

    if (failures != 0) {
        printf("FAILED failures=%d\n", failures);
        return 1;
    }

    printf("PASS nc_feature_backlog_pallet_fixture_safety\n");
    return 0;
}

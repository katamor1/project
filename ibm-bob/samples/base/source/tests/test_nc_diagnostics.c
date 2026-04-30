#include "test_common.h"
#include "stub_external_io.h"
#include "nc_diagnostics.h"
#include "nc_program.h"

static int test_axis_load_normal_input_keeps_safe_state(void)
{
    Test_ResetSystem();
    Test_SetAnalog(1U, 0);
    Test_SetAnalog(2U, 100);

    NcDiagnostics_UpdateAxisLoadRt();

    TEST_ASSERT_EQ_UINT(0U, g_ncAxisLoadStatus.warning_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncAxisLoadStatus.limit_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncAxisLoadStatus.critical_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncAxisLoadStatus.decel_request);
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, g_ncAxisLoadStatus.decel_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.generation);
    return 0;
}

static int test_axis_load_negative_input_is_abs_and_generates_critical_failsafe(void)
{
    Test_ResetSystem();
    g_machineCtx.cycle_count = 123U;
    Test_SetAnalog(1U, -4000); /* filtered = 1000, above critical threshold */

    NcDiagnostics_UpdateAxisLoadRt();

    TEST_ASSERT_EQ_INT(4000, g_ncAxisLoadStatus.current_load[0]);
    TEST_ASSERT_TRUE(g_ncAxisLoadStatus.filtered_load[0] >= NC_AXIS_LOAD_CRITICAL_THRESHOLD);
    TEST_ASSERT_EQ_UINT(0x1U, g_ncAxisLoadStatus.warning_mask);
    TEST_ASSERT_EQ_UINT(0x1U, g_ncAxisLoadStatus.limit_mask);
    TEST_ASSERT_EQ_UINT(0x1U, g_ncAxisLoadStatus.critical_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.decel_request);
    TEST_ASSERT_EQ_UINT(3U, g_ncAxisLoadStatus.staged_decel_level);
    TEST_ASSERT_EQ_UINT(25U, g_ncAxisLoadStatus.decel_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.warning_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.limit_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.critical_events);
    TEST_ASSERT_EQ_UINT(123U, g_ncAxisLoadStatus.last_warning_cycle);
    TEST_ASSERT_TRUE(g_logQueue.tail != g_logQueue.head);
    return 0;
}

static int test_axis_load_event_counter_only_counts_new_mask_entry(void)
{
    Test_ResetSystem();
    Test_SetAnalog(1U, 4000);

    NcDiagnostics_UpdateAxisLoadRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.warning_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.limit_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.critical_events);

    NcDiagnostics_UpdateAxisLoadRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.warning_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.limit_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.critical_events);
    return 0;
}

static int test_tool_life_limit_validation_and_update(void)
{
    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(0U, 10U));
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(NC_TOOL_LIFE_TABLE_SIZE, 10U));
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(2U, 0U));

    TEST_ASSERT_EQ_INT(0, NcDiagnostics_SetToolLifeLimit(2U, 3U));
    TEST_ASSERT_EQ_UINT(3U, g_ncToolLifeStatus.tool_life_limit[2]);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolLifeStatus.generation);
    return 0;
}

static int test_tool_life_warning_and_expired_masks_by_m06_tool_block(void)
{
    NC_EXEC_BLOCK block;
    uint32_t toolBit = (1UL << 2U);

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(0, NcDiagnostics_SetToolLifeLimit(2U, 3U));
    block = Test_MakeToolBlock(100U, 2U, 6U, NC_AUX_FLAG_TOOL);

    NcDiagnostics_OnBlockRt(&block);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolLifeStatus.tool_use_count[2]);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolLifeStatus.warning_mask & toolBit);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolLifeStatus.expired_mask & toolBit);

    block.line_no = 101U;
    NcDiagnostics_OnBlockRt(&block);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolLifeStatus.tool_use_count[2]);
    TEST_ASSERT_EQ_UINT(toolBit, g_ncToolLifeStatus.warning_mask & toolBit);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolLifeStatus.warning_events);

    block.line_no = 102U;
    NcDiagnostics_OnBlockRt(&block);
    TEST_ASSERT_EQ_UINT(3U, g_ncToolLifeStatus.tool_use_count[2]);
    TEST_ASSERT_EQ_UINT(toolBit, g_ncToolLifeStatus.expired_mask & toolBit);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolLifeStatus.expired_events);
    TEST_ASSERT_EQ_UINT(102U, g_ncToolLifeStatus.last_update_line_no);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolLifeStatus.active_tool_no);
    return 0;
}

static int test_tool_life_active_tool_counts_motion_but_not_dwell_or_null(void)
{
    NC_EXEC_BLOCK toolBlock;
    NC_EXEC_BLOCK motionBlock;
    NC_EXEC_BLOCK dwellBlock;

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(0, NcDiagnostics_SetToolLifeLimit(3U, 10U));
    toolBlock = Test_MakeToolBlock(10U, 3U, 6U, NC_AUX_FLAG_TOOL);
    motionBlock = Test_MakeMotionBlock(11U, NC_MOTION_LINEAR, 0x1U, 100, 0, 0, 0);
    dwellBlock = Test_MakeMotionBlock(12U, NC_MOTION_DWELL, 0U, 0, 0, 0, 0);

    NcDiagnostics_OnBlockRt(NULL);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolLifeStatus.tool_use_count[3]);

    NcDiagnostics_OnBlockRt(&toolBlock);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolLifeStatus.tool_use_count[3]);

    NcDiagnostics_OnBlockRt(&motionBlock);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolLifeStatus.tool_use_count[3]);

    NcDiagnostics_OnBlockRt(&dwellBlock);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolLifeStatus.tool_use_count[3]);
    return 0;
}

static int test_diagnostic_snapshot_copies_current_shared_state(void)
{
    Test_ResetSystem();
    g_machineCtx.cycle_count = 44U;
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_ALARM;
    g_machineCtx.alarm_code = 9001U;
    g_ncProgramStatus.state = NC_HOLD;
    g_ncProgramStatus.buffered_blocks = 12U;
    g_ncProgramStatus.exec_line_no = 34U;
    g_ncProgramStatus.read_line_no = 56U;
    g_ncAxisLoadStatus.warning_mask = 0x3U;
    g_ncAxisLoadStatus.limit_mask = 0x1U;
    g_ncAxisLoadStatus.staged_decel_level = 2U;
    g_ncAxisLoadStatus.decel_override_percent = 45U;
    g_ncToolLifeStatus.warning_mask = 0x4U;
    g_ncToolLifeStatus.expired_mask = 0x8U;
    g_ncToolLifeStatus.active_tool_no = 3U;
    g_ncInterpStatus.completed_segments = 77U;
    g_ncCycleStatus.completed_cycles = 88U;
    g_ncAuxStatus.last_wait_cycles = 99U;
    g_ioTraceBuffer.frozen = 1U;

    NcDiagnostics_BuildSnapshot();

    TEST_ASSERT_EQ_UINT(44U, g_ncDiagnosticSnapshot.cycle_count);
    TEST_ASSERT_EQ_UINT(RUN_MODE_AUTO, g_ncDiagnosticSnapshot.run_mode);
    TEST_ASSERT_EQ_UINT(MACHINE_ALARM, g_ncDiagnosticSnapshot.machine_state);
    TEST_ASSERT_EQ_UINT(NC_HOLD, g_ncDiagnosticSnapshot.nc_state);
    TEST_ASSERT_EQ_UINT(9001U, g_ncDiagnosticSnapshot.alarm_code);
    TEST_ASSERT_EQ_UINT(12U, g_ncDiagnosticSnapshot.buffered_blocks);
    TEST_ASSERT_EQ_UINT(34U, g_ncDiagnosticSnapshot.exec_line_no);
    TEST_ASSERT_EQ_UINT(56U, g_ncDiagnosticSnapshot.read_line_no);
    TEST_ASSERT_EQ_UINT(0x3U, g_ncDiagnosticSnapshot.axis_warning_mask);
    TEST_ASSERT_EQ_UINT(0x1U, g_ncDiagnosticSnapshot.axis_limit_mask);
    TEST_ASSERT_EQ_UINT(2U, g_ncDiagnosticSnapshot.staged_decel_level);
    TEST_ASSERT_EQ_UINT(45U, g_ncDiagnosticSnapshot.decel_override_percent);
    TEST_ASSERT_EQ_UINT(0x4U, g_ncDiagnosticSnapshot.tool_warning_mask);
    TEST_ASSERT_EQ_UINT(0x8U, g_ncDiagnosticSnapshot.tool_expired_mask);
    TEST_ASSERT_EQ_UINT(3U, g_ncDiagnosticSnapshot.active_tool_no);
    TEST_ASSERT_EQ_UINT(77U, g_ncDiagnosticSnapshot.completed_segments);
    TEST_ASSERT_EQ_UINT(88U, g_ncDiagnosticSnapshot.completed_cycles);
    TEST_ASSERT_EQ_UINT(99U, g_ncDiagnosticSnapshot.aux_wait_cycles);
    TEST_ASSERT_EQ_UINT(1U, g_ncDiagnosticSnapshot.trace_frozen);
    TEST_ASSERT_EQ_UINT(1U, g_ncDiagnosticSnapshot.generation);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_load_normal_input_keeps_safe_state);
    RUN_TEST(test_axis_load_negative_input_is_abs_and_generates_critical_failsafe);
    RUN_TEST(test_axis_load_event_counter_only_counts_new_mask_entry);
    RUN_TEST(test_tool_life_limit_validation_and_update);
    RUN_TEST(test_tool_life_warning_and_expired_masks_by_m06_tool_block);
    RUN_TEST(test_tool_life_active_tool_counts_motion_but_not_dwell_or_null);
    RUN_TEST(test_diagnostic_snapshot_copies_current_shared_state);
    printf("[  PASSED  ] test_nc_diagnostics\n");
    return 0;
}

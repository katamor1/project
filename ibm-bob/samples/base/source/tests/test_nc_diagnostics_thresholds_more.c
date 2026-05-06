#include "test_common.h"
#include "system_shared.h"
#include "nc_diagnostics.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_tool_block(uint32_t lineNo, uint32_t toolNo, uint32_t flags, NC_MOTION_TYPE motion)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.tool_no = toolNo;
    b.aux_flags = flags;
    b.aux_m_code = 6U;
    b.motion_type = motion;
    return b;
}

static int test_axis_load_critical_sets_masks_and_decel_override(void)
{
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 10U;
    g_ioIn.analog[1] = 4000;
    NcDiagnostics_UpdateAxisLoadRt();
    TEST_ASSERT_TRUE((g_ncAxisLoadStatus.warning_mask & 0x01U) != 0U);
    TEST_ASSERT_TRUE((g_ncAxisLoadStatus.limit_mask & 0x01U) != 0U);
    TEST_ASSERT_TRUE((g_ncAxisLoadStatus.critical_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_UINT(3U, g_ncAxisLoadStatus.staged_decel_level);
    TEST_ASSERT_EQ_UINT(25U, g_ncAxisLoadStatus.decel_override_percent);
    return 0;
}

static int test_repeated_axis_load_warning_does_not_recount_events_until_cleared(void)
{
    SystemShared_Initialize();
    g_ioIn.analog[1] = 4000;
    NcDiagnostics_UpdateAxisLoadRt();
    NcDiagnostics_UpdateAxisLoadRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.warning_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.limit_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisLoadStatus.critical_events);
    return 0;
}

static int test_tool_life_limit_validation_and_expiration(void)
{
    NC_EXEC_BLOCK block;
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(0U, 2U));
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(1U, 0U));
    TEST_ASSERT_EQ_INT(0, NcDiagnostics_SetToolLifeLimit(3U, 2U));
    block = make_tool_block(1761U, 3U, NC_AUX_FLAG_TOOL, NC_MOTION_NONE);
    NcDiagnostics_OnBlockRt(&block);
    block = make_tool_block(1762U, 3U, 0U, NC_MOTION_LINEAR);
    NcDiagnostics_OnBlockRt(&block);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolLifeStatus.tool_use_count[3]);
    TEST_ASSERT_TRUE((g_ncToolLifeStatus.warning_mask & (1UL << 3U)) != 0U);
    TEST_ASSERT_TRUE((g_ncToolLifeStatus.expired_mask & (1UL << 3U)) != 0U);
    return 0;
}

static int test_diagnostic_snapshot_copies_current_shared_status(void)
{
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 123U;
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ncProgramStatus.state = NC_RUNNING;
    g_ncProgramStatus.exec_line_no = 77U;
    g_ncAxisLoadStatus.warning_mask = 0x03U;
    g_ncToolLifeStatus.expired_mask = 0x04U;
    g_ioTraceBuffer.frozen = 1U;
    NcDiagnostics_BuildSnapshot();
    TEST_ASSERT_EQ_UINT(123U, g_ncDiagnosticSnapshot.cycle_count);
    TEST_ASSERT_EQ_UINT(RUN_MODE_AUTO, g_ncDiagnosticSnapshot.run_mode);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncDiagnosticSnapshot.nc_state);
    TEST_ASSERT_EQ_UINT(77U, g_ncDiagnosticSnapshot.exec_line_no);
    TEST_ASSERT_EQ_UINT(0x04U, g_ncDiagnosticSnapshot.tool_expired_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncDiagnosticSnapshot.trace_frozen);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_load_critical_sets_masks_and_decel_override);
    RUN_TEST(test_repeated_axis_load_warning_does_not_recount_events_until_cleared);
    RUN_TEST(test_tool_life_limit_validation_and_expiration);
    RUN_TEST(test_diagnostic_snapshot_copies_current_shared_status);
    printf("test_nc_diagnostics_thresholds_more: PASS\n");
    return 0;
}

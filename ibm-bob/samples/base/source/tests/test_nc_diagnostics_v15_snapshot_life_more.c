#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_diagnostics.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static NC_EXEC_BLOCK tool_block(uint32_t lineNo, uint32_t toolNo, uint32_t auxFlags)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.tool_no = toolNo;
    b.aux_flags = auxFlags;
    b.aux_m_code = 6U;
    return b;
}

static int test_axis_load_negative_analog_is_abs_filtered(void)
{
    setup();
    g_ioIn.analog[1] = -NC_AXIS_LOAD_CRITICAL_THRESHOLD * NC_AXIS_LOAD_FILTER_DIV;
    NcDiagnostics_UpdateAxisLoadRt();
    TEST_ASSERT_TRUE((g_ncAxisLoadStatus.critical_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_UINT(3U, g_ncAxisLoadStatus.staged_decel_level);
    TEST_ASSERT_EQ_UINT(25U, g_ncAxisLoadStatus.decel_override_percent);
    return 0;
}

static int test_tool_life_limit_rejects_zero_tool_and_zero_limit(void)
{
    setup();
    uint32_t before = g_ncToolLifeStatus.tool_life_limit[1];
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(0U, 10U));
    TEST_ASSERT_EQ_INT(-1, NcDiagnostics_SetToolLifeLimit(1U, 0U));
    TEST_ASSERT_EQ_UINT(before, g_ncToolLifeStatus.tool_life_limit[1]);
    return 0;
}

static int test_motion_block_increments_active_tool_usage_after_tool_select(void)
{
    NC_EXEC_BLOCK b;
    setup();
    TEST_ASSERT_EQ_INT(0, NcDiagnostics_SetToolLifeLimit(2U, 3U));
    b = tool_block(71U, 2U, NC_AUX_FLAG_TOOL);
    NcDiagnostics_OnBlockRt(&b);
    memset(&b, 0, sizeof(b));
    b.line_no = 72U;
    b.motion_type = NC_MOTION_LINEAR;
    NcDiagnostics_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolLifeStatus.tool_use_count[2]);
    TEST_ASSERT_EQ_UINT(72U, g_ncToolLifeStatus.last_update_line_no);
    return 0;
}

static int test_snapshot_copies_current_diagnostic_fields(void)
{
    setup();
    g_machineCtx.cycle_count = 99U;
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_machineCtx.alarm_code = 123U;
    g_prestartInterlockStatus.mask = 0x55U;
    g_ncProgramStatus.state = NC_HOLD;
    g_ncProgramStatus.buffered_blocks = 8U;
    g_ncAxisLoadStatus.warning_mask = 0x03U;
    g_ncAxisLoadStatus.staged_decel_level = 2U;
    g_ncAxisLoadStatus.decel_override_percent = 45U;
    g_ncToolLifeStatus.active_tool_no = 2U;
    g_ioTraceBuffer.frozen = 1U;
    NcDiagnostics_BuildSnapshot();
    TEST_ASSERT_EQ_UINT(99U, g_ncDiagnosticSnapshot.cycle_count);
    TEST_ASSERT_EQ_UINT(RUN_MODE_AUTO, g_ncDiagnosticSnapshot.run_mode);
    TEST_ASSERT_EQ_UINT(NC_HOLD, g_ncDiagnosticSnapshot.nc_state);
    TEST_ASSERT_EQ_UINT(0x03U, g_ncDiagnosticSnapshot.axis_warning_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncDiagnosticSnapshot.trace_frozen);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_load_negative_analog_is_abs_filtered);
    RUN_TEST(test_tool_life_limit_rejects_zero_tool_and_zero_limit);
    RUN_TEST(test_motion_block_increments_active_tool_usage_after_tool_select);
    RUN_TEST(test_snapshot_copies_current_diagnostic_fields);
    printf("test_nc_diagnostics_v15_snapshot_life_more: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_spindle.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.axis_target[0] = 100000;
    return b;
}

static int test_speed_limit_setter_rejects_invalid_ranges(void)
{
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(1000U, 0U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(2000U, 1000U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(1U, NC_SPINDLE_ABSOLUTE_MAX_RPM + 1U));
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetSpeedLimit(50U, 500U));
    TEST_ASSERT_EQ_UINT(50U, g_ncSpindleStatus.min_rpm);
    TEST_ASSERT_EQ_UINT(500U, g_ncSpindleStatus.max_rpm);
    return 0;
}

static int test_fixed_rpm_start_clamps_to_min_and_stop_clears_output(void)
{
    NC_EXEC_BLOCK b = make_block(1781U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetSpeedLimit(100U, 500U));
    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_M_CODE;
    b.spindle_speed = 20U;
    b.aux_m_code = 3U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(100U, g_ncSpindleStatus.output_rpm);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_CW, g_ncSpindleStatus.direction);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.speed_clamp_events);
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 5U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ioOut.spindle_speed);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.stop_events);
    return 0;
}

static int test_css_mode_estimates_rpm_and_clamps_to_max(void)
{
    NC_EXEC_BLOCK b = make_block(1782U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetSpeedLimit(30U, 1000U));
    b.g_code10 = NC_G_CODE_WHOLE(96);
    b.aux_flags = NC_AUX_FLAG_SPINDLE;
    b.spindle_speed = 50000U;
    b.axis_target[0] = NC_SPINDLE_CSS_MIN_DIAMETER;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.css_mode_active);
    TEST_ASSERT_EQ_UINT(1000U, g_ncSpindleStatus.output_rpm);
    TEST_ASSERT_TRUE(g_ncSpindleStatus.speed_clamp_events > 0U);
    return 0;
}

static int test_orientation_done_and_timeout_paths(void)
{
    NC_EXEC_BLOCK b = make_block(1783U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetOrientWindow(2U));
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 10U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_requested);
    g_machineCtx.cycle_count = 11U;
    g_ioIn.seq_bits = SENSOR_SPINDLE_ORIENT_DONE_BIT;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_completed);

    g_ioIn.seq_bits = 0U;
    g_machineCtx.cycle_count = 20U;
    NcSpindle_OnBlockRt(&b);
    g_machineCtx.cycle_count = 22U;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_timeout_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    return 0;
}

int main(void)
{
    RUN_TEST(test_speed_limit_setter_rejects_invalid_ranges);
    RUN_TEST(test_fixed_rpm_start_clamps_to_min_and_stop_clears_output);
    RUN_TEST(test_css_mode_estimates_rpm_and_clamps_to_max);
    RUN_TEST(test_orientation_done_and_timeout_paths);
    printf("test_nc_spindle_v17_css_orient_more: PASS\n");
    return 0;
}

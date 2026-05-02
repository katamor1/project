#include <string.h>
#include "test_common.h"
#include "nc_spindle.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 31U;
    return b;
}

static int test_speed_limit_rejects_zero_and_absolute_overflow(void)
{
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(0U, 0U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(10U, NC_SPINDLE_ABSOLUTE_MAX_RPM + 1U));
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DEFAULT_MAX_RPM, g_ncSpindleStatus.max_rpm);
    return 0;
}

static int test_fixed_rpm_clamps_to_min_and_max(void)
{
    NC_EXEC_BLOCK b = make_block();
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetSpeedLimit(100U, 1000U));
    b.aux_flags = NC_AUX_FLAG_SPINDLE;
    b.spindle_speed = 10U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(100U, g_ncSpindleStatus.output_rpm);
    b.spindle_speed = 2000U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1000U, g_ncSpindleStatus.output_rpm);
    TEST_ASSERT_TRUE(g_ncSpindleStatus.speed_clamp_events >= 2U);
    return 0;
}

static int test_ccw_then_stop_updates_direction_and_output(void)
{
    NC_EXEC_BLOCK b = make_block();
    SystemShared_Initialize();
    NcSpindle_Reset();
    b.aux_flags = NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_SPINDLE;
    b.aux_m_code = 4U;
    b.spindle_speed = 500U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_CCW, g_ncSpindleStatus.direction);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.running);
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 5U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_STOP, g_ncSpindleStatus.direction);
    TEST_ASSERT_EQ_UINT(0U, g_ioOut.spindle_speed);
    return 0;
}

static int test_orientation_done_and_timeout_paths(void)
{
    NC_EXEC_BLOCK b = make_block();
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetOrientWindow(3U));
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 10U;
    NcSpindle_OnBlockRt(&b);
    g_machineCtx.cycle_count = 12U;
    g_ioIn.seq_bits = SENSOR_SPINDLE_ORIENT_DONE_BIT;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_completed);
    g_ioIn.seq_bits = 0U;
    g_machineCtx.cycle_count = 20U;
    NcSpindle_OnBlockRt(&b);
    g_machineCtx.cycle_count = 23U;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_timeout_events);
    return 0;
}

int main(void)
{
    RUN_TEST(test_speed_limit_rejects_zero_and_absolute_overflow);
    RUN_TEST(test_fixed_rpm_clamps_to_min_and_max);
    RUN_TEST(test_ccw_then_stop_updates_direction_and_output);
    RUN_TEST(test_orientation_done_and_timeout_paths);
    printf("test_nc_spindle_v14_more: PASS\n");
    return 0;
}

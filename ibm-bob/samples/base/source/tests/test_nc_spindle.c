#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_spindle.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcSpindle_Reset();
}

static NC_EXEC_BLOCK make_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.axis_target[0] = 10 * NC_POSITION_SCALE;
    return b;
}

static int test_speed_limit_validation_and_clamp(void)
{
    NC_EXEC_BLOCK b = make_block(51U);
    setup();
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(5000U, 1000U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(0U, NC_SPINDLE_ABSOLUTE_MAX_RPM + 1U));
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetSpeedLimit(100U, 1000U));

    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_M_CODE;
    b.spindle_speed = 5000U;
    b.aux_m_code = 3U;
    NcSpindle_OnBlockRt(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.running);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_CW, g_ncSpindleStatus.direction);
    TEST_ASSERT_EQ_UINT(1000U, g_ncSpindleStatus.output_rpm);
    TEST_ASSERT_EQ_UINT(1000U, g_ioOut.spindle_speed);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.speed_clamp_events);
    return 0;
}

static int test_css_mode_and_stop_m_code(void)
{
    NC_EXEC_BLOCK b = make_block(52U);
    setup();
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(96);
    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_M_CODE;
    b.spindle_speed = 200U;
    b.aux_m_code = 4U;
    NcSpindle_OnParsedBlockTs(&b);
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.css_mode_active);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_CCW, g_ncSpindleStatus.direction);
    TEST_ASSERT_TRUE(g_ncSpindleStatus.output_rpm >= g_ncSpindleStatus.min_rpm);

    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 5U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.running);
    TEST_ASSERT_EQ_UINT(0U, g_ioOut.spindle_speed);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.stop_events);
    return 0;
}

static int test_orientation_done_and_timeout_paths(void)
{
    NC_EXEC_BLOCK b = make_block(53U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetOrientWindow(3U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetOrientWindow(0U));
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 10U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_requested);

    g_machineCtx.cycle_count = 11U;
    g_ioIn.seq_bits = SENSOR_SPINDLE_ORIENT_DONE_BIT;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_completed);

    setup();
    (void)NcSpindle_SetOrientWindow(2U);
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 20U;
    NcSpindle_OnBlockRt(&b);
    g_machineCtx.cycle_count = 22U;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_timeout_events);
    return 0;
}

int main(void)
{
    RUN_TEST(test_speed_limit_validation_and_clamp);
    RUN_TEST(test_css_mode_and_stop_m_code);
    RUN_TEST(test_orientation_done_and_timeout_paths);
    printf("test_nc_spindle: PASS\n");
    return 0;
}

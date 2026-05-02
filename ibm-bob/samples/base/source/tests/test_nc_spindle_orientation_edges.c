#include "test_common.h"
#include "system_shared.h"
#include "nc_spindle.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_spindle_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_AUX_ONLY;
    return b;
}

static int test_speed_limit_validation_and_css_clamp_to_max(void)
{
    NC_EXEC_BLOCK b = make_spindle_block(801U);
    SystemShared_Initialize();
    NcSpindle_Reset();

    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(1000U, 0U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetSpeedLimit(5000U, 1000U));
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetSpeedLimit(100U, 5000U));

    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(96);
    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_M_CODE;
    b.spindle_speed = 1000U;
    b.aux_m_code = 3U;
    b.axis_target[0] = NC_SPINDLE_CSS_MIN_DIAMETER;
    NcSpindle_OnParsedBlockTs(&b);
    NcSpindle_OnBlockRt(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.css_mode_active);
    TEST_ASSERT_EQ_UINT(5000U, g_ncSpindleStatus.output_rpm);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.clamped);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.speed_clamp_events);
    TEST_ASSERT_EQ_UINT(5000U, g_ioOut.spindle_speed);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_CW, g_ncSpindleStatus.direction);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.start_events);
    return 0;
}

static int test_orient_done_clears_request_and_records_completion(void)
{
    NC_EXEC_BLOCK b = make_spindle_block(802U);
    SystemShared_Initialize();
    NcSpindle_Reset();

    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 10U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_requested);

    g_machineCtx.cycle_count = 15U;
    g_ioIn.seq_bits |= SENSOR_SPINDLE_ORIENT_DONE_BIT;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_completed);
    TEST_ASSERT_EQ_UINT(5U, g_ncSpindleStatus.orient_elapsed_ticks);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_SPINDLE_ORIENT_DONE, g_ncEventRing.items[g_ncEventRing.write_index - 1U].code);
    return 0;
}

static int test_orient_timeout_records_timeout_event(void)
{
    NC_EXEC_BLOCK b = make_spindle_block(803U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    TEST_ASSERT_EQ_INT(0, NcSpindle_SetOrientWindow(3U));
    TEST_ASSERT_EQ_INT(-1, NcSpindle_SetOrientWindow(0U));

    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 100U;
    NcSpindle_OnBlockRt(&b);

    g_machineCtx.cycle_count = 103U;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_completed);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_timeout_events);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_SPINDLE_ORIENT_TIMEOUT, g_ncEventRing.items[g_ncEventRing.write_index - 1U].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_speed_limit_validation_and_css_clamp_to_max);
    RUN_TEST(test_orient_done_clears_request_and_records_completion);
    RUN_TEST(test_orient_timeout_records_timeout_event);
    printf("test_nc_spindle_orientation_edges: PASS\n");
    return 0;
}

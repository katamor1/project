#include "test_common.h"
#include "system_shared.h"
#include "nc_spindle.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.axis_target[0] = 100000;
    return b;
}

static int test_parsed_null_is_noop_and_g96_s_word_sets_css_surface(void)
{
    NC_EXEC_BLOCK b = make_block(1850U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    NcSpindle_OnParsedBlockTs(0);
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.css_blocks);
    b.g_code10 = NC_G_CODE_WHOLE(96);
    b.aux_flags = NC_AUX_FLAG_SPINDLE;
    b.spindle_speed = 220U;
    NcSpindle_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.css_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.css_mode_active);
    TEST_ASSERT_EQ_UINT(220U, g_ncSpindleStatus.css_surface_speed);
    return 0;
}

static int test_m04_after_m03_counts_direction_change_and_start_events(void)
{
    NC_EXEC_BLOCK b = make_block(1851U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    b.aux_flags = NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_SPINDLE;
    b.spindle_speed = 500U;
    b.aux_m_code = 3U;
    NcSpindle_OnBlockRt(&b);
    b.aux_m_code = 4U;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_CCW, g_ncSpindleStatus.direction);
    TEST_ASSERT_TRUE(g_ncSpindleStatus.direction_change_events >= 2U);
    TEST_ASSERT_EQ_UINT(2U, g_ncSpindleStatus.start_events);
    return 0;
}

static int test_thread_and_tap_blocks_increment_sync_required_counter(void)
{
    NC_EXEC_BLOCK b = make_block(1852U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    b.motion_type = NC_MOTION_THREAD;
    NcSpindle_OnBlockRt(&b);
    b.motion_type = NC_MOTION_LINEAR;
    b.cycle_flags = NC_CYCLE_FLAG_TAP;
    NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(2U, g_ncSpindleStatus.sync_required_blocks);
    return 0;
}

static int test_service_without_orientation_request_is_noop_then_done_clears_request(void)
{
    NC_EXEC_BLOCK b = make_block(1853U);
    SystemShared_Initialize();
    NcSpindle_Reset();
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    b.aux_flags = NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 19U;
    g_machineCtx.cycle_count = 100U;
    NcSpindle_OnBlockRt(&b);
    g_machineCtx.cycle_count = 101U;
    g_ioIn.seq_bits = SENSOR_SPINDLE_ORIENT_DONE_BIT;
    NcSpindle_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_requested);
    TEST_ASSERT_EQ_UINT(1U, g_ncSpindleStatus.orient_completed);
    TEST_ASSERT_EQ_UINT(0U, g_ncSpindleStatus.orient_timeout_events);
    return 0;
}

int main(void)
{
    RUN_TEST(test_parsed_null_is_noop_and_g96_s_word_sets_css_surface);
    RUN_TEST(test_m04_after_m03_counts_direction_change_and_start_events);
    RUN_TEST(test_thread_and_tap_blocks_increment_sync_required_counter);
    RUN_TEST(test_service_without_orientation_request_is_noop_then_done_clears_request);
    printf("test_nc_spindle_v18_parse_sync_more: PASS\n");
    return 0;
}

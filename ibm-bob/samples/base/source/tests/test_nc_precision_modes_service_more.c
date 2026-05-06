#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_precision.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcPrecision_Reset();
}

static NC_EXEC_BLOCK motion(uint32_t lineNo, int32_t target)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = target;
    b.feed_override_percent = 100U;
    return b;
}

static int test_learning_memory_window_clamps_to_max(void)
{
    setup();
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, NC_PRECISION_LEARNING_GAIN_MAX, (uint8_t)(NC_PRECISION_LEARNING_MEMORY_MAX + 9U)));
    TEST_ASSERT_EQ_UINT(NC_PRECISION_LEARNING_MEMORY_MAX, g_ncPrecisionStatus.learning_memory_window);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.learning_enabled);
    return 0;
}

static int test_g08_p0_clears_preview_and_high_precision_mode(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = motion(21U, 1000);
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    b.p_word = 2U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.preview_control_enabled);
    b.p_word = 0U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.high_precision_contour_active);
    return 0;
}

static int test_g05_p3_sets_high_speed_cycle_without_hpcc_count(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = motion(22U, 2000);
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    b.p_word = 3U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.high_speed_cycle_active);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.hpcc_mode_blocks);
    TEST_ASSERT_EQ_UINT((uint32_t)NC_G_CODE_WHOLE(5), g_ncPrecisionStatus.active_mode_code10);
    return 0;
}

static int test_service_clamps_learning_correction_to_axis_limit(void)
{
    NC_EXEC_BLOCK b;
    setup();
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, NC_PRECISION_LEARNING_GAIN_MAX, 1U));
    b = motion(23U, 1000000);
    g_ioOut.axis_target[0] = 0;
    NcPrecision_OnBlockRt(&b);
    g_ioOut.axis_target[0] = NC_PRECISION_AXIS_TARGET_LIMIT - 1;
    NcPrecision_ServiceRt();
    TEST_ASSERT_EQ_INT(NC_PRECISION_AXIS_TARGET_LIMIT, g_ioOut.axis_target[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_learning_memory_window_clamps_to_max);
    RUN_TEST(test_g08_p0_clears_preview_and_high_precision_mode);
    RUN_TEST(test_g05_p3_sets_high_speed_cycle_without_hpcc_count);
    RUN_TEST(test_service_clamps_learning_correction_to_axis_limit);
    printf("test_nc_precision_modes_service_more: PASS\n");
    return 0;
}

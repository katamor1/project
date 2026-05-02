#include "test_common.h"
#include "system_shared.h"
#include "nc_precision.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_motion(uint32_t line, int32_t target)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = target;
    b.feed_override_percent = 100U;
    return b;
}

static int test_reset_sets_default_precision_parameters(void)
{
    SystemShared_Initialize();
    NcPrecision_Reset();
    TEST_ASSERT_EQ_INT(NC_PRECISION_LEARNING_GAIN_DEFAULT, g_ncPrecisionStatus.learning_gain_percent);
    TEST_ASSERT_EQ_UINT(NC_PRECISION_LEARNING_MEMORY_DEFAULT, g_ncPrecisionStatus.learning_memory_window);
    TEST_ASSERT_EQ_UINT(NC_PRECISION_PREVIEW_LOOKAHEAD_DEFAULT, g_ncPrecisionStatus.preview_lookahead_blocks);
    return 0;
}

static int test_invalid_setters_reject_out_of_range_values(void)
{
    SystemShared_Initialize();
    NcPrecision_Reset();
    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetLearningControl(1U, NC_PRECISION_LEARNING_GAIN_MAX + 1, 1U));
    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetVibrationControl(1U, NC_PRECISION_VIBRATION_FREQ_MAX_HZ + 1U, 10U));
    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetPreviewControl(1U, 0U, 0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.learning_enabled);
    return 0;
}

static int test_parsed_g08_p2_enables_preview_and_high_precision(void)
{
    NC_EXEC_BLOCK b = make_motion(1830U, 10000);
    SystemShared_Initialize();
    NcPrecision_Reset();
    b.g_code10 = NC_G_CODE_WHOLE(8);
    b.p_word = 2U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.high_precision_contour_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.previewed_blocks);
    TEST_ASSERT_EQ_UINT(1830U, g_ncPrecisionStatus.last_preview_line_no);
    return 0;
}

static int test_learning_correction_is_applied_by_service_rt(void)
{
    NC_EXEC_BLOCK b = make_motion(1831U, 1000);
    SystemShared_Initialize();
    NcPrecision_Reset();
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, 100, 1U));
    g_ioOut.axis_target[0] = 0;
    NcPrecision_OnBlockRt(&b);
    TEST_ASSERT_EQ_INT(1000, g_ncPrecisionStatus.last_learning_correction[0]);
    g_ioOut.axis_target[0] = 10;
    NcPrecision_ServiceRt();
    TEST_ASSERT_EQ_INT(1010, g_ioOut.axis_target[0]);
    TEST_ASSERT_TRUE(g_ncPrecisionStatus.rt_service_ticks > 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_reset_sets_default_precision_parameters);
    RUN_TEST(test_invalid_setters_reject_out_of_range_values);
    RUN_TEST(test_parsed_g08_p2_enables_preview_and_high_precision);
    RUN_TEST(test_learning_correction_is_applied_by_service_rt);
    printf("test_nc_precision_v18_learning_service_more: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_precision.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcPrecision_Reset();
}

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = 1000;
    b.feed_override_percent = 100U;
    return b;
}

static int test_parameter_validation_and_defaults(void)
{
    setup();
    TEST_ASSERT_EQ_UINT(NC_PRECISION_LEARNING_GAIN_DEFAULT, g_ncPrecisionStatus.learning_gain_percent);
    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetLearningControl(1U, -1, 1U));
    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetLearningControl(1U, NC_PRECISION_LEARNING_GAIN_MAX + 1, 1U));
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, 50, 0U));
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.learning_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.learning_memory_window);

    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetVibrationControl(1U, NC_PRECISION_VIBRATION_FREQ_MAX_HZ + 1U, 1U));
    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetVibrationControl(1U, 100U, NC_PRECISION_VIBRATION_DAMPING_MAX + 1U));
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetVibrationControl(1U, 100U, 20U));

    TEST_ASSERT_EQ_INT(-1, NcPrecision_SetPreviewControl(1U, 0U, 10U));
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetPreviewControl(1U, 8U, 10U));
    TEST_ASSERT_EQ_UINT(8U, g_ncPrecisionStatus.preview_lookahead_blocks);
    return 0;
}

static int test_parsed_g08_and_feature_flags_enable_preview(void)
{
    NC_EXEC_BLOCK b = make_motion(61U);
    setup();
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    b.p_word = 2U;
    b.feature_flags = NC_FEATURE_FLAG_SMOOTHING;
    b.axis_target[0] = 5000;

    NcPrecision_OnParsedBlockTs(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.high_precision_contour_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.previewed_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.preview_feed_limited_blocks);
    TEST_ASSERT_EQ_UINT(61U, g_ncPrecisionStatus.last_preview_line_no);
    return 0;
}

static int test_rt_learning_and_vibration_service_updates_output(void)
{
    NC_EXEC_BLOCK b = make_motion(62U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, 50, 1U));
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetVibrationControl(1U, 120U, 20U));
    g_ioOut.axis_target[0] = 0;
    b.axis_target[0] = 100000;
    NcPrecision_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.learning_updates);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.vibration_filtered_samples);
    TEST_ASSERT_EQ_INT(100000, g_ncPrecisionStatus.last_following_error[0]);

    g_ioOut.axis_target[0] = 1000;
    NcPrecision_ServiceRt();
    TEST_ASSERT_TRUE(g_ioOut.axis_target[0] > 1000);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.rt_service_ticks);

    b.axis_target[0] = 220000;
    NcPrecision_OnBlockRt(&b);
    TEST_ASSERT_TRUE(g_ncPrecisionStatus.vibration_warnings >= 1U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_parameter_validation_and_defaults);
    RUN_TEST(test_parsed_g08_and_feature_flags_enable_preview);
    RUN_TEST(test_rt_learning_and_vibration_service_updates_output);
    printf("test_nc_precision: PASS\n");
    return 0;
}

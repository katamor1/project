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
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 1000;
    b.feed_override_percent = 80U;
    return b;
}

static int test_g08_and_g05_cancel_paths_clear_modes(void)
{
    NC_EXEC_BLOCK b = make_motion(981U);
    setup();

    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    b.p_word = 2U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.high_precision_contour_active);

    b.p_word = 0U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.high_precision_contour_active);

    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    b.p_word = 10000U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_TRUE(g_ncPrecisionStatus.hpcc_mode_blocks >= 1U);
    b.p_word = 0U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.preview_control_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.high_speed_cycle_active);
    return 0;
}

static int test_ai_precision_modes_and_preview_axis_mask_zero_noop(void)
{
    NC_EXEC_BLOCK b = make_motion(982U);
    setup();
    b.g_code10 = (uint16_t)NC_G_CODE10(5, 2);
    b.axis_mask = 0U;
    NcPrecision_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.learning_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.vibration_suppression_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncPrecisionStatus.free_surface_mode_active);
    TEST_ASSERT_EQ_UINT(0U, g_ncPrecisionStatus.previewed_blocks);
    return 0;
}

static int test_service_saturates_learning_correction_to_axis_limit(void)
{
    NC_EXEC_BLOCK b = make_motion(983U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, NC_PRECISION_LEARNING_GAIN_MAX, 1U));
    g_ioOut.axis_target[0] = NC_PRECISION_AXIS_TARGET_LIMIT - 1;
    b.axis_target[0] = NC_PRECISION_AXIS_TARGET_LIMIT;
    NcPrecision_OnBlockRt(&b);
    NcPrecision_ServiceRt();
    TEST_ASSERT_EQ_INT(NC_PRECISION_AXIS_TARGET_LIMIT, g_ioOut.axis_target[0]);

    setup();
    TEST_ASSERT_EQ_INT(0, NcPrecision_SetLearningControl(1U, NC_PRECISION_LEARNING_GAIN_MAX, 1U));
    g_ioOut.axis_target[0] = -NC_PRECISION_AXIS_TARGET_LIMIT + 1;
    b.axis_target[0] = -NC_PRECISION_AXIS_TARGET_LIMIT;
    NcPrecision_OnBlockRt(&b);
    NcPrecision_ServiceRt();
    TEST_ASSERT_EQ_INT(-NC_PRECISION_AXIS_TARGET_LIMIT, g_ioOut.axis_target[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g08_and_g05_cancel_paths_clear_modes);
    RUN_TEST(test_ai_precision_modes_and_preview_axis_mask_zero_noop);
    RUN_TEST(test_service_saturates_learning_correction_to_axis_limit);
    printf("test_nc_precision_edges: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_axis_config.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x0FU;
    b.axis_start[0] = 0;
    b.axis_start[1] = -5;
    b.axis_target[0] = 5;
    b.axis_target[1] = -5;
    b.axis_target[2] = 200;
    b.axis_target[3] = 300;
    b.arc_center[0] = 5;
    b.arc_center[1] = -5;
    return b;
}

static int test_axis_definition_normalizes_lowercase_name_and_updates_masks(void)
{
    setup();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetAxisDefinition(2U, (uint8_t)'c', NC_AXIS_TYPE_ROTARY, 7U));
    TEST_ASSERT_EQ_UINT((uint8_t)'C', g_ncAxisConfigStatus.axis_name[2]);
    TEST_ASSERT_EQ_UINT(NC_AXIS_TYPE_ROTARY, g_ncAxisConfigStatus.axis_type[2]);
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.rotary_axis_mask & (1UL << 2U)) != 0U);
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.linear_axis_mask & (1UL << 2U)) == 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.axis_definition_calls);
    return 0;
}

static int test_invalid_path_axis_mask_is_rejected(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(1UL << AXIS_MAX));
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.rejected_calls);
    TEST_ASSERT_EQ_UINT(NC_AXIS_CONFIG_DEFAULT_PATH_MASK, g_ncAxisConfigStatus.active_path_axis_mask);
    return 0;
}

static int test_diameter_mode_converts_positive_and_negative_half_away_from_zero(void)
{
    NC_EXEC_BLOCK b = make_motion(1621U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 1U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(1U, 1U));

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_INT(3, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(-3, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(3, b.arc_center[0]);
    TEST_ASSERT_EQ_INT(-3, b.arc_center[1]);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.diameter_converted_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.applied_blocks);
    return 0;
}

static int test_detached_and_path_filtered_axes_are_removed_from_motion_mask(void)
{
    NC_EXEC_BLOCK b = make_motion(1622U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x03U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDetachedMask(0x02U));

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_UINT(0x01U, b.axis_mask);
    TEST_ASSERT_EQ_INT(b.axis_start[1], b.axis_target[1]);
    TEST_ASSERT_EQ_INT(b.axis_start[2], b.axis_target[2]);
    TEST_ASSERT_EQ_INT(b.axis_start[3], b.axis_target[3]);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.detached_axis_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_filtered_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_definition_normalizes_lowercase_name_and_updates_masks);
    RUN_TEST(test_invalid_path_axis_mask_is_rejected);
    RUN_TEST(test_diameter_mode_converts_positive_and_negative_half_away_from_zero);
    RUN_TEST(test_detached_and_path_filtered_axes_are_removed_from_motion_mask);
    printf("test_nc_axis_config_v16_filter_more: PASS\n");
    return 0;
}

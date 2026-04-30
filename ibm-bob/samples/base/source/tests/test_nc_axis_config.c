#include "test_common.h"
#include "stub_external_io.h"
#include "nc_axis_config.h"

static int test_axis_definition_validation_normalizes_name_and_updates_masks(void)
{
    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetAxisDefinition(AXIS_MAX, 'X', NC_AXIS_TYPE_LINEAR, 0U));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetAxisDefinition(0U, '#', NC_AXIS_TYPE_LINEAR, 0U));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetAxisDefinition(0U, 'X', 99U, 0U));
    TEST_ASSERT_TRUE(g_ncAxisConfigStatus.rejected_calls >= 3U);

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetAxisDefinition(3U, 'c', NC_AXIS_TYPE_ROTARY, 1U));
    TEST_ASSERT_EQ_UINT((uint8_t)'C', g_ncAxisConfigStatus.axis_name[3]);
    TEST_ASSERT_EQ_UINT(NC_AXIS_TYPE_ROTARY, g_ncAxisConfigStatus.axis_type[3]);
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.rotary_axis_mask & 0x8U) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.axis_definition_calls);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count > 0U);
    return 0;
}

static int test_path_detached_and_diameter_mode_setters_validate_masks(void)
{
    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(0U));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(1UL << AXIS_MAX));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x3U));
    TEST_ASSERT_EQ_UINT(0x3U, g_ncAxisConfigStatus.active_path_axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_assignment_calls);

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetDetachedMask(1UL << AXIS_MAX));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDetachedMask(0x4U));
    TEST_ASSERT_EQ_UINT(0x4U, g_ncAxisConfigStatus.detached_axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.detached_set_calls);

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetDiameterMode(AXIS_MAX, 1U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 1U));
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.diameter_axis_mask & 0x1U) != 0U);
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 0U));
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.diameter_axis_mask & 0x1U) == 0U);
    return 0;
}

static int test_apply_block_filters_path_detached_axes_and_diameter_conversion(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    block = Test_MakeMotionBlock(70U, NC_MOTION_LINEAR, 0x7U, 101, 200, -101, 0);
    block.axis_start[0] = 1;
    block.axis_start[2] = -3;
    block.arc_center[0] = 5;

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_ApplyBlockTs(&block, NULL));

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x3U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDetachedMask(0x2U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 1U));

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(51, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(1, block.axis_start[0]);
    TEST_ASSERT_EQ_INT(3, block.arc_center[0]);
    TEST_ASSERT_EQ_INT(0, (int)(block.axis_mask & 0x6U));
    TEST_ASSERT_EQ_UINT(0x1U, block.axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.applied_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.diameter_converted_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.detached_axis_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_filtered_blocks);
    TEST_ASSERT_EQ_UINT(70U, g_ncAxisConfigStatus.last_line_no);
    return 0;
}

static int test_apply_non_motion_noop_preserves_block(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    block = Test_MakeMotionBlock(71U, NC_MOTION_AUX_ONLY, 0x1U, 100, 0, 0, 0);
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 1U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(100, block.axis_target[0]);
    TEST_ASSERT_EQ_UINT(0x1U, block.axis_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncAxisConfigStatus.applied_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_definition_validation_normalizes_name_and_updates_masks);
    RUN_TEST(test_path_detached_and_diameter_mode_setters_validate_masks);
    RUN_TEST(test_apply_block_filters_path_detached_axes_and_diameter_conversion);
    RUN_TEST(test_apply_non_motion_noop_preserves_block);
    printf("[  PASSED  ] test_nc_axis_config\n");
    return 0;
}

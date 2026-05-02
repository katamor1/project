#include "test_common.h"
#include "nc_axis_config.h"

/*
 * Direct tests for NcAxisConfig_ApplyBlockTs.  The test checks detached axes,
 * path filtering, diameter conversion and the non-motion/NULL early returns.
 */

static int test_apply_block_filters_detached_and_diameter_axes(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    memset(&block, 0, sizeof(block));

    block.line_no = 44U;
    block.motion_type = NC_MOTION_LINEAR;
    block.axis_mask = 0x0FU;
    block.axis_start[0] = -101;
    block.axis_target[0] = 101;
    block.arc_center[0] = 5;
    block.axis_start[1] = 200;
    block.axis_target[1] = 900;
    block.axis_start[2] = 300;
    block.axis_target[2] = 901;
    block.axis_start[3] = 400;
    block.axis_target[3] = 902;

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x03U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDetachedMask(0x02U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 1U));

    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(51, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(-51, block.axis_start[0]);
    TEST_ASSERT_EQ_INT(3, block.arc_center[0]);
    TEST_ASSERT_EQ_INT(200, block.axis_target[1]);
    TEST_ASSERT_EQ_INT(300, block.axis_target[2]);
    TEST_ASSERT_EQ_INT(400, block.axis_target[3]);
    TEST_ASSERT_EQ_UINT(0x01U, block.axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.applied_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.diameter_converted_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.detached_axis_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_filtered_blocks);
    TEST_ASSERT_EQ_UINT(44U, g_ncAxisConfigStatus.last_line_no);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 3U);
    return 0;
}

static int test_apply_block_early_returns_and_rejects(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;
    uint32_t applied;
    SystemShared_Initialize();
    memset(&block, 0, sizeof(block));

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_ApplyBlockTs(&block, NULL));

    block.motion_type = NC_MOTION_AUX_ONLY;
    block.axis_mask = 0x01U;
    block.axis_target[0] = 777;
    applied = g_ncAxisConfigStatus.applied_blocks;
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_UINT(applied, g_ncAxisConfigStatus.applied_blocks);
    TEST_ASSERT_EQ_INT(777, block.axis_target[0]);

    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetAxisDefinition(0U, 'X', 99U, 0U));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(1UL << AXIS_MAX));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetDetachedMask(1UL << AXIS_MAX));
    TEST_ASSERT_TRUE(g_ncAxisConfigStatus.rejected_calls >= 3U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_apply_block_filters_detached_and_diameter_axes);
    RUN_TEST(test_apply_block_early_returns_and_rejects);
    printf("test_nc_axis_config_apply_edges: PASS\n");
    return 0;
}

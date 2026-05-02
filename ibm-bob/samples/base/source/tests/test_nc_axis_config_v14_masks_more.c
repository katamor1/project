#include <string.h>
#include "test_common.h"
#include "nc_axis_config.h"

static NC_EXEC_BLOCK make_motion(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 44U;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x0FU;
    b.axis_target[0] = 100;
    b.axis_target[1] = 200;
    b.axis_target[2] = 300;
    b.axis_target[3] = 400;
    return b;
}

static int test_invalid_masks_are_rejected_and_counted(void)
{
    uint32_t before;
    SystemShared_Initialize();
    before = g_ncAxisConfigStatus.rejected_calls;
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(0U));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetDetachedMask(1UL << AXIS_MAX));
    TEST_ASSERT_TRUE(g_ncAxisConfigStatus.rejected_calls >= before + 2U);
    return 0;
}

static int test_detached_mask_removes_axes_from_motion_block(void)
{
    NC_EXEC_BLOCK b = make_motion();
    NC_ERROR_CODE e = NC_ERR_NONE;
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDetachedMask(0x02U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &e));
    TEST_ASSERT_EQ_UINT(0U, b.axis_mask & 0x02U);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.detached_axis_blocks);
    TEST_ASSERT_EQ_UINT(44U, g_ncAxisConfigStatus.last_line_no);
    return 0;
}

static int test_path_axis_mask_filters_axes_after_detach(void)
{
    NC_EXEC_BLOCK b = make_motion();
    NC_ERROR_CODE e = NC_ERR_NONE;
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x05U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &e));
    TEST_ASSERT_EQ_UINT(0x05U, b.axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_filtered_blocks);
    return 0;
}

static int test_apply_non_motion_preserves_targets_and_does_not_count(void)
{
    NC_EXEC_BLOCK b = make_motion();
    NC_ERROR_CODE e = NC_ERR_NONE;
    SystemShared_Initialize();
    b.motion_type = NC_MOTION_NONE;
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x01U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &e));
    TEST_ASSERT_EQ_UINT(0x0FU, b.axis_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncAxisConfigStatus.applied_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_invalid_masks_are_rejected_and_counted);
    RUN_TEST(test_detached_mask_removes_axes_from_motion_block);
    RUN_TEST(test_path_axis_mask_filters_axes_after_detach);
    RUN_TEST(test_apply_non_motion_preserves_targets_and_does_not_count);
    printf("test_nc_axis_config_v14_masks_more: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_axis_config.h"

static NC_EXEC_BLOCK make_motion(uint32_t line, uint32_t mask)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = mask;
    for (uint32_t i = 0U; i < AXIS_MAX; ++i) {
        b.axis_start[i] = (int32_t)(10 + (int32_t)i);
        b.axis_target[i] = (int32_t)(100 * ((int32_t)i + 1));
        b.arc_center[i] = (int32_t)(50 * ((int32_t)i + 1));
    }
    return b;
}

static int test_axis_definition_normalizes_lowercase_and_updates_masks(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetAxisDefinition(3U, (uint8_t)'c', NC_AXIS_TYPE_ROTARY, 2U));
    TEST_ASSERT_EQ_UINT((uint8_t)'C', g_ncAxisConfigStatus.axis_name[3]);
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.rotary_axis_mask & (1UL << 3U)) != 0U);
    TEST_ASSERT_EQ_UINT(2U, g_ncAxisConfigStatus.coordinate_group[3]);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count > 0U);
    return 0;
}

static int test_path_mask_rejects_zero_and_high_bits(void)
{
    uint32_t rejected;
    SystemShared_Initialize();
    rejected = g_ncAxisConfigStatus.rejected_calls;
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(0U));
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetPathAxisMask(1UL << AXIS_MAX));
    TEST_ASSERT_EQ_UINT(rejected + 2U, g_ncAxisConfigStatus.rejected_calls);
    TEST_ASSERT_EQ_UINT(NC_AXIS_CONFIG_DEFAULT_PATH_MASK, g_ncAxisConfigStatus.active_path_axis_mask);
    return 0;
}

static int test_apply_filters_detached_and_out_of_path_axes(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    b = make_motion(1901U, 0x0FU);
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x03U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDetachedMask(0x02U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0x01U, b.axis_mask);
    TEST_ASSERT_EQ_INT(11, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(12, b.axis_target[2]);
    TEST_ASSERT_EQ_INT(13, b.axis_target[3]);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.detached_axis_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_filtered_blocks);
    return 0;
}

static int test_diameter_mode_halves_positive_and_negative_fixed_values(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    b = make_motion(1902U, 0x01U);
    b.axis_start[0] = -101;
    b.axis_target[0] = 101;
    b.arc_center[0] = 99;
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U, 1U));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(-51, b.axis_start[0]);
    TEST_ASSERT_EQ_INT(51, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(50, b.arc_center[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.diameter_converted_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_definition_normalizes_lowercase_and_updates_masks);
    RUN_TEST(test_path_mask_rejects_zero_and_high_bits);
    RUN_TEST(test_apply_filters_detached_and_out_of_path_axes);
    RUN_TEST(test_diameter_mode_halves_positive_and_negative_fixed_values);
    printf("test_nc_axis_config_apply_more: PASS\n");
    return 0;
}

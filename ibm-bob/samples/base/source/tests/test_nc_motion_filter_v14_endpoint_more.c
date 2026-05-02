#include "test_common.h"
#include "nc_motion_filter.h"

static int test_invalid_config_mode_rejected_without_disabling_default(void)
{
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(-1, NcMotionFilter_SetConfigRt(1U, 99U, 3U, 3U));
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.enabled);
    return 0;
}

static int test_axis_limit_zero_uses_default_limit_values(void)
{
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, 0, 0));
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY, g_ncMotionFilterStatus.max_velocity_per_tick[0]);
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_ACCEL, g_ncMotionFilterStatus.max_accel_per_tick[0]);
    return 0;
}

static int test_disabled_apply_preserves_targets_and_counts_nothing(void)
{
    volatile int32_t target[AXIS_MAX] = {100, 0, 0, 0};
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(0U, NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE, 3U, 3U));
    NcMotionFilter_ApplyRt(target, 0x01U, 1U, 10U);
    TEST_ASSERT_EQ_INT(100, target[0]);
    TEST_ASSERT_EQ_UINT(0U, g_ncMotionFilterStatus.applied_samples);
    return 0;
}

static int test_endpoint_force_allows_final_target_to_snap_to_command(void)
{
    volatile int32_t target[AXIS_MAX] = {0, 0, 0, 0};
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, 10, 10));
    NcMotionFilter_ApplyRt(target, 0x01U, 0U, 10U);
    target[0] = 1000;
    NcMotionFilter_ApplyRt(target, 0x01U, 1U, 0U);
    TEST_ASSERT_EQ_INT(1000, target[0]);
    TEST_ASSERT_TRUE((g_ncMotionFilterStatus.filtered_axis_mask & 1U) != 0U);
    TEST_ASSERT_EQ_INT(1000, g_ncMotionFilterStatus.last_filtered_target[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_invalid_config_mode_rejected_without_disabling_default);
    RUN_TEST(test_axis_limit_zero_uses_default_limit_values);
    RUN_TEST(test_disabled_apply_preserves_targets_and_counts_nothing);
    RUN_TEST(test_endpoint_force_allows_final_target_to_snap_to_command);
    printf("test_nc_motion_filter_v14_endpoint_more: PASS\n");
    return 0;
}

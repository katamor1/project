#include "test_common.h"
#include "system_shared.h"
#include "nc_motion_filter.h"

static int test_disabled_config_leaves_axis_targets_unchanged(void)
{
    volatile int32_t axis[AXIS_MAX] = {1000, 2000, 3000, 4000};
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(0U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 3U, 3U));
    NcMotionFilter_ApplyRt(axis, 0x03U, 0U, 10U);
    TEST_ASSERT_EQ_INT(1000, axis[0]);
    TEST_ASSERT_EQ_INT(2000, axis[1]);
    TEST_ASSERT_EQ_UINT(0U, g_ncMotionFilterStatus.applied_samples);
    return 0;
}

static int test_axis_limit_accepts_negative_and_zero_as_normalized_values(void)
{
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(1U, -123, 0));
    TEST_ASSERT_EQ_INT(123, g_ncMotionFilterStatus.max_velocity_per_tick[1]);
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_ACCEL, g_ncMotionFilterStatus.max_accel_per_tick[1]);
    TEST_ASSERT_EQ_INT(-1, NcMotionFilter_SetAxisLimitRt((uint8_t)AXIS_MAX, 1, 1));
    return 0;
}

static int test_reset_preserves_preconfigured_limits_and_clamps_windows(void)
{
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, 321, 45));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(1U,
        NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE,
        (uint8_t)(NC_MOTION_FILTER_VELOCITY_WINDOW_MAX + 3U),
        (uint8_t)(NC_MOTION_FILTER_ACCEL_WINDOW_MAX + 4U)));
    TEST_ASSERT_EQ_INT(321, g_ncMotionFilterStatus.max_velocity_per_tick[0]);
    TEST_ASSERT_EQ_INT(45, g_ncMotionFilterStatus.max_accel_per_tick[0]);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_VELOCITY_WINDOW_MAX, g_ncMotionFilterStatus.velocity_window);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_ACCEL_WINDOW_MAX, g_ncMotionFilterStatus.accel_window);
    return 0;
}

static int test_force_endpoint_records_correction_after_lag_is_created(void)
{
    volatile int32_t axis[AXIS_MAX] = {0, 0, 0, 0};
    SystemShared_Initialize();
    NcMotionFilter_ResetRt();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(1U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 3U, 3U));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, 100, 10));
    NcMotionFilter_ApplyRt(axis, 0x01U, 0U, 10U); /* initialize */
    axis[0] = 1000;
    NcMotionFilter_ApplyRt(axis, 0x01U, 0U, 10U);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.last_lag_error[0] != 0);
    axis[0] = 2000;
    NcMotionFilter_ApplyRt(axis, 0x01U, 1U, 0U);
    TEST_ASSERT_EQ_INT(2000, axis[0]);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.endpoint_corrections > 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_disabled_config_leaves_axis_targets_unchanged);
    RUN_TEST(test_axis_limit_accepts_negative_and_zero_as_normalized_values);
    RUN_TEST(test_reset_preserves_preconfigured_limits_and_clamps_windows);
    RUN_TEST(test_force_endpoint_records_correction_after_lag_is_created);
    printf("test_nc_motion_filter_v18_disable_limits_more: PASS\n");
    return 0;
}

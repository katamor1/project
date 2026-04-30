#include "test_common.h"
#include "stub_external_io.h"
#include "nc_motion_filter.h"

static int test_reset_restores_default_rt_safe_configuration(void)
{
    uint32_t axis;

    Test_ResetSystem();

    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.enabled);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_SECOND_STAGE_FIR, g_ncMotionFilterStatus.second_stage_mode);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_DEFAULT_VELOCITY_WINDOW, g_ncMotionFilterStatus.velocity_window);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_DEFAULT_ACCEL_WINDOW, g_ncMotionFilterStatus.accel_window);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_ENDPOINT_WINDOW, g_ncMotionFilterStatus.endpoint_correction_window);
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY, g_ncMotionFilterStatus.max_velocity_per_tick[axis]);
        TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_ACCEL, g_ncMotionFilterStatus.max_accel_per_tick[axis]);
    }
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.generation);
    return 0;
}

static int test_set_config_rejects_invalid_mode_and_clamps_windows(void)
{
    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(-1, NcMotionFilter_SetConfigRt(1U, 99U, 3U, 3U));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(1U,
                                                     NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE,
                                                     0U,
                                                     (uint8_t)(NC_MOTION_FILTER_ACCEL_WINDOW_MAX + 10U)));
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.enabled);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE, g_ncMotionFilterStatus.second_stage_mode);
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.velocity_window);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_ACCEL_WINDOW_MAX, g_ncMotionFilterStatus.accel_window);
    return 0;
}

static int test_set_axis_limit_validation_normalization_and_clamp(void)
{
    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(-1, NcMotionFilter_SetAxisLimitRt(AXIS_MAX, 100, 100));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, -120, 0));
    TEST_ASSERT_EQ_INT(120, g_ncMotionFilterStatus.max_velocity_per_tick[0]);
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_ACCEL, g_ncMotionFilterStatus.max_accel_per_tick[0]);

    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(1U,
                                                        NC_MOTION_FILTER_AXIS_LIMIT_MAX + 10,
                                                        NC_MOTION_FILTER_AXIS_LIMIT_MAX + 20));
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_AXIS_LIMIT_MAX, g_ncMotionFilterStatus.max_velocity_per_tick[1]);
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_AXIS_LIMIT_MAX, g_ncMotionFilterStatus.max_accel_per_tick[1]);
    return 0;
}

static int test_apply_null_and_disabled_are_noop(void)
{
    volatile int32_t target[AXIS_MAX] = {100, 200, 300, 400};
    uint32_t generation;

    Test_ResetSystem();
    generation = g_ncMotionFilterStatus.generation;
    NcMotionFilter_ApplyRt(NULL, 0x1U, 0U, 10U);
    TEST_ASSERT_EQ_UINT(generation, g_ncMotionFilterStatus.generation);

    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(0U,
                                                     NC_MOTION_FILTER_SECOND_STAGE_FIR,
                                                     3U,
                                                     5U));
    generation = g_ncMotionFilterStatus.generation;
    NcMotionFilter_ApplyRt(target, 0x1U, 0U, 10U);
    TEST_ASSERT_EQ_INT(100, target[0]);
    TEST_ASSERT_EQ_UINT(generation, g_ncMotionFilterStatus.generation);
    return 0;
}

static int test_apply_first_sample_initializes_without_changing_target(void)
{
    volatile int32_t target[AXIS_MAX] = {0, 0, 0, 0};

    Test_ResetSystem();
    g_ncProgramStatus.exec_line_no = 123U;
    NcMotionFilter_ApplyRt(target, 0x1U, 0U, 10U);

    TEST_ASSERT_EQ_INT(0, target[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.applied_samples);
    TEST_ASSERT_EQ_UINT(0U, g_ncMotionFilterStatus.filtered_axis_mask);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 1U);
    return 0;
}

static int test_apply_axis_limit_lag_and_force_endpoint_correction(void)
{
    volatile int32_t target[AXIS_MAX] = {0, 0, 0, 0};
    uint32_t endpointCorrectionsBefore;

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(1U,
                                                     NC_MOTION_FILTER_SECOND_STAGE_FIR,
                                                     1U,
                                                     1U));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, 80, 1000));

    NcMotionFilter_ApplyRt(target, 0x1U, 0U, 10U);
    target[0] = 1000;
    NcMotionFilter_ApplyRt(target, 0x1U, 0U, 10U);

    TEST_ASSERT_EQ_INT(80, target[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.velocity_limit_events);
    TEST_ASSERT_EQ_INT(1000, g_ncMotionFilterStatus.last_raw_velocity[0]);
    TEST_ASSERT_EQ_INT(80, g_ncMotionFilterStatus.last_velocity_stage2[0]);
    TEST_ASSERT_EQ_INT(920, g_ncMotionFilterStatus.last_lag_error[0]);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.max_lag_error[0] >= 920);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 2U);

    endpointCorrectionsBefore = g_ncMotionFilterStatus.endpoint_corrections;
    target[0] = 1000;
    NcMotionFilter_ApplyRt(target, 0x1U, 1U, 0U);
    TEST_ASSERT_EQ_INT(1000, target[0]);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.endpoint_corrections > endpointCorrectionsBefore);
    TEST_ASSERT_EQ_INT(0, g_ncMotionFilterStatus.last_lag_error[0]);
    return 0;
}

static int test_unmasked_axes_are_initialized_but_not_reported_as_filtered(void)
{
    volatile int32_t target[AXIS_MAX] = {10, 20, 30, 40};

    Test_ResetSystem();
    NcMotionFilter_ApplyRt(target, 0x0U, 0U, 10U);
    TEST_ASSERT_EQ_INT(10, target[0]);
    TEST_ASSERT_EQ_INT(20, target[1]);
    TEST_ASSERT_EQ_UINT(0U, g_ncMotionFilterStatus.filtered_axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.applied_samples);
    return 0;
}

int main(void)
{
    RUN_TEST(test_reset_restores_default_rt_safe_configuration);
    RUN_TEST(test_set_config_rejects_invalid_mode_and_clamps_windows);
    RUN_TEST(test_set_axis_limit_validation_normalization_and_clamp);
    RUN_TEST(test_apply_null_and_disabled_are_noop);
    RUN_TEST(test_apply_first_sample_initializes_without_changing_target);
    RUN_TEST(test_apply_axis_limit_lag_and_force_endpoint_correction);
    RUN_TEST(test_unmasked_axes_are_initialized_but_not_reported_as_filtered);
    printf("[  PASSED  ] test_nc_motion_filter\n");
    return 0;
}

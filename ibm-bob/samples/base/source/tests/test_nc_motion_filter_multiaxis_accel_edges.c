#include "test_common.h"
#include "nc_motion_filter.h"

static int test_multiaxis_limits_and_unmasked_noop(void)
{
    volatile int32_t target[AXIS_MAX] = {0, 1000, 2000, 3000};
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(1U, NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE, 1U, 1U));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(0U, 10, 5));
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetAxisLimitRt(1U, -1, -1));
    TEST_ASSERT_EQ_INT(-1, NcMotionFilter_SetAxisLimitRt(AXIS_MAX, 1, 1));
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY, g_ncMotionFilterStatus.max_velocity_per_tick[1]);

    NcMotionFilter_ApplyRt(target, 0x03U, 0U, 5U); /* initialize */
    target[0] = 1000;
    target[1] = 2000;
    target[2] = 9999; /* unmasked axis remains accepted as current no-op state */
    NcMotionFilter_ApplyRt(target, 0x03U, 0U, 5U);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.accel_limit_events > 0U);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.velocity_limit_events > 0U);
    TEST_ASSERT_EQ_INT(9999, target[2]);
    return 0;
}

static int test_disabled_and_endpoint_force_paths(void)
{
    volatile int32_t target[AXIS_MAX] = {0, 0, 0, 0};
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(0U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 3U, 3U));
    target[0] = 1234;
    NcMotionFilter_ApplyRt(target, 0x01U, 1U, 0U);
    TEST_ASSERT_EQ_INT(1234, target[0]);

    TEST_ASSERT_EQ_INT(0, NcMotionFilter_SetConfigRt(1U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 1U, 1U));
    NcMotionFilter_ApplyRt(target, 0x01U, 0U, 1U);
    target[0] = 4321;
    NcMotionFilter_ApplyRt(target, 0x01U, 1U, 0U);
    TEST_ASSERT_EQ_INT(4321, target[0]);
    TEST_ASSERT_TRUE(g_ncMotionFilterStatus.endpoint_corrections > 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_multiaxis_limits_and_unmasked_noop);
    RUN_TEST(test_disabled_and_endpoint_force_paths);
    printf("test_nc_motion_filter_multiaxis_accel_edges: PASS\n");
    return 0;
}

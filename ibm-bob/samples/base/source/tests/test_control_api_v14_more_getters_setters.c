#include "test_common.h"
#include "control_api.h"

static int test_axis_config_api_setters_and_getter_copy_status(void)
{
    NC_AXIS_CONFIG_STATUS st;
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDefinition(0U, 'x', NC_AXIS_TYPE_LINEAR, 1U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcPathAxisMask(0x03U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDetachedMask(0x04U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDiameterMode(0U, 1U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcAxisConfigStatus(&st));
    TEST_ASSERT_EQ_UINT(0x03U, st.active_path_axis_mask);
    TEST_ASSERT_EQ_UINT(0x04U, st.detached_axis_mask);
    TEST_ASSERT_TRUE((st.diameter_axis_mask & 0x01U) != 0U);
    return 0;
}

static int test_motion_filter_api_setter_getter_and_axis_limit(void)
{
    NC_MOTION_FILTER_STATUS st;
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, Api_SetNcMotionFilterConfig(1U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 4U, 4U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcMotionFilterAxisLimit(1U, 123, 45));
    TEST_ASSERT_EQ_INT(0, Api_GetNcMotionFilterStatus(&st));
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_SECOND_STAGE_FIR, st.second_stage_mode);
    TEST_ASSERT_EQ_INT(123, st.max_velocity_per_tick[1]);
    TEST_ASSERT_EQ_INT(45, st.max_accel_per_tick[1]);
    return 0;
}

static int test_api_rejects_invalid_axis_config_and_motion_filter_parameters(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, Api_SetNcAxisDefinition(AXIS_MAX, 'X', NC_AXIS_TYPE_LINEAR, 0U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcPathAxisMask(0U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcAxisDiameterMode(AXIS_MAX, 1U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcMotionFilterAxisLimit(AXIS_MAX, 1, 1));
    return 0;
}

static int test_api_additional_getters_reject_null_outputs(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, Api_GetNcAxisConfigStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcMotionFilterStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcEventRing(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcCapabilityStatus(NULL));
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_config_api_setters_and_getter_copy_status);
    RUN_TEST(test_motion_filter_api_setter_getter_and_axis_limit);
    RUN_TEST(test_api_rejects_invalid_axis_config_and_motion_filter_parameters);
    RUN_TEST(test_api_additional_getters_reject_null_outputs);
    printf("test_control_api_v14_more_getters_setters: PASS\n");
    return 0;
}

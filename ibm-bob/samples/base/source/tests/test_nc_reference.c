#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_reference.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcReference_Reset();
}

static NC_EXEC_BLOCK make_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_REFERENCE_RETURN;
    b.axis_mask = 0x03U;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 2000;
    return b;
}

static int test_axis_config_validation_and_defaults(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, NcReference_SetAxisConfig(AXIS_MAX, 0, 1, 100, 0U));
    TEST_ASSERT_EQ_INT(-1, NcReference_SetAxisConfig(0U, 0, 0, 100, 0U));
    TEST_ASSERT_EQ_INT(-1, NcReference_SetAxisConfig(0U, 0, 1, -1, 0U));
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 12345, -1, 50000, 1U));
    TEST_ASSERT_EQ_INT(12345, g_ncReferenceStatus.reference_position[1]);
    TEST_ASSERT_EQ_INT(-1, g_ncReferenceStatus.approach_direction[1]);
    TEST_ASSERT_TRUE((g_ncReferenceStatus.distance_coded_axis_mask & 0x02U) != 0U);

    TEST_ASSERT_EQ_INT(-1, NcReference_SetOneDirectionApproach(AXIS_MAX, 100));
    TEST_ASSERT_EQ_INT(-1, NcReference_SetOneDirectionApproach(0U, -1));
    TEST_ASSERT_EQ_INT(0, NcReference_SetOneDirectionApproach(0U, 333));
    TEST_ASSERT_EQ_INT(333, g_ncReferenceStatus.one_direction_approach[0]);
    return 0;
}

static int test_reference_return_parsed_and_rt_completion(void)
{
    NC_EXEC_BLOCK b = make_block(101U);
    setup();
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(28);
    NcReference_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.planned_reference_return_blocks);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_REFERENCE_RETURN) != 0U);

    NcReference_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.executed_reference_return_blocks);
    TEST_ASSERT_TRUE((g_ncReferenceStatus.homed_axis_mask & 0x03U) == 0x03U);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_REFERENCE_RETURN, g_ncEventRing.items[0].code);
    return 0;
}

static int test_one_direction_and_reference_marker_capture(void)
{
    NC_EXEC_BLOCK b = make_block(102U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcReference_SetOneDirectionApproach(0U, 100));
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(60);
    b.axis_mask = 0x01U;
    b.axis_target[0] = 1000;
    NcReference_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.one_direction_positioning_blocks);
    TEST_ASSERT_EQ_INT(900, g_ncReferenceStatus.last_approach_start[0]);
    NcReference_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.executed_one_direction_blocks);

    g_ioOut.axis_target[0] = 5555;
    g_ioIn.sensor_bits = SENSOR_REFERENCE_MARK_BASE_BIT;
    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.reference_marker_captures);
    TEST_ASSERT_EQ_UINT(0U, g_ncReferenceStatus.last_marker_axis);
    TEST_ASSERT_EQ_INT(5555, g_ncReferenceStatus.last_marker_position[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_config_validation_and_defaults);
    RUN_TEST(test_reference_return_parsed_and_rt_completion);
    RUN_TEST(test_one_direction_and_reference_marker_capture);
    printf("test_nc_reference: PASS\n");
    return 0;
}

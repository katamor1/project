#include "test_common.h"
#include "system_shared.h"
#include "nc_reference.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_ref_block(uint32_t line, uint16_t gcode, uint32_t mask)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_REFERENCE_RETURN;
    b.g_code10 = gcode;
    b.axis_mask = mask;
    b.axis_target[0] = 123;
    b.axis_target[1] = 456;
    return b;
}

static int test_axis_config_rejects_invalid_direction_and_negative_rollover(void)
{
    SystemShared_Initialize();
    NcReference_Reset();
    TEST_ASSERT_EQ_INT(-1, NcReference_SetAxisConfig(0U, 0, 0, 1000, 0U));
    TEST_ASSERT_EQ_INT(-1, NcReference_SetAxisConfig(0U, 0, 1, -1, 0U));
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 777, -1, 9999, 1U));
    TEST_ASSERT_EQ_INT(777, g_ncReferenceStatus.reference_position[1]);
    TEST_ASSERT_TRUE((g_ncReferenceStatus.distance_coded_axis_mask & 0x02U) != 0U);
    return 0;
}

static int test_parsed_g28_without_axis_mask_requests_all_axes(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcReference_Reset();
    b = make_ref_block(1971U, (uint16_t)NC_G_CODE_WHOLE(28), 0U);
    NcReference_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT((1UL << AXIS_MAX) - 1UL, g_ncReferenceStatus.requested_axis_mask);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_REFERENCE_RETURN) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.planned_reference_return_blocks);
    return 0;
}

static int test_rt_g30_updates_completed_and_homed_masks(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcReference_Reset();
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(0U, 111, 1, 10000, 0U));
    b = make_ref_block(1972U, (uint16_t)NC_G_CODE_WHOLE(30), 0x03U);
    NcReference_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(0x03U, g_ncReferenceStatus.completed_axis_mask);
    TEST_ASSERT_EQ_UINT(0x03U, g_ncReferenceStatus.homed_axis_mask);
    TEST_ASSERT_EQ_INT(111, g_ncReferenceStatus.last_reference_position[0]);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count > 0U);
    return 0;
}

static int test_service_captures_distance_coded_reference_mark_rising_edge(void)
{
    SystemShared_Initialize();
    NcReference_Reset();
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(2U, 222, 1, 10000, 1U));
    g_ioOut.axis_target[2] = 3456;
    g_ioIn.sensor_bits = SENSOR_REFERENCE_MARK_BASE_BIT << 2U;
    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.reference_marker_captures);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.distance_coded_marker_captures);
    TEST_ASSERT_EQ_UINT(2U, g_ncReferenceStatus.last_marker_axis);
    TEST_ASSERT_EQ_INT(3456, g_ncReferenceStatus.last_marker_position[2]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_config_rejects_invalid_direction_and_negative_rollover);
    RUN_TEST(test_parsed_g28_without_axis_mask_requests_all_axes);
    RUN_TEST(test_rt_g30_updates_completed_and_homed_masks);
    RUN_TEST(test_service_captures_distance_coded_reference_mark_rising_edge);
    printf("test_nc_reference_v19_markers_more: PASS\n");
    return 0;
}

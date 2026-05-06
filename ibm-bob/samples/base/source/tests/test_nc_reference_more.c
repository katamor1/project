#include <string.h>
#include "test_common.h"
#include "nc_reference.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t code10, uint32_t axisMask)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 21U;
    b.g_code10 = (uint16_t)code10;
    b.motion_type = NC_MOTION_REFERENCE_RETURN;
    b.axis_mask = axisMask;
    b.axis_target[0] = 1000;
    b.axis_target[1] = -1000;
    return b;
}

static int test_axis_config_distance_coded_marker_mask_toggles(void)
{
    SystemShared_Initialize();
    NcReference_Reset();
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 12345, -1, 100000, 1U));
    TEST_ASSERT_EQ_INT(12345, g_ncReferenceStatus.reference_position[1]);
    TEST_ASSERT_EQ_INT(-1, g_ncReferenceStatus.approach_direction[1]);
    TEST_ASSERT_TRUE((g_ncReferenceStatus.distance_coded_axis_mask & 0x02U) != 0U);
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 12345, -1, 100000, 0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncReferenceStatus.distance_coded_axis_mask & 0x02U);
    return 0;
}

static int test_reference_return_without_axis_mask_defaults_all_axes(void)
{
    NC_EXEC_BLOCK b = make_block(NC_G_CODE_WHOLE(28), 0U);
    uint32_t allMask = (uint32_t)((1UL << AXIS_MAX) - 1UL);
    SystemShared_Initialize();
    NcReference_Reset();
    NcReference_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(allMask, g_ncReferenceStatus.requested_axis_mask & allMask);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_REFERENCE_RETURN) != 0U);
    NcReference_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(allMask, g_ncReferenceStatus.homed_axis_mask & allMask);
    return 0;
}

static int test_second_reference_return_counter_for_g30(void)
{
    NC_EXEC_BLOCK b = make_block(NC_G_CODE_WHOLE(30), 0x01U);
    SystemShared_Initialize();
    NcReference_Reset();
    NcReference_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.planned_reference_return_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.second_reference_return_blocks);
    return 0;
}

static int test_reference_mark_rising_edge_captured_once(void)
{
    uint32_t bit = SENSOR_REFERENCE_MARK_BASE_BIT;
    SystemShared_Initialize();
    NcReference_Reset();
    g_ioOut.axis_target[0] = 777;
    g_ioIn.sensor_bits = bit;
    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.reference_marker_captures);
    TEST_ASSERT_EQ_INT(777, g_ncReferenceStatus.last_marker_position[0]);
    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.reference_marker_captures);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_config_distance_coded_marker_mask_toggles);
    RUN_TEST(test_reference_return_without_axis_mask_defaults_all_axes);
    RUN_TEST(test_second_reference_return_counter_for_g30);
    RUN_TEST(test_reference_mark_rising_edge_captured_once);
    printf("test_nc_reference_more: PASS\n");
    return 0;
}

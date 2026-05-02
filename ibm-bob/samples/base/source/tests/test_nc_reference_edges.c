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
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_REFERENCE_RETURN;
    b.feed_override_percent = 100U;
    return b;
}

static int test_g30_without_axis_word_targets_all_axes_and_records_second_reference(void)
{
    NC_EXEC_BLOCK b = make_block(901U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 222, -1, 1000, 1U));
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(30);
    b.axis_mask = 0U;

    NcReference_OnParsedBlockTs(&b);
    NcReference_OnBlockRt(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.second_reference_return_blocks);
    TEST_ASSERT_EQ_UINT((1U << AXIS_MAX) - 1U, g_ncReferenceStatus.requested_axis_mask);
    TEST_ASSERT_EQ_UINT((1U << AXIS_MAX) - 1U, g_ncReferenceStatus.homed_axis_mask);
    TEST_ASSERT_EQ_INT(222, g_ncReferenceStatus.last_reference_position[1]);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_REFERENCE_RETURN, g_ncEventRing.items[g_ncEventRing.captured_count - 1U].code);
    return 0;
}

static int test_rollover_warning_on_parsed_motion_and_null_noop(void)
{
    NC_EXEC_BLOCK b = make_block(902U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 0, 1, 100, 0U));
    b.motion_type = NC_MOTION_LINEAR;
    b.g_code10 = 0U;
    b.axis_mask = 0x02U;
    b.axis_target[1] = -101;

    NcReference_OnParsedBlockTs(NULL);
    NcReference_OnBlockRt(NULL);
    NcReference_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.rollover_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.last_rollover_axis);
    TEST_ASSERT_EQ_INT(-101, g_ncReferenceStatus.last_rollover_position);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_REFERENCE_ROLLOVER, g_ncEventRing.items[g_ncEventRing.captured_count - 1U].code);
    return 0;
}

static int test_distance_coded_marker_capture_is_rising_edge_only(void)
{
    setup();
    TEST_ASSERT_EQ_INT(0, NcReference_SetAxisConfig(1U, 0, 1, 1000, 1U));
    g_ncProgramStatus.exec_line_no = 903U;
    g_ioOut.axis_target[1] = 12345;
    g_ioIn.sensor_bits = SENSOR_REFERENCE_MARK_BASE_BIT << 1U;

    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.reference_marker_captures);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.distance_coded_marker_captures);
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.last_marker_axis);
    TEST_ASSERT_EQ_INT(12345, g_ncReferenceStatus.last_marker_position[1]);

    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncReferenceStatus.reference_marker_captures);
    g_ioIn.sensor_bits = 0U;
    NcReference_ServiceRt();
    g_ioIn.sensor_bits = SENSOR_REFERENCE_MARK_BASE_BIT << 1U;
    NcReference_ServiceRt();
    TEST_ASSERT_EQ_UINT(2U, g_ncReferenceStatus.reference_marker_captures);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g30_without_axis_word_targets_all_axes_and_records_second_reference);
    RUN_TEST(test_rollover_warning_on_parsed_motion_and_null_noop);
    RUN_TEST(test_distance_coded_marker_capture_is_rising_edge_only);
    printf("test_nc_reference_edges: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_compensation.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCompensation_Reset();
}

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x07U;
    b.axis_target[0] = 10000;
    b.axis_target[1] = 0;
    b.axis_target[2] = 1000;
    return b;
}

static int test_offset_table_boundaries(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetToolLengthOffset(0U, 100));
    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetToolLengthOffset(NC_TOOL_OFFSET_TABLE_SIZE, 100));
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetToolLengthOffset(1U, 250));
    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetCutterRadiusOffset(0U, 100));
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetCutterRadiusOffset(2U, 100));
    return 0;
}

static int test_tool_length_apply_and_cancel(void)
{
    NC_EXEC_BLOCK b = make_linear(81U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    (void)NcCompensation_SetToolLengthOffset(1U, 250);
    b.tool_length_command = NC_TOOL_LENGTH_CMD_POSITIVE;
    b.h_offset_no = 1U;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.tool_length_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.active_h_no);
    TEST_ASSERT_EQ_INT(1250, b.axis_target[2]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_TOOL_LENGTH) != 0U);

    memset(&b, 0, sizeof(b));
    b.tool_length_command = NC_TOOL_LENGTH_CMD_CANCEL;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.tool_length_active);
    return 0;
}

static int test_cutter_radius_left_adjusts_xy_target(void)
{
    NC_EXEC_BLOCK b = make_linear(82U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    (void)NcCompensation_SetCutterRadiusOffset(2U, 100);
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 0;
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_LEFT;
    b.d_offset_no = 2U;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.cutter_comp_mode);
    TEST_ASSERT_EQ_INT(1000, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(100, b.axis_target[1]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_CUTTER_RADIUS) != 0U);
    return 0;
}

static int test_null_and_bad_offset_report_errors(void)
{
    NC_EXEC_BLOCK b = make_linear(83U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcCompensation_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcCompensation_ApplyBlockTs(&b, NULL));
    b.tool_length_command = NC_TOOL_LENGTH_CMD_POSITIVE;
    b.h_offset_no = 0U;
    TEST_ASSERT_EQ_INT(-1, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    return 0;
}

int main(void)
{
    RUN_TEST(test_offset_table_boundaries);
    RUN_TEST(test_tool_length_apply_and_cancel);
    RUN_TEST(test_cutter_radius_left_adjusts_xy_target);
    RUN_TEST(test_null_and_bad_offset_report_errors);
    printf("test_nc_compensation: PASS\n");
    return 0;
}

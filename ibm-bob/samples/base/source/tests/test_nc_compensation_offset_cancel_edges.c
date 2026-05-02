#include "test_common.h"
#include "system_shared.h"
#include "nc_compensation.h"

static NC_EXEC_BLOCK make_motion_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x07U;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 0;
    b.axis_target[2] = 100;
    return b;
}

static int test_offset_table_rejects_zero_and_upper_boundary(void)
{
    SystemShared_Initialize();
    NcCompensation_Reset();

    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetToolLengthOffset(0U, 100));
    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetToolLengthOffset(NC_TOOL_OFFSET_TABLE_SIZE, 100));
    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetCutterRadiusOffset(0U, 100));
    TEST_ASSERT_EQ_INT(-1, NcCompensation_SetCutterRadiusOffset(NC_TOOL_OFFSET_TABLE_SIZE, 100));
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetToolLengthOffset(NC_TOOL_OFFSET_TABLE_SIZE - 1U, 100));
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetCutterRadiusOffset(NC_TOOL_OFFSET_TABLE_SIZE - 1U, 50));
    return 0;
}

static int test_missing_h_or_d_offset_returns_range_error(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCompensation_Reset();

    b = make_motion_block(620U);
    b.tool_length_command = NC_TOOL_LENGTH_CMD_POSITIVE;
    b.h_offset_no = NC_TOOL_OFFSET_TABLE_SIZE;
    TEST_ASSERT_EQ_INT(-1, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);

    err = NC_ERR_NONE;
    b = make_motion_block(621U);
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_LEFT;
    b.d_offset_no = NC_TOOL_OFFSET_TABLE_SIZE;
    TEST_ASSERT_EQ_INT(-1, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    return 0;
}

static int test_cancel_commands_clear_active_status_without_target_change(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCompensation_Reset();

    TEST_ASSERT_EQ_INT(0, NcCompensation_SetToolLengthOffset(1U, 200));
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetCutterRadiusOffset(1U, 50));
    b = make_motion_block(622U);
    b.tool_length_command = NC_TOOL_LENGTH_CMD_POSITIVE;
    b.h_offset_no = 1U;
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_LEFT;
    b.d_offset_no = 1U;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.tool_length_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.cutter_comp_mode);

    memset(&b, 0, sizeof(b));
    b.tool_length_command = NC_TOOL_LENGTH_CMD_CANCEL;
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_CANCEL;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.tool_length_active);
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.cutter_comp_mode);
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.active_h_no);
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.active_d_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_offset_table_rejects_zero_and_upper_boundary);
    RUN_TEST(test_missing_h_or_d_offset_returns_range_error);
    RUN_TEST(test_cancel_commands_clear_active_status_without_target_change);
    printf("test_nc_compensation_offset_cancel_edges: PASS\n");
    return 0;
}

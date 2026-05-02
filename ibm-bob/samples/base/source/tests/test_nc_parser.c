#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcParse_ResetModal();
}

static int test_parse_basic_motion_aux_and_comments(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("G01 X1.5 Y-2.0 Z.25 F1200 S3000 T4 M06 (tool change)", 10U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(10U, b.line_no);
    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, b.motion_type);
    TEST_ASSERT_TRUE((b.axis_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_INT(-20000, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(2500, b.axis_target[2]);
    TEST_ASSERT_EQ_INT(12000000, b.feed);
    TEST_ASSERT_EQ_UINT(3000U, b.spindle_speed);
    TEST_ASSERT_EQ_UINT(4U, b.tool_no);
    TEST_ASSERT_EQ_UINT(6U, b.aux_m_code);
    TEST_ASSERT_TRUE((b.aux_flags & NC_AUX_FLAG_M_CODE) != 0U);
    TEST_ASSERT_TRUE((b.aux_flags & NC_AUX_FLAG_TOOL) != 0U);
    TEST_ASSERT_TRUE((b.aux_flags & NC_AUX_FLAG_SPINDLE) != 0U);
    return 0;
}

static int test_parse_blank_line_is_noop(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(1, NcParse_LineToBlock("   (comment only)   ", 11U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    return 0;
}

static int test_parse_invalid_token_and_unsupported_address(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G01 X1 #", 12U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_TOKEN, err);
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G999", 13U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_UNSUPPORTED, err);
    return 0;
}

static int test_parse_modal_conflict_and_coordinate_error(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G90 G91", 14U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_MODAL_CONFLICT, err);

    setup();
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("G91", 15U, &b, &err));
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G53 G00 X0", 16U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_COORDINATE, err);
    return 0;
}

static int test_parse_feature_boundaries(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G08", 17U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);

    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("G08 P2", 18U, &b, &err));
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_SMOOTHING) != 0U);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_HIGH_PRECISION) != 0U);

    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G20 G21", 19U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_MODAL_CONFLICT, err);
    return 0;
}

int main(void)
{
    RUN_TEST(test_parse_basic_motion_aux_and_comments);
    RUN_TEST(test_parse_blank_line_is_noop);
    RUN_TEST(test_parse_invalid_token_and_unsupported_address);
    RUN_TEST(test_parse_modal_conflict_and_coordinate_error);
    RUN_TEST(test_parse_feature_boundaries);
    printf("test_nc_parser: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_coordinate.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcParse_ResetModal();
}

static int test_inch_mode_scales_linear_axis_to_mm_fixed_point(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("G20", 1870U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(NC_UNIT_INCH, g_ncCoordinateState.unit_mode);
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("G01 X1.0 F1.0", 1871U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_TRUE((b.axis_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_INT(254000, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(254000, b.feed);
    return 0;
}

static int test_same_modal_group_on_one_line_reports_conflict(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G17 G18 X1.0", 1871U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_MODAL_CONFLICT, err);
    return 0;
}

static int test_g53_rejects_incremental_distance_mode(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G91 G53 X1.0", 1872U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_COORDINATE, err);
    return 0;
}

static int test_unsupported_g_code_and_bad_leading_token_report_errors(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G123 X1.0", 1873U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_UNSUPPORTED, err);
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("#1=2", 1874U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_TOKEN, err);
    return 0;
}

int main(void)
{
    RUN_TEST(test_inch_mode_scales_linear_axis_to_mm_fixed_point);
    RUN_TEST(test_same_modal_group_on_one_line_reports_conflict);
    RUN_TEST(test_g53_rejects_incremental_distance_mode);
    RUN_TEST(test_unsupported_g_code_and_bad_leading_token_report_errors);
    printf("test_nc_parser_v18_modal_errors_more: PASS\n");
    return 0;
}

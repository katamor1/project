#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcParse_ResetModal();
}

static int test_percent_and_comment_only_lines_are_noop(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(1, NcParse_LineToBlock("%", 701U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_INT(1, NcParse_LineToBlock("   (header comment)   ; trailing", 702U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    return 0;
}

static int test_semicolon_stops_parsing_after_valid_tokens(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("G01 X1.000 F100 ; Y999.000 should be ignored", 703U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, b.motion_type);
    TEST_ASSERT_TRUE((b.axis_mask & 0x01U) != 0U);
    TEST_ASSERT_TRUE((b.axis_mask & 0x02U) == 0U);
    return 0;
}

static int test_missing_number_after_address_returns_token_error(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G X1.0", 704U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_TOKEN, err);
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParse_LineToBlock("G01 X", 705U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_TOKEN, err);
    return 0;
}

static int test_lowercase_addresses_and_signed_decimal_values(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcParse_LineToBlock("g01 x-1.25 f10", 706U, &b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, b.motion_type);
    TEST_ASSERT_TRUE((b.axis_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_INT(100000, b.feed);
    return 0;
}

int main(void)
{
    RUN_TEST(test_percent_and_comment_only_lines_are_noop);
    RUN_TEST(test_semicolon_stops_parsing_after_valid_tokens);
    RUN_TEST(test_missing_number_after_address_returns_token_error);
    RUN_TEST(test_lowercase_addresses_and_signed_decimal_values);
    printf("test_nc_parser_public_line_more_edges: PASS\n");
    return 0;
}

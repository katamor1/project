#include "test_common.h"
#include "nc_codes.h"
#include "nc_program.h"

static int test_gcode_decimal_and_unsupported_lookup(void)
{
    NC_G_CODE_INFO info;

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(1), NULL));
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(51, 6), &info));
    TEST_ASSERT_EQ_UINT(NC_G_CODE10(51, 6), info.code);
    TEST_ASSERT_EQ_UINT(NC_MOTION_NONE, info.motion_type);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(76, 7), &info));
    TEST_ASSERT_EQ_UINT(NC_MOTION_THREAD, info.motion_type);

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(1234), &info));
    return 0;
}

static int test_address_lookup_normalizes_case_and_rejects_unknown(void)
{
    NC_ADDRESS_INFO info;

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetAddressInfo('X', NULL));
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('x', &info));
    TEST_ASSERT_EQ_UINT('X', info.address);
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(0, info.axis_index);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('w', &info));
    TEST_ASSERT_EQ_UINT('W', info.address);
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_INCREMENTAL_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(2, info.axis_index);

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetAddressInfo('?', &info));
    return 0;
}

static int test_mcode_table_and_default_range_behavior(void)
{
    NC_M_CODE_INFO info;

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetMCodeInfo(6U, NULL));
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(6U, &info));
    TEST_ASSERT_EQ_UINT(6U, info.code);
    TEST_ASSERT_EQ_UINT(1U, info.waits_for_mfin);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(999U, &info));
    TEST_ASSERT_EQ_UINT(999U, info.code);
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_UINT(0U, info.waits_for_mfin);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(1000U, &info));
    TEST_ASSERT_EQ_UINT(1000U, info.code);
    TEST_ASSERT_EQ_UINT(0U, info.supported_v1);
    return 0;
}

int main(void)
{
    RUN_TEST(test_gcode_decimal_and_unsupported_lookup);
    RUN_TEST(test_address_lookup_normalizes_case_and_rejects_unknown);
    RUN_TEST(test_mcode_table_and_default_range_behavior);
    printf("test_nc_codes_extended: PASS\n");
    return 0;
}

#include "test_common.h"
#include "nc_codes.h"
#include "nc_program.h"

static int test_decimal_g_code_lookup_keeps_motion_categories(void)
{
    NC_G_CODE_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(76, 7), &info));
    TEST_ASSERT_EQ_INT(NC_MOTION_THREAD, info.motion_type);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(70, 7), &info));
    TEST_ASSERT_EQ_INT(NC_MOTION_CANNED_CYCLE, info.motion_type);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(51, 6), &info));
    TEST_ASSERT_EQ_INT(NC_MOTION_NONE, info.motion_type);
    return 0;
}

static int test_lowercase_axis_and_incremental_addresses_are_normalized(void)
{
    NC_ADDRESS_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('a', &info));
    TEST_ASSERT_EQ_INT(NC_ADDRESS_KIND_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(3, info.axis_index);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('u', &info));
    TEST_ASSERT_EQ_INT(NC_ADDRESS_KIND_INCREMENTAL_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(0, info.axis_index);
    return 0;
}

static int test_offset_and_arc_addresses_return_expected_kinds(void)
{
    NC_ADDRESS_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('D', &info));
    TEST_ASSERT_EQ_INT(NC_ADDRESS_KIND_OFFSET, info.kind);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('K', &info));
    TEST_ASSERT_EQ_INT(NC_ADDRESS_KIND_ARC_OFFSET, info.kind);
    TEST_ASSERT_EQ_INT(2, info.axis_index);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('R', &info));
    TEST_ASSERT_EQ_INT(NC_ADDRESS_KIND_ARC_RADIUS, info.kind);
    return 0;
}

static int test_m_code_explicit_and_fallback_bounds(void)
{
    NC_M_CODE_INFO info;
    TEST_ASSERT_EQ_INT(-1, NcCodes_GetMCodeInfo(6U, 0));
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(6U, &info));
    TEST_ASSERT_EQ_UINT(1U, info.waits_for_mfin);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(999U, &info));
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(1000U, &info));
    TEST_ASSERT_EQ_UINT(0U, info.supported_v1);
    return 0;
}

int main(void)
{
    RUN_TEST(test_decimal_g_code_lookup_keeps_motion_categories);
    RUN_TEST(test_lowercase_axis_and_incremental_addresses_are_normalized);
    RUN_TEST(test_offset_and_arc_addresses_return_expected_kinds);
    RUN_TEST(test_m_code_explicit_and_fallback_bounds);
    printf("test_nc_codes_lookup_more: PASS\n");
    return 0;
}

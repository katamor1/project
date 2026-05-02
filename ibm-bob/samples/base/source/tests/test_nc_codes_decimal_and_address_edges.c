#include "test_common.h"
#include "nc_codes.h"
#include "nc_program.h"

static int test_decimal_g_codes_keep_scaled_code_and_motion_type(void)
{
    NC_G_CODE_INFO info;

    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(50, 4), &info));
    TEST_ASSERT_EQ_INT(NC_G_CODE10(50, 4), info.code);
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_UINT(NC_MOTION_NONE, info.motion_type);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(76, 7), &info));
    TEST_ASSERT_EQ_UINT(NC_MOTION_THREAD, info.motion_type);
    return 0;
}

static int test_address_lowercase_and_incremental_axes(void)
{
    NC_ADDRESS_INFO info;

    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('u', &info));
    TEST_ASSERT_EQ_UINT('U', info.address);
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_INCREMENTAL_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(0, info.axis_index);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('w', &info));
    TEST_ASSERT_EQ_UINT('W', info.address);
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_INCREMENTAL_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(2, info.axis_index);

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetAddressInfo('[', &info));
    return 0;
}

static int test_unknown_m_code_policy_accepts_up_to_999_only(void)
{
    NC_M_CODE_INFO info;

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
    RUN_TEST(test_decimal_g_codes_keep_scaled_code_and_motion_type);
    RUN_TEST(test_address_lowercase_and_incremental_axes);
    RUN_TEST(test_unknown_m_code_policy_accepts_up_to_999_only);
    printf("test_nc_codes_decimal_and_address_edges: PASS\n");
    return 0;
}

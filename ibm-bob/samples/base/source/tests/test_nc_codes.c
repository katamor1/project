#include "test_common.h"
#include "nc_codes.h"
#include "nc_program.h"

static int test_g_code_lookup_normal_decimal_unknown_and_null(void)
{
    NC_G_CODE_INFO info;

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(1), NULL));
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(1), &info));
    TEST_ASSERT_EQ_INT(NC_G_CODE_WHOLE(1), info.code);
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, info.motion_type);
    TEST_ASSERT_TRUE((info.modal_flag & NC_MODAL_FLAG_G01) != 0U);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(76, 7), &info));
    TEST_ASSERT_EQ_UINT(NC_MOTION_THREAD, info.motion_type);

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(999), &info));
    return 0;
}

static int test_address_lookup_case_normalization_and_invalid(void)
{
    NC_ADDRESS_INFO info;

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetAddressInfo('X', NULL));

    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('x', &info));
    TEST_ASSERT_EQ_UINT((uint8_t)'X', (uint8_t)info.address);
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(0, info.axis_index);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('w', &info));
    TEST_ASSERT_EQ_UINT((uint8_t)'W', (uint8_t)info.address);
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_INCREMENTAL_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(2, info.axis_index);

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetAddressInfo('#', &info));
    return 0;
}

static int test_m_code_lookup_known_and_default_range(void)
{
    NC_M_CODE_INFO info;

    TEST_ASSERT_EQ_INT(-1, NcCodes_GetMCodeInfo(6U, NULL));

    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(6U, &info));
    TEST_ASSERT_EQ_UINT(6U, info.code);
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_UINT(1U, info.waits_for_mfin);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(777U, &info));
    TEST_ASSERT_EQ_UINT(777U, info.code);
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_UINT(0U, info.waits_for_mfin);

    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(1000U, &info));
    TEST_ASSERT_EQ_UINT(1000U, info.code);
    TEST_ASSERT_EQ_UINT(0U, info.supported_v1);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g_code_lookup_normal_decimal_unknown_and_null);
    RUN_TEST(test_address_lookup_case_normalization_and_invalid);
    RUN_TEST(test_m_code_lookup_known_and_default_range);
    printf("[  PASSED  ] test_nc_codes\n");
    return 0;
}

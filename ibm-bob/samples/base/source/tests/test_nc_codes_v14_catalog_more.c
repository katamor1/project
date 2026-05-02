#include "test_common.h"
#include "nc_codes.h"
#include "nc_program.h"

static int test_reference_and_decimal_g_code_catalog(void)
{
    NC_G_CODE_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(28), &info));
    TEST_ASSERT_EQ_UINT(NC_MOTION_REFERENCE_RETURN, info.motion_type);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(10, 6), &info));
    TEST_ASSERT_EQ_UINT(NC_MOTION_REFERENCE_RETURN, info.motion_type);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE10(73, 7), &info));
    TEST_ASSERT_EQ_UINT(NC_MOTION_CANNED_CYCLE, info.motion_type);
    return 0;
}

static int test_modal_code_flags_for_distance_and_return_modes(void)
{
    NC_G_CODE_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(90), &info));
    TEST_ASSERT_TRUE((info.modal_flag & NC_MODAL_FLAG_G90) != 0U);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(91), &info));
    TEST_ASSERT_TRUE((info.modal_flag & NC_MODAL_FLAG_G91) != 0U);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(98), &info));
    TEST_ASSERT_TRUE((info.modal_flag & NC_MODAL_FLAG_G98) != 0U);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetGCodeInfo(NC_G_CODE_WHOLE(99), &info));
    TEST_ASSERT_TRUE((info.modal_flag & NC_MODAL_FLAG_G99) != 0U);
    return 0;
}

static int test_address_catalog_for_offsets_arc_and_incremental_axes(void)
{
    NC_ADDRESS_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('d', &info));
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_OFFSET, info.kind);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('i', &info));
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_ARC_OFFSET, info.kind);
    TEST_ASSERT_EQ_INT(0, info.axis_index);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetAddressInfo('w', &info));
    TEST_ASSERT_EQ_UINT(NC_ADDRESS_KIND_INCREMENTAL_AXIS, info.kind);
    TEST_ASSERT_EQ_INT(2, info.axis_index);
    TEST_ASSERT_EQ_INT(-1, NcCodes_GetAddressInfo('@', &info));
    return 0;
}

static int test_m_code_mfin_and_unknown_boundary_policy(void)
{
    NC_M_CODE_INFO info;
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(6U, &info));
    TEST_ASSERT_EQ_UINT(1U, info.waits_for_mfin);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(999U, &info));
    TEST_ASSERT_EQ_UINT(1U, info.supported_v1);
    TEST_ASSERT_EQ_INT(0, NcCodes_GetMCodeInfo(1000U, &info));
    TEST_ASSERT_EQ_UINT(0U, info.supported_v1);
    TEST_ASSERT_EQ_INT(-1, NcCodes_GetMCodeInfo(1U, NULL));
    return 0;
}

int main(void)
{
    RUN_TEST(test_reference_and_decimal_g_code_catalog);
    RUN_TEST(test_modal_code_flags_for_distance_and_return_modes);
    RUN_TEST(test_address_catalog_for_offsets_arc_and_incremental_axes);
    RUN_TEST(test_m_code_mfin_and_unknown_boundary_policy);
    printf("test_nc_codes_v14_catalog_more: PASS\n");
    return 0;
}

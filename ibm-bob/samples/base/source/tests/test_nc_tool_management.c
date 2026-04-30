#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_tool_management.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcToolManagement_Reset();
}

static int test_set_pocket_valid_duplicate_and_invalid_cases(void)
{
    setup();
    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(5U, 10U));
    TEST_ASSERT_EQ_UINT(10U, g_ncToolManagementStatus.tool_to_pocket[5]);
    TEST_ASSERT_EQ_UINT(5U, g_ncToolManagementStatus.pocket_tool[10]);

    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(6U, 10U));
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.tool_to_pocket[5]);
    TEST_ASSERT_EQ_UINT(6U, g_ncToolManagementStatus.pocket_tool[10]);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolManagementStatus.duplicate_pocket_events);

    TEST_ASSERT_EQ_INT(-1, NcToolManagement_SetPocket(0U, 1U));
    TEST_ASSERT_EQ_INT(-1, NcToolManagement_SetPocket(2U, NC_TOOL_MAGAZINE_POCKETS));
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.invalid_tool_events >= 1U);
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.no_pocket_events >= 1U);
    return 0;
}

static int test_prepare_and_m06_without_mfin_completes_immediately(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 41U;
    b.aux_flags = NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE;
    b.tool_no = 3U;
    b.aux_m_code = 6U;

    TEST_ASSERT_EQ_INT(0, NcToolManagement_RequestPrepare(3U));
    TEST_ASSERT_EQ_UINT(3U, g_ncToolManagementStatus.prepared_tool_no);
    NcToolManagement_OnBlockRt(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_requests);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_completed);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_in_progress);
    TEST_ASSERT_EQ_UINT(3U, g_ncToolManagementStatus.active_tool_no);
    TEST_ASSERT_EQ_UINT(3U, g_ioOut.tool_no);
    return 0;
}

static int test_m06_with_mfin_wait_completes_when_sensor_arrives(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 42U;
    b.aux_flags = NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT;
    b.tool_no = 4U;
    b.aux_m_code = 6U;

    NcToolManagement_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_in_progress);
    NcToolManagement_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_wait_cycles);
    g_ioIn.seq_bits = SENSOR_SEQ_MFIN_BIT;
    NcToolManagement_ServiceRt();

    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_in_progress);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_completed);
    TEST_ASSERT_EQ_UINT(4U, g_ncToolManagementStatus.active_tool_no);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_TOOL_CHANGE_DONE, g_ncEventRing.items[g_ncEventRing.captured_count - 1U].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_set_pocket_valid_duplicate_and_invalid_cases);
    RUN_TEST(test_prepare_and_m06_without_mfin_completes_immediately);
    RUN_TEST(test_m06_with_mfin_wait_completes_when_sensor_arrives);
    printf("test_nc_tool_management: PASS\n");
    return 0;
}

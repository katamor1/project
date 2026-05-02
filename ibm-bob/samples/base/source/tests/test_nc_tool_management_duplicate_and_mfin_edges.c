#include "test_common.h"
#include "system_shared.h"
#include "nc_tool_management.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_tool_change_block(uint32_t toolNo, uint32_t lineNo, uint8_t waitMfin)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.aux_flags = NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE;
    if (waitMfin != 0U) {
        b.aux_flags |= NC_AUX_FLAG_MFIN_WAIT;
    }
    b.tool_no = toolNo;
    b.aux_m_code = 6U;
    return b;
}

static int test_set_pocket_reassigns_duplicate_pocket(void)
{
    SystemShared_Initialize();
    NcToolManagement_Reset();

    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(5U, 5U));
    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(6U, 5U));
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.tool_to_pocket[5]);
    TEST_ASSERT_EQ_UINT(5U, g_ncToolManagementStatus.tool_to_pocket[6]);
    TEST_ASSERT_EQ_UINT(6U, g_ncToolManagementStatus.pocket_tool[5]);
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.duplicate_pocket_events > 0U);
    return 0;
}

static int test_request_prepare_invalid_and_unassigned_tool_paths(void)
{
    SystemShared_Initialize();
    NcToolManagement_Reset();

    TEST_ASSERT_EQ_INT(-1, NcToolManagement_RequestPrepare(0U));
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.invalid_tool_events > 0U);

    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(7U, 0U));
    TEST_ASSERT_EQ_INT(-1, NcToolManagement_RequestPrepare(7U));
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.no_pocket_events > 0U);
    return 0;
}

static int test_mfin_wait_completes_exchange_and_records_event(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcToolManagement_Reset();

    b = make_tool_change_block(3U, 630U, 1U);
    NcToolManagement_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_in_progress);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.mfin_required);
    TEST_ASSERT_EQ_UINT(3U, g_ioOut.tool_no);

    g_ioIn.seq_bits = SENSOR_SEQ_MFIN_BIT;
    NcToolManagement_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_in_progress);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.mfin_required);
    TEST_ASSERT_EQ_UINT(3U, g_ncToolManagementStatus.active_tool_no);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_completed);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 2U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_set_pocket_reassigns_duplicate_pocket);
    RUN_TEST(test_request_prepare_invalid_and_unassigned_tool_paths);
    RUN_TEST(test_mfin_wait_completes_exchange_and_records_event);
    printf("test_nc_tool_management_duplicate_and_mfin_edges: PASS\n");
    return 0;
}

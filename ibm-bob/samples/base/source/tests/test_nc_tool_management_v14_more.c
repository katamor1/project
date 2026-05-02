#include <string.h>
#include "test_common.h"
#include "nc_tool_management.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_tool_block(uint32_t line, uint32_t toolNo, uint32_t m, uint32_t flags)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_AUX_ONLY;
    b.tool_no = toolNo;
    b.aux_m_code = m;
    b.aux_flags = flags;
    return b;
}

static int test_set_pocket_zero_unregisters_tool_and_recounts(void)
{
    SystemShared_Initialize();
    NcToolManagement_Reset();
    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(5U, 0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.tool_to_pocket[5]);
    TEST_ASSERT_EQ_INT(-1, NcToolManagement_RequestPrepare(5U));
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.no_pocket_events > 0U);
    return 0;
}

static int test_on_parsed_t_word_requests_prepare_and_tracks_line(void)
{
    NC_EXEC_BLOCK b = make_tool_block(41U, 6U, 0U, NC_AUX_FLAG_TOOL);
    SystemShared_Initialize();
    NcToolManagement_Reset();
    NcToolManagement_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.parsed_t_words);
    TEST_ASSERT_EQ_UINT(6U, g_ncToolManagementStatus.requested_tool_no);
    TEST_ASSERT_EQ_UINT(41U, g_ncToolManagementStatus.last_line_no);
    return 0;
}

static int test_m06_without_mfin_completes_immediately(void)
{
    NC_EXEC_BLOCK b = make_tool_block(42U, 7U, 6U, NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE);
    SystemShared_Initialize();
    NcToolManagement_Reset();
    NcToolManagement_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_requests);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_completed);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_in_progress);
    TEST_ASSERT_EQ_UINT(7U, g_ncToolManagementStatus.active_tool_no);
    return 0;
}

static int test_mfin_wait_counter_and_completion_max_wait(void)
{
    NC_EXEC_BLOCK b = make_tool_block(43U, 8U, 6U, NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT);
    SystemShared_Initialize();
    NcToolManagement_Reset();
    NcToolManagement_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_in_progress);
    NcToolManagement_ServiceRt();
    NcToolManagement_ServiceRt();
    TEST_ASSERT_EQ_UINT(2U, g_ncToolManagementStatus.exchange_wait_cycles);
    g_ioIn.seq_bits = SENSOR_SEQ_MFIN_BIT;
    NcToolManagement_ServiceRt();
    TEST_ASSERT_EQ_UINT(8U, g_ncToolManagementStatus.active_tool_no);
    TEST_ASSERT_EQ_UINT(2U, g_ncToolManagementStatus.max_exchange_wait_cycles);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_in_progress);
    return 0;
}

int main(void)
{
    RUN_TEST(test_set_pocket_zero_unregisters_tool_and_recounts);
    RUN_TEST(test_on_parsed_t_word_requests_prepare_and_tracks_line);
    RUN_TEST(test_m06_without_mfin_completes_immediately);
    RUN_TEST(test_mfin_wait_counter_and_completion_max_wait);
    printf("test_nc_tool_management_v14_more: PASS\n");
    return 0;
}

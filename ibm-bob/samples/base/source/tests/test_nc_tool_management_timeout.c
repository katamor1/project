#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_tool_management.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcToolManagement_Reset();
}

static NC_EXEC_BLOCK make_m06_wait(uint32_t lineNo, uint32_t toolNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_AUX_ONLY;
    b.aux_flags = NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT;
    b.tool_no = toolNo;
    b.aux_m_code = 6U;
    return b;
}

static int test_service_no_exchange_is_noop_and_invalid_prepare_is_reported(void)
{
    setup();
    NcToolManagement_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_wait_cycles);
    TEST_ASSERT_EQ_INT(-1, NcToolManagement_RequestPrepare(0U));
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.invalid_tool_events);
    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(9U, 0U));
    TEST_ASSERT_EQ_INT(-1, NcToolManagement_RequestPrepare(9U));
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.no_pocket_events >= 1U);
    return 0;
}

static int test_mfin_wait_timeout_clears_exchange_and_records_warning_event(void)
{
    NC_EXEC_BLOCK b = make_m06_wait(951U, 4U);
    uint32_t i;
    setup();
    NcToolManagement_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.exchange_in_progress);

    for (i = 0U; i <= NC_TOOL_CHANGE_TIMEOUT_TICKS; ++i) {
        NcToolManagement_ServiceRt();
    }

    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_in_progress);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.timeout_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncToolManagementStatus.exchange_completed);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_TOOL_MANAGEMENT_WARN, g_ncEventRing.items[g_ncEventRing.captured_count - 1U].code);
    return 0;
}

static int test_parsed_t_word_with_unmapped_tool_pushes_warning(void)
{
    NC_EXEC_BLOCK b;
    setup();
    TEST_ASSERT_EQ_INT(0, NcToolManagement_SetPocket(7U, 0U));
    memset(&b, 0, sizeof(b));
    b.line_no = 952U;
    b.aux_flags = NC_AUX_FLAG_TOOL;
    b.tool_no = 7U;
    NcToolManagement_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.parsed_t_words);
    TEST_ASSERT_EQ_UINT(7U, g_ncToolManagementStatus.last_t_code);
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.no_pocket_events >= 1U);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_TOOL_MANAGEMENT_WARN, g_ncEventRing.items[g_ncEventRing.captured_count - 1U].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_service_no_exchange_is_noop_and_invalid_prepare_is_reported);
    RUN_TEST(test_mfin_wait_timeout_clears_exchange_and_records_warning_event);
    RUN_TEST(test_parsed_t_word_with_unmapped_tool_pushes_warning);
    printf("test_nc_tool_management_timeout: PASS\n");
    return 0;
}

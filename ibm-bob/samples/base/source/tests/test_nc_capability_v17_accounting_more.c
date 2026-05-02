#include "test_common.h"
#include "system_shared.h"
#include "nc_capability.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.axis_mask = 0x07U;
    b.feed = 123;
    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 130U;
    b.spindle_speed = 500U;
    b.tool_no = 3U;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_P | NC_FEATURE_FLAG_PARAM_Q | NC_FEATURE_FLAG_PARAM_R;
    b.h_offset_no = 2U;
    b.d_offset_no = 4U;
    b.cycle_repeat = 2U;
    b.g_code10 = NC_G_CODE_WHOLE(81);
    return b;
}

static int test_on_parsed_block_counts_axis_and_aux_address_words(void)
{
    NC_EXEC_BLOCK b = make_block(1711U);
    SystemShared_Initialize();
    NcCapability_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_g_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_m_blocks);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('X' - 'A'))) != 0U);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('M' - 'A'))) != 0U);
    TEST_ASSERT_TRUE(g_ncCapabilityStatus.address_words_seen >= 12U);
    return 0;
}

static int test_history_wrap_keeps_last_g_and_m_codes(void)
{
    uint32_t i;
    NC_EXEC_BLOCK b = make_block(1720U);
    SystemShared_Initialize();
    for (i = 0U; i < (NC_CAPABILITY_G_HISTORY_SIZE + 3U); ++i) {
        b.g_code10 = NC_G_CODE_WHOLE(1) + i;
        b.aux_m_code = i;
        NcCapability_OnParsedBlockTs(&b);
    }
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.g_history_index);
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.m_history_index);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(1) + NC_CAPABILITY_G_HISTORY_SIZE + 2U, g_ncCapabilityStatus.last_g_code10);
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 2U, g_ncCapabilityStatus.last_m_code);
    return 0;
}

static int test_record_unsupported_updates_kind_specific_counters_and_event_ring(void)
{
    SystemShared_Initialize();
    g_ncProgramStatus.read_line_no = 77U;
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_G, NC_G_CODE_WHOLE(999));
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_M, 1001U);
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_ADDRESS, (uint32_t)('Z' - 'A'));
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_g_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_m_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_address_events);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('Z' - 'A'))) != 0U);
    TEST_ASSERT_EQ_UINT(3U, g_ncEventRing.captured_count);
    return 0;
}

static int test_reset_and_null_parsed_block_are_safe_noops(void)
{
    SystemShared_Initialize();
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_G, 1U);
    NcCapability_Reset();
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_g_events);
    NcCapability_OnParsedBlockTs(NULL);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.parsed_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_on_parsed_block_counts_axis_and_aux_address_words);
    RUN_TEST(test_history_wrap_keeps_last_g_and_m_codes);
    RUN_TEST(test_record_unsupported_updates_kind_specific_counters_and_event_ring);
    RUN_TEST(test_reset_and_null_parsed_block_are_safe_noops);
    printf("test_nc_capability_v17_accounting_more: PASS\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_capability.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_rich_block(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = line;
    b.g_code10 = NC_G_CODE_WHOLE(1);
    b.axis_mask = 0x0DU; /* X, Z, A */
    b.feed = 1200;
    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE;
    b.aux_m_code = 130U;
    b.spindle_speed = 1500U;
    b.tool_no = 4U;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_P | NC_FEATURE_FLAG_PARAM_Q | NC_FEATURE_FLAG_PARAM_R;
    b.p_word = 1U;
    b.q_value = 2;
    b.r_value = 3;
    b.h_offset_no = 5U;
    b.d_offset_no = 6U;
    b.cycle_repeat = 7U;
    return b;
}

static int test_null_block_is_noop_after_reset(void)
{
    SystemShared_Initialize();
    NcCapability_Reset();
    NcCapability_OnParsedBlockTs(0);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.parsed_blocks);
    return 0;
}

static int test_rich_block_sets_address_mask_and_supported_counters(void)
{
    NC_EXEC_BLOCK b = make_rich_block(1810U);
    SystemShared_Initialize();
    NcCapability_Reset();
    NcCapability_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_g_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_m_blocks);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('X' - 'A'))) != 0U);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('A' - 'A'))) != 0U);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('D' - 'A'))) != 0U);
    TEST_ASSERT_TRUE(g_ncCapabilityStatus.address_words_seen >= 10U);
    return 0;
}

static int test_g_and_m_history_wrap_keeps_latest_indexes(void)
{
    uint32_t i;
    SystemShared_Initialize();
    NcCapability_Reset();
    for (i = 0U; i < (NC_CAPABILITY_G_HISTORY_SIZE + 3U); ++i) {
        NC_EXEC_BLOCK b = make_rich_block(1820U + i);
        b.g_code10 = (uint16_t)NC_G_CODE_WHOLE((int32_t)((i + 1U) % 100U));
        b.aux_m_code = i;
        NcCapability_OnParsedBlockTs(&b);
    }
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.g_history_index);
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.m_history_index);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE((NC_CAPABILITY_G_HISTORY_SIZE + 3U) % 100U), g_ncCapabilityStatus.last_g_code10);
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 2U, g_ncCapabilityStatus.last_m_code);
    return 0;
}

static int test_unsupported_address_out_of_range_does_not_set_mask_bit(void)
{
    uint32_t beforeMask;
    SystemShared_Initialize();
    NcCapability_Reset();
    beforeMask = g_ncCapabilityStatus.last_address_mask;
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_ADDRESS, 30U);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_address_events);
    TEST_ASSERT_EQ_UINT(beforeMask, g_ncCapabilityStatus.last_address_mask);
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_G, 9990U);
    TEST_ASSERT_EQ_UINT(9990U, g_ncCapabilityStatus.last_g_code10);
    return 0;
}

int main(void)
{
    RUN_TEST(test_null_block_is_noop_after_reset);
    RUN_TEST(test_rich_block_sets_address_mask_and_supported_counters);
    RUN_TEST(test_g_and_m_history_wrap_keeps_latest_indexes);
    RUN_TEST(test_unsupported_address_out_of_range_does_not_set_mask_bit);
    printf("test_nc_capability_v18_address_history_more: PASS\n");
    return 0;
}

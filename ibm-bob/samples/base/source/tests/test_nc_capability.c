#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_capability.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCapability_Reset();
}

static int test_parsed_block_records_g_m_and_address_words(void)
{
    NC_EXEC_BLOCK b;
    uint32_t expectMask = 0U;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 12U;
    b.axis_mask = 0x0FU; /* X/Y/Z/A */
    b.feed = 1200;
    b.aux_flags = NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_TOOL | NC_AUX_FLAG_M_CODE;
    b.spindle_speed = 3000U;
    b.tool_no = 5U;
    b.aux_m_code = 30U;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_P | NC_FEATURE_FLAG_PARAM_Q | NC_FEATURE_FLAG_PARAM_R;
    b.h_offset_no = 1U;
    b.d_offset_no = 2U;
    b.cycle_repeat = 3U;
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(1);

    NcCapability_OnParsedBlockTs(&b);

    expectMask |= (1UL << ('X' - 'A')) | (1UL << ('Y' - 'A')) |
                  (1UL << ('Z' - 'A')) | (1UL << ('A' - 'A'));
    expectMask |= (1UL << ('F' - 'A')) | (1UL << ('S' - 'A')) |
                  (1UL << ('T' - 'A')) | (1UL << ('M' - 'A'));
    expectMask |= (1UL << ('P' - 'A')) | (1UL << ('Q' - 'A')) |
                  (1UL << ('R' - 'A')) | (1UL << ('H' - 'A')) |
                  (1UL << ('D' - 'A')) | (1UL << ('K' - 'A'));

    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_g_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_m_blocks);
    TEST_ASSERT_EQ_UINT((uint32_t)b.g_code10, g_ncCapabilityStatus.last_g_code10);
    TEST_ASSERT_EQ_UINT(30U, g_ncCapabilityStatus.last_m_code);
    TEST_ASSERT_EQ_UINT(expectMask, g_ncCapabilityStatus.last_address_mask);
    TEST_ASSERT_TRUE(g_ncCapabilityStatus.address_words_seen >= 14U);
    return 0;
}

static int test_record_unsupported_updates_counters_and_event_ring(void)
{
    setup();
    g_ncProgramStatus.read_line_no = 77U;

    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_G, NC_G_CODE_WHOLE(123));
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_M, 999U);
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_ADDRESS, (uint32_t)('W' - 'A'));

    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_g_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_m_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_address_events);
    TEST_ASSERT_EQ_UINT(3U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_CAPABILITY_UNSUPPORTED, g_ncEventRing.items[2].code);
    TEST_ASSERT_EQ_UINT(77U, g_ncEventRing.items[2].line_no);
    TEST_ASSERT_TRUE((g_ncCapabilityStatus.last_address_mask & (1UL << ('W' - 'A'))) != 0U);
    return 0;
}

static int test_null_parsed_block_is_noop(void)
{
    setup();
    NcCapability_OnParsedBlockTs(NULL);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.parsed_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_parsed_block_records_g_m_and_address_words);
    RUN_TEST(test_record_unsupported_updates_counters_and_event_ring);
    RUN_TEST(test_null_parsed_block_is_noop);
    printf("test_nc_capability: PASS\n");
    return 0;
}

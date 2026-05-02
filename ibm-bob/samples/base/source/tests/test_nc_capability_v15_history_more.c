#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_capability.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCapability_Reset();
}

static NC_EXEC_BLOCK make_block(uint32_t lineNo, uint16_t gcode, uint32_t mcode)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.g_code10 = gcode;
    if (mcode != 0U) {
        b.aux_flags = NC_AUX_FLAG_M_CODE;
        b.aux_m_code = mcode;
    }
    return b;
}

static int test_reset_clears_counters_but_advances_generation(void)
{
    uint32_t gen;
    setup();
    g_ncCapabilityStatus.parsed_blocks = 7U;
    gen = g_ncCapabilityStatus.generation;
    NcCapability_Reset();
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.generation);
    TEST_ASSERT_TRUE(gen >= 1U);
    return 0;
}

static int test_g_history_wrap_records_latest_entries(void)
{
    uint32_t i;
    setup();
    for (i = 0U; i < (NC_CAPABILITY_G_HISTORY_SIZE + 3U); ++i) {
        NC_EXEC_BLOCK b = make_block(i + 1U, (uint16_t)NC_G_CODE_WHOLE(1U + i), 0U);
        NcCapability_OnParsedBlockTs(&b);
    }
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.g_history_index);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(NC_CAPABILITY_G_HISTORY_SIZE + 3U), g_ncCapabilityStatus.last_g_code10);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(NC_CAPABILITY_G_HISTORY_SIZE + 1U), g_ncCapabilityStatus.g_history[0]);
    return 0;
}

static int test_m_history_wrap_records_latest_m_code(void)
{
    uint32_t i;
    setup();
    for (i = 0U; i < (NC_CAPABILITY_M_HISTORY_SIZE + 2U); ++i) {
        NC_EXEC_BLOCK b = make_block(i + 20U, 0U, 10U + i);
        NcCapability_OnParsedBlockTs(&b);
    }
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_M_HISTORY_SIZE + 2U, g_ncCapabilityStatus.m_history_index);
    TEST_ASSERT_EQ_UINT(10U + NC_CAPABILITY_M_HISTORY_SIZE + 1U, g_ncCapabilityStatus.last_m_code);
    TEST_ASSERT_EQ_UINT(10U + NC_CAPABILITY_M_HISTORY_SIZE, g_ncCapabilityStatus.m_history[0]);
    return 0;
}

static int test_unsupported_address_out_of_range_does_not_set_address_bit(void)
{
    setup();
    g_ncProgramStatus.read_line_no = 1234U;
    NcCapability_RecordUnsupported(NC_CAPABILITY_KIND_ADDRESS, 30U);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.unsupported_address_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.last_address_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(1234U, g_ncEventRing.items[0].line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_reset_clears_counters_but_advances_generation);
    RUN_TEST(test_g_history_wrap_records_latest_entries);
    RUN_TEST(test_m_history_wrap_records_latest_m_code);
    RUN_TEST(test_unsupported_address_out_of_range_does_not_set_address_bit);
    printf("test_nc_capability_v15_history_more: PASS\n");
    return 0;
}

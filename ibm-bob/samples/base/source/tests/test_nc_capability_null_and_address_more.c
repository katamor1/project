#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_capability.h"

#define ADDR_BIT(ch) (1UL << ((ch) - 'A'))

static void setup(void)
{
    SystemShared_Initialize();
    NcCapability_Reset();
}

static uint32_t popcount32(uint32_t value)
{
    uint32_t count = 0U;
    while (value != 0U) {
        count += (value & 1U);
        value >>= 1U;
    }
    return count;
}

static int test_null_block_is_noop_after_reset(void)
{
    uint32_t generation;
    setup();
    generation = g_ncCapabilityStatus.generation;
    NcCapability_OnParsedBlockTs(NULL);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(generation, g_ncCapabilityStatus.generation);
    TEST_ASSERT_EQ_UINT(0U, g_ncEventRing.captured_count);
    return 0;
}

static int test_axis_aux_and_parameter_words_build_address_mask(void)
{
    NC_EXEC_BLOCK b;
    uint32_t expectedMask;
    setup();
    memset(&b, 0, sizeof(b));
    b.axis_mask = 0x0FU; /* X/Y/Z/A */
    b.feed = 1000;
    b.aux_flags = NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_TOOL;
    b.aux_m_code = 6U;
    b.tool_no = 12U;
    b.spindle_speed = 1500U;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_P | NC_FEATURE_FLAG_PARAM_Q | NC_FEATURE_FLAG_PARAM_R;
    b.p_word = 1U;
    b.q_value = 2;
    b.r_value = 3;
    b.h_offset_no = 4U;
    b.d_offset_no = 5U;
    b.cycle_repeat = 6U;
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(1);

    NcCapability_OnParsedBlockTs(&b);

    expectedMask = ADDR_BIT('A') | ADDR_BIT('X') | ADDR_BIT('Y') | ADDR_BIT('Z') |
                   ADDR_BIT('F') | ADDR_BIT('M') | ADDR_BIT('S') | ADDR_BIT('T') |
                   ADDR_BIT('P') | ADDR_BIT('Q') | ADDR_BIT('R') | ADDR_BIT('H') |
                   ADDR_BIT('D') | ADDR_BIT('K');
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.parsed_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_g_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCapabilityStatus.supported_m_blocks);
    TEST_ASSERT_EQ_UINT((uint32_t)NC_G_CODE_WHOLE(1), g_ncCapabilityStatus.last_g_code10);
    TEST_ASSERT_EQ_UINT(6U, g_ncCapabilityStatus.last_m_code);
    TEST_ASSERT_EQ_UINT(expectedMask, g_ncCapabilityStatus.last_address_mask);
    TEST_ASSERT_EQ_UINT(popcount32(expectedMask), g_ncCapabilityStatus.address_words_seen);
    return 0;
}

static int test_unsupported_invalid_kind_only_records_event(void)
{
    setup();
    g_ncProgramStatus.read_line_no = 77U;
    NcCapability_RecordUnsupported(99U, 1234U);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_g_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_m_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_address_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_CAPABILITY_UNSUPPORTED, g_ncEventRing.items[0].code);
    TEST_ASSERT_EQ_INT(99, g_ncEventRing.items[0].value0);
    TEST_ASSERT_EQ_INT(1234, g_ncEventRing.items[0].value1);
    TEST_ASSERT_EQ_UINT(77U, g_ncEventRing.items[0].line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_null_block_is_noop_after_reset);
    RUN_TEST(test_axis_aux_and_parameter_words_build_address_mask);
    RUN_TEST(test_unsupported_invalid_kind_only_records_event);
    printf("test_nc_capability_null_and_address_more: PASS\n");
    return 0;
}

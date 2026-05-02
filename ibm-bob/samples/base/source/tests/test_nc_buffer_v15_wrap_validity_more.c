#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
}

static NC_EXEC_BLOCK block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = (int32_t)lineNo;
    return b;
}

static int test_count_handles_tail_before_head_wrap_case(void)
{
    setup();
    g_ncProgramBuffer.head_index = NC_MAX_LOOKAHEAD_LINES - 2U;
    g_ncProgramBuffer.tail_index = 3U;
    TEST_ASSERT_EQ_UINT(5U, NcBuffer_Count());
    return 0;
}

static int test_is_full_detects_next_tail_equal_head_at_wrap_boundary(void)
{
    setup();
    g_ncProgramBuffer.head_index = 0U;
    g_ncProgramBuffer.tail_index = NC_MAX_LOOKAHEAD_LINES - 1U;
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_IsFull());
    return 0;
}

static int test_peek_rejects_invalid_head_slot_without_consuming(void)
{
    NC_EXEC_BLOCK out;
    setup();
    g_ncProgramBuffer.head_index = 4U;
    g_ncProgramBuffer.tail_index = 5U;
    g_ncProgramBuffer.slots[4].valid = 0U;
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&out));
    TEST_ASSERT_EQ_UINT(4U, g_ncProgramBuffer.head_index);
    return 0;
}

static int test_consume_clears_valid_bit_and_advances_consumer_generation(void)
{
    NC_EXEC_BLOCK b = block(81U);
    uint32_t gen;
    setup();
    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&b));
    gen = g_ncProgramBuffer.consumer_generation;
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.slots[0].valid);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.head_index);
    TEST_ASSERT_EQ_UINT(gen + 1U, g_ncProgramBuffer.consumer_generation);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramStatus.buffered_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_count_handles_tail_before_head_wrap_case);
    RUN_TEST(test_is_full_detects_next_tail_equal_head_at_wrap_boundary);
    RUN_TEST(test_peek_rejects_invalid_head_slot_without_consuming);
    RUN_TEST(test_consume_clears_valid_bit_and_advances_consumer_generation);
    printf("test_nc_buffer_v15_wrap_validity_more: PASS\n");
    return 0;
}

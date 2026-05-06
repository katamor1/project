#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = (int32_t)lineNo;
    return b;
}

static int test_commit_and_peek_preserve_line_and_assign_generation(void)
{
    NC_EXEC_BLOCK in = make_block(1751U);
    NC_EXEC_BLOCK out;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&in));
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_Count());
    TEST_ASSERT_EQ_INT(0, NcBuffer_PeekHead(&out));
    TEST_ASSERT_EQ_UINT(1751U, out.line_no);
    TEST_ASSERT_EQ_UINT(1U, out.generation);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramStatus.buffered_blocks);
    return 0;
}

static int test_consume_empty_is_noop_and_consume_head_reduces_count(void)
{
    NC_EXEC_BLOCK in = make_block(1752U);
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&in));
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.consumer_generation);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramStatus.buffered_blocks);
    return 0;
}

static int test_wrapped_count_and_full_detection_use_head_tail_positions(void)
{
    SystemShared_Initialize();
    g_ncProgramBuffer.head_index = NC_MAX_LOOKAHEAD_LINES - 2U;
    g_ncProgramBuffer.tail_index = 3U;
    TEST_ASSERT_EQ_UINT(5U, NcBuffer_Count());
    g_ncProgramBuffer.head_index = 2U;
    g_ncProgramBuffer.tail_index = 1U;
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_IsFull());
    TEST_ASSERT_EQ_INT(-1, NcBuffer_CommitBlock(&(NC_EXEC_BLOCK){ .valid = 1U }));
    return 0;
}

static int test_peek_rejects_null_empty_and_invalid_head_slot(void)
{
    NC_EXEC_BLOCK out;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&out));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(NULL));
    g_ncProgramBuffer.tail_index = 1U;
    g_ncProgramBuffer.slots[0].valid = 0U;
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&out));
    return 0;
}

int main(void)
{
    RUN_TEST(test_commit_and_peek_preserve_line_and_assign_generation);
    RUN_TEST(test_consume_empty_is_noop_and_consume_head_reduces_count);
    RUN_TEST(test_wrapped_count_and_full_detection_use_head_tail_positions);
    RUN_TEST(test_peek_rejects_null_empty_and_invalid_head_slot);
    printf("test_nc_buffer_ring_more: PASS\n");
    return 0;
}

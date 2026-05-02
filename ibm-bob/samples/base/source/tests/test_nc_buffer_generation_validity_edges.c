#include "test_common.h"
#include "nc_program.h"

/*
 * Ring-buffer edge tests that force generation updates, manual invalid slots,
 * and index wrap without changing the production source.
 */

static NC_EXEC_BLOCK make_block(uint32_t line_no, int32_t target)
{
    NC_EXEC_BLOCK block;
    memset(&block, 0, sizeof(block));
    block.valid = 1U;
    block.line_no = line_no;
    block.motion_type = NC_MOTION_LINEAR;
    block.axis_mask = 0x01U;
    block.axis_target[0] = target;
    block.interp_ticks = 1U;
    return block;
}

static int test_null_empty_and_invalid_head_slot(void)
{
    NC_EXEC_BLOCK in = make_block(10U, 100);
    NC_EXEC_BLOCK out;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();

    TEST_ASSERT_EQ_INT(-1, NcBuffer_CommitBlock(NULL));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(NULL));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&out));
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.consumer_generation);

    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&in));
    g_ncProgramBuffer.slots[g_ncProgramBuffer.head_index].valid = 0U;
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&out));
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.consumer_generation);
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    return 0;
}

static int test_tail_and_head_wrap_generation(void)
{
    NC_EXEC_BLOCK in = make_block(99U, -500);
    NC_EXEC_BLOCK out;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();

    g_ncProgramBuffer.head_index = NC_MAX_LOOKAHEAD_LINES - 1U;
    g_ncProgramBuffer.tail_index = NC_MAX_LOOKAHEAD_LINES - 1U;

    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&in));
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.tail_index);
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.producer_generation);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.slots[NC_MAX_LOOKAHEAD_LINES - 1U].generation);

    TEST_ASSERT_EQ_INT(0, NcBuffer_PeekHead(&out));
    TEST_ASSERT_EQ_UINT(99U, out.line_no);
    TEST_ASSERT_EQ_INT(-500, out.axis_target[0]);

    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.head_index);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.consumer_generation);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramStatus.buffered_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_null_empty_and_invalid_head_slot);
    RUN_TEST(test_tail_and_head_wrap_generation);
    printf("test_nc_buffer_generation_validity_edges: PASS\n");
    return 0;
}

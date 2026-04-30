#include "test_common.h"
#include "stub_external_io.h"
#include "nc_program.h"

static int test_clear_buffer_initializes_empty_state(void)
{
    Test_ResetSystem();
    g_ncProgramBuffer.head_index = 123U;
    g_ncProgramBuffer.tail_index = 456U;
    g_ncProgramStatus.buffered_blocks = 999U;

    NcProgram_ClearBuffer();

    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.head_index);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.tail_index);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.eof_reached);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramStatus.buffered_blocks);
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_IsFull());
    return 0;
}

static int test_commit_peek_and_consume_one_block(void)
{
    NC_EXEC_BLOCK inBlock;
    NC_EXEC_BLOCK outBlock;

    Test_ResetSystem();
    inBlock = Test_MakeMotionBlock(10U, NC_MOTION_LINEAR, 0x1U, 100, 0, 0, 0);

    TEST_ASSERT_EQ_INT(-1, NcBuffer_CommitBlock(NULL));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(NULL));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&outBlock));

    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&inBlock));
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramStatus.buffered_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.producer_generation);

    memset(&outBlock, 0, sizeof(outBlock));
    TEST_ASSERT_EQ_INT(0, NcBuffer_PeekHead(&outBlock));
    TEST_ASSERT_EQ_UINT(10U, outBlock.line_no);
    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, outBlock.motion_type);
    TEST_ASSERT_EQ_INT(100, outBlock.axis_target[0]);
    TEST_ASSERT_EQ_UINT(1U, outBlock.valid);
    TEST_ASSERT_EQ_UINT(1U, outBlock.generation);

    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramStatus.buffered_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramBuffer.consumer_generation);
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&outBlock));
    return 0;
}

static int test_ring_buffer_boundary_full_and_wraparound(void)
{
    NC_EXEC_BLOCK block;
    NC_EXEC_BLOCK outBlock;
    uint32_t i;

    Test_ResetSystem();
    block = Test_MakeMotionBlock(1U, NC_MOTION_LINEAR, 0x1U, 0, 0, 0, 0);

    for (i = 0U; i < (NC_MAX_LOOKAHEAD_LINES - 1U); ++i) {
        block.line_no = i + 1U;
        TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&block));
    }
    TEST_ASSERT_EQ_UINT(NC_MAX_LOOKAHEAD_LINES - 1U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_IsFull());

    block.line_no = 999999U;
    TEST_ASSERT_EQ_INT(-1, NcBuffer_CommitBlock(&block));

    TEST_ASSERT_EQ_INT(0, NcBuffer_PeekHead(&outBlock));
    TEST_ASSERT_EQ_UINT(1U, outBlock.line_no);
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_IsFull());

    block.line_no = 1000000U;
    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&block));
    TEST_ASSERT_EQ_UINT(NC_MAX_LOOKAHEAD_LINES - 1U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_IsFull());
    return 0;
}

static int test_consume_empty_buffer_is_noop(void)
{
    Test_ResetSystem();
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.consumer_generation);
    return 0;
}

int main(void)
{
    RUN_TEST(test_clear_buffer_initializes_empty_state);
    RUN_TEST(test_commit_peek_and_consume_one_block);
    RUN_TEST(test_ring_buffer_boundary_full_and_wraparound);
    RUN_TEST(test_consume_empty_buffer_is_noop);
    printf("[  PASSED  ] test_nc_buffer\n");
    return 0;
}

#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 1U;
    b.axis_target[0] = (int32_t)line;
    return b;
}

static int test_commit_peek_consume_updates_generations_and_count(void)
{
    NC_EXEC_BLOCK in = make_block(1801U);
    NC_EXEC_BLOCK out;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&in));
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_Count());
    TEST_ASSERT_TRUE(g_ncProgramBuffer.producer_generation > 0U);
    TEST_ASSERT_EQ_INT(0, NcBuffer_PeekHead(&out));
    TEST_ASSERT_EQ_UINT(1801U, out.line_no);
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, NcBuffer_Count());
    TEST_ASSERT_TRUE(g_ncProgramBuffer.consumer_generation > 0U);
    return 0;
}

static int test_full_ring_rejects_one_extra_block(void)
{
    uint32_t i;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    for (i = 0U; i < (NC_MAX_LOOKAHEAD_LINES - 1U); ++i) {
        NC_EXEC_BLOCK b = make_block(1900U + i);
        TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&b));
    }
    TEST_ASSERT_EQ_UINT(NC_MAX_LOOKAHEAD_LINES - 1U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(1U, NcBuffer_IsFull());
    {
        NC_EXEC_BLOCK extra = make_block(1999U);
        TEST_ASSERT_EQ_INT(-1, NcBuffer_CommitBlock(&extra));
    }
    return 0;
}

static int test_null_and_empty_paths_are_rejected_without_consume_side_effect(void)
{
    NC_EXEC_BLOCK out;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    TEST_ASSERT_EQ_INT(-1, NcBuffer_CommitBlock(0));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(&out));
    TEST_ASSERT_EQ_INT(-1, NcBuffer_PeekHead(0));
    NcBuffer_ConsumeHead();
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.consumer_generation);
    return 0;
}

static int test_clear_after_wrap_resets_indexes_and_status_count(void)
{
    uint32_t i;
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    for (i = 0U; i < 3U; ++i) {
        NC_EXEC_BLOCK b = make_block(2000U + i);
        TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&b));
    }
    NcBuffer_ConsumeHead();
    NcBuffer_ConsumeHead();
    for (i = 0U; i < 4U; ++i) {
        NC_EXEC_BLOCK b = make_block(2100U + i);
        TEST_ASSERT_EQ_INT(0, NcBuffer_CommitBlock(&b));
    }
    TEST_ASSERT_TRUE(NcBuffer_Count() > 0U);
    NcProgram_ClearBuffer();
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.head_index);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramBuffer.tail_index);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramStatus.buffered_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_commit_peek_consume_updates_generations_and_count);
    RUN_TEST(test_full_ring_rejects_one_extra_block);
    RUN_TEST(test_null_and_empty_paths_are_rejected_without_consume_side_effect);
    RUN_TEST(test_clear_after_wrap_resets_indexes_and_status_count);
    printf("test_nc_buffer_generation_more: PASS\n");
    return 0;
}

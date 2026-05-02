#include "test_common.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(uint32_t line_no)
{
    NC_EXEC_BLOCK block;
    memset(&block, 0, sizeof(block));
    block.valid = 1U;
    block.line_no = line_no;
    block.motion_type = NC_MOTION_LINEAR;
    block.axis_mask = 0x01U;
    block.axis_target[0] = (int32_t)line_no;
    block.interp_ticks = 1U;
    return block;
}

static int test_binary_load_rejects_null_zero_and_oversize(void)
{
    NC_EXEC_BLOCK block = make_block(1U);
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, NcProgram_RequestBinaryLoad(NULL, 1U));
    TEST_ASSERT_EQ_INT(-1, NcProgram_RequestBinaryLoad(&block, 0U));
    TEST_ASSERT_EQ_INT(-1, NcProgram_RequestBinaryLoad(&block, NC_BINARY_PROGRAM_MAX_BLOCKS + 1U));
    TEST_ASSERT_EQ_UINT(0U, g_ncBinaryProgramStatus.loaded_blocks);
    return 0;
}

static int test_binary_load_sets_ready_status_and_buffer_copy(void)
{
    NC_EXEC_BLOCK blocks[2];
    SystemShared_Initialize();
    blocks[0] = make_block(10U);
    blocks[1] = make_block(11U);
    TEST_ASSERT_EQ_INT(0, NcProgram_RequestBinaryLoad(blocks, 2U));
    TEST_ASSERT_EQ_UINT(1U, g_ncBinaryProgramRequest.load_request);
    TEST_ASSERT_EQ_UINT(0U, g_ncBinaryProgramRequest.ready);
    TEST_ASSERT_EQ_UINT(10U, g_ncBinaryProgramRequest.blocks[0].line_no);
    TEST_ASSERT_EQ_UINT(11U, g_ncBinaryProgramRequest.blocks[1].line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_binary_load_rejects_null_zero_and_oversize);
    RUN_TEST(test_binary_load_sets_ready_status_and_buffer_copy);
    printf("test_nc_program_request_edges: PASS\n");
    return 0;
}

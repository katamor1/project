#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_lathe_cycle.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcLatheCycle_Reset();
}

static NC_EXEC_BLOCK make_block(uint32_t lineNo, uint16_t code10)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_CANNED_CYCLE;
    b.cycle_code10 = code10;
    b.axis_target[0] = 1200;
    b.axis_target[2] = -3400;
    b.feed = 2500;
    b.q_value = 321;
    b.r_value = 54;
    return b;
}

static int test_finishing_grooving_and_end_face_counters(void)
{
    NC_EXEC_BLOCK finish = make_block(241U, (uint16_t)NC_G_CODE_WHOLE(70));
    NC_EXEC_BLOCK groove1 = make_block(242U, (uint16_t)NC_G_CODE_WHOLE(74));
    NC_EXEC_BLOCK groove2 = make_block(243U, (uint16_t)NC_G_CODE_WHOLE(75));
    NC_EXEC_BLOCK endface = make_block(244U, (uint16_t)NC_G_CODE_WHOLE(94));
    setup();
    NcLatheCycle_OnParsedBlockTs(&finish);
    NcLatheCycle_OnParsedBlockTs(&groove1);
    NcLatheCycle_OnParsedBlockTs(&groove2);
    NcLatheCycle_OnParsedBlockTs(&endface);
    TEST_ASSERT_EQ_UINT(6U, g_ncTurningCycleStatus.planned_cycle_blocks);
    TEST_ASSERT_EQ_UINT(2U, g_ncTurningCycleStatus.grinding_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.finishing_blocks);
    TEST_ASSERT_EQ_UINT(2U, g_ncTurningCycleStatus.grooving_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.end_face_blocks);
    TEST_ASSERT_EQ_UINT(244U, g_ncTurningCycleStatus.last_line_no);
    return 0;
}

static int test_circular_and_variable_lead_thread_flags(void)
{
    NC_EXEC_BLOCK b = make_block(245U, (uint16_t)NC_G_CODE_WHOLE(32));
    setup();
    b.modal_flags = NC_ARC_FLAG_IJK;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_Q;
    NcLatheCycle_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.g32_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.circular_thread_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.variable_lead_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.spindle_sync_required);
    return 0;
}

static int test_thread_feature_flag_without_gcode_is_still_tracked(void)
{
    NC_EXEC_BLOCK b = make_block(246U, 0U);
    setup();
    b.feature_flags = NC_FEATURE_FLAG_THREAD;
    NcLatheCycle_OnParsedBlockTs(&b);
    NcLatheCycle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.planned_thread_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.executed_thread_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.active);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 1U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_finishing_grooving_and_end_face_counters);
    RUN_TEST(test_circular_and_variable_lead_thread_flags);
    RUN_TEST(test_thread_feature_flag_without_gcode_is_still_tracked);
    printf("test_nc_lathe_cycle_variants: PASS\n");
    return 0;
}

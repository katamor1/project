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
    b.axis_target[0] = 1000;
    b.axis_target[2] = -2000;
    b.feed = 1500;
    b.q_value = 123;
    b.r_value = 45;
    return b;
}

static int test_diameter_mode_toggle(void)
{
    setup();
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.radius_mode_active);
    TEST_ASSERT_EQ_INT(0, NcLatheCycle_SetDiameterMode(1U));
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.diameter_mode_active);
    TEST_ASSERT_EQ_UINT(0U, g_ncTurningCycleStatus.radius_mode_active);
    TEST_ASSERT_EQ_INT(0, NcLatheCycle_SetDiameterMode(0U));
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.radius_mode_active);
    return 0;
}

static int test_turning_and_grinding_planning_counters(void)
{
    NC_EXEC_BLOCK rough = make_block(111U, (uint16_t)NC_G_CODE_WHOLE(71));
    NC_EXEC_BLOCK grind = make_block(112U, (uint16_t)NC_G_CODE10(70, 7));
    setup();
    NcLatheCycle_OnParsedBlockTs(&rough);
    NcLatheCycle_OnParsedBlockTs(&grind);
    TEST_ASSERT_EQ_UINT(3U, g_ncTurningCycleStatus.planned_cycle_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.roughing_blocks);
    TEST_ASSERT_EQ_UINT(2U, g_ncTurningCycleStatus.grinding_blocks);
    TEST_ASSERT_EQ_INT(123, g_ncTurningCycleStatus.last_depth);
    return 0;
}

static int test_thread_cycle_parsed_and_rt_events(void)
{
    NC_EXEC_BLOCK thread = make_block(113U, (uint16_t)NC_G_CODE_WHOLE(76));
    setup();
    thread.feature_flags = NC_FEATURE_FLAG_PARAM_Q;
    NcLatheCycle_OnParsedBlockTs(&thread);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.planned_thread_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.g76_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.variable_lead_blocks);

    NcLatheCycle_OnBlockRt(&thread);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.executed_thread_blocks);
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 2U);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_THREAD_CYCLE_EXECUTED, g_ncEventRing.items[1].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_diameter_mode_toggle);
    RUN_TEST(test_turning_and_grinding_planning_counters);
    RUN_TEST(test_thread_cycle_parsed_and_rt_events);
    printf("test_nc_lathe_cycle: PASS\n");
    return 0;
}

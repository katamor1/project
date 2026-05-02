#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_lathe_cycle.h"

#define G(w) NC_G_CODE_WHOLE(w)
#define GD(w,d) NC_G_CODE10((w),(d))

static void setup(void)
{
    SystemShared_Initialize();
    NcLatheCycle_Reset();
}

static NC_EXEC_BLOCK cycle_block(uint32_t lineNo, uint16_t code)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.g_code10 = code;
    b.q_value = 123;
    b.r_value = 45;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 2000;
    return b;
}

static int test_reset_defaults_to_radius_mode(void)
{
    setup();
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.radius_mode_active);
    TEST_ASSERT_EQ_UINT(0U, g_ncTurningCycleStatus.diameter_mode_active);
    return 0;
}

static int test_turning_cycle_g70_and_g94_update_separate_counters(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = cycle_block(51U, (uint16_t)G(70));
    NcLatheCycle_OnParsedBlockTs(&b);
    b = cycle_block(52U, (uint16_t)G(94));
    NcLatheCycle_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(2U, g_ncTurningCycleStatus.planned_cycle_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.finishing_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.end_face_blocks);
    TEST_ASSERT_EQ_UINT(52U, g_ncTurningCycleStatus.last_line_no);
    return 0;
}

static int test_grinding_decimal_cycle_updates_grinding_status(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = cycle_block(53U, (uint16_t)GD(70, 7));
    NcLatheCycle_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.grinding_blocks);
    TEST_ASSERT_EQ_UINT(GD(70, 7), g_ncTurningCycleStatus.last_cycle_code10);
    TEST_ASSERT_EQ_INT(123, g_ncTurningCycleStatus.last_depth);
    return 0;
}

static int test_thread_cycle_feature_flag_counts_variable_and_circular_thread(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = cycle_block(54U, (uint16_t)G(32));
    b.feed = 2500;
    b.modal_flags = NC_ARC_FLAG_IJK;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_Q;
    NcLatheCycle_OnParsedBlockTs(&b);
    NcLatheCycle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.g32_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.variable_lead_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.circular_thread_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncThreadCycleStatus.executed_thread_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_reset_defaults_to_radius_mode);
    RUN_TEST(test_turning_cycle_g70_and_g94_update_separate_counters);
    RUN_TEST(test_grinding_decimal_cycle_updates_grinding_status);
    RUN_TEST(test_thread_cycle_feature_flag_counts_variable_and_circular_thread);
    printf("test_nc_lathe_cycle_v15_tracking_more: PASS\n");
    return 0;
}

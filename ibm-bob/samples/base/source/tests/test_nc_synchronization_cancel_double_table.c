#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_synchronization.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcSynchronization_Reset();
}

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x03U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 10000;
    b.axis_start[1] = 0;
    b.axis_target[1] = 15000;
    return b;
}

static int test_mode_cancel_updates_bits_and_cancel_counter(void)
{
    NC_EXEC_BLOCK mode;
    setup();
    memset(&mode, 0, sizeof(mode));
    mode.line_no = 231U;
    mode.g_code10 = (uint16_t)NC_G_CODE10(51, 4);
    NcSynchronization_ApplyBlockRt(&mode);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.sync_enabled);

    mode.g_code10 = (uint16_t)NC_G_CODE10(50, 4);
    NcSynchronization_ApplyBlockRt(&mode);
    TEST_ASSERT_EQ_UINT(0U, g_ncSynchronizationStatus.sync_enabled);
    TEST_ASSERT_TRUE(g_ncSynchronizationStatus.mode_cancel_events >= 1U);
    TEST_ASSERT_EQ_UINT(0U, (g_ncSynchronizationStatus.active_mode_bits & 0x0001U));
    return 0;
}

static int test_double_table_applies_master_slave_without_sync_gcode(void)
{
    NC_EXEC_BLOCK b = make_linear(232U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x02U));
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetDoubleTable(1U, 0x02U));
    b.axis_mask = 0x01U;
    b.axis_start[1] = 100;

    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_TRUE((b.axis_mask & 0x02U) != 0U);
    TEST_ASSERT_EQ_INT(10100, b.axis_target[1]);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.double_table_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.synchronized_motion_blocks);
    return 0;
}

static int test_following_error_warning_when_slave_position_disagrees(void)
{
    NC_EXEC_BLOCK mode;
    NC_EXEC_BLOCK b = make_linear(233U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x02U));
    memset(&mode, 0, sizeof(mode));
    mode.g_code10 = (uint16_t)NC_G_CODE10(51, 4);
    NcSynchronization_ApplyBlockRt(&mode);

    b.axis_mask = 0x03U;
    b.axis_start[1] = 0;
    b.axis_target[1] = NC_SYNC_FOLLOWING_WARN_THRESHOLD + 5000;
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_TRUE(g_ncSynchronizationStatus.following_warn_events >= 1U);
    TEST_ASSERT_TRUE((g_ncSynchronizationStatus.max_following_error[1] > NC_SYNC_FOLLOWING_WARN_THRESHOLD) || (g_ncSynchronizationStatus.max_following_error[1] < -NC_SYNC_FOLLOWING_WARN_THRESHOLD));
    TEST_ASSERT_TRUE(g_ncEventRing.captured_count >= 1U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_mode_cancel_updates_bits_and_cancel_counter);
    RUN_TEST(test_double_table_applies_master_slave_without_sync_gcode);
    RUN_TEST(test_following_error_warning_when_slave_position_disagrees);
    printf("test_nc_synchronization_cancel_double_table: PASS\n");
    return 0;
}

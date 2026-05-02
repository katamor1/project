#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_synchronization.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcSynchronization_Reset();
}

static NC_EXEC_BLOCK block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 1000;
    return b;
}

static int test_on_parsed_counts_only_mode_commands(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = block(31U);
    b.g_code10 = (uint16_t)NC_G_CODE10(51, 4);
    NcSynchronization_OnParsedBlockTs(&b);
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(1);
    NcSynchronization_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.planned_mode_blocks);
    TEST_ASSERT_EQ_UINT(NC_G_CODE10(51, 4), g_ncSynchronizationStatus.last_mode_code10);
    return 0;
}

static int test_mode_switch_and_cancel_update_mode_bits_and_events(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = block(32U);
    b.axis_mask = 0U;
    b.g_code10 = (uint16_t)NC_G_CODE10(51, 4);
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.sync_enabled);
    TEST_ASSERT_TRUE(g_ncSynchronizationStatus.active_mode_bits != 0U);
    b.g_code10 = (uint16_t)NC_G_CODE10(50, 4);
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncSynchronizationStatus.sync_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.mode_switch_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.mode_cancel_events);
    return 0;
}

static int test_slave_following_error_warns_when_slave_target_deviates(void)
{
    NC_EXEC_BLOCK mode;
    NC_EXEC_BLOCK b;
    setup();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x02U));
    memset(&mode, 0, sizeof(mode));
    mode.g_code10 = (uint16_t)NC_G_CODE10(51, 4);
    NcSynchronization_ApplyBlockRt(&mode);
    b = block(33U);
    b.axis_mask = 0x03U;
    b.axis_start[1] = 0;
    b.axis_target[1] = NC_SYNC_FOLLOWING_WARN_THRESHOLD + 5000;
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_TRUE(g_ncSynchronizationStatus.following_warn_events >= 1U);
    TEST_ASSERT_EQ_INT((int32_t)(NC_SYNC_FOLLOWING_WARN_THRESHOLD + 4000), g_ncSynchronizationStatus.last_following_error[1]);
    return 0;
}

static int test_double_table_counts_motion_without_sync_mode_command(void)
{
    NC_EXEC_BLOCK b;
    setup();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetDoubleTable(1U, 0x02U));
    b = block(34U);
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.double_table_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.double_table_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_on_parsed_counts_only_mode_commands);
    RUN_TEST(test_mode_switch_and_cancel_update_mode_bits_and_events);
    RUN_TEST(test_slave_following_error_warns_when_slave_target_deviates);
    RUN_TEST(test_double_table_counts_motion_without_sync_mode_command);
    printf("test_nc_synchronization_v15_modes_following_more: PASS\n");
    return 0;
}

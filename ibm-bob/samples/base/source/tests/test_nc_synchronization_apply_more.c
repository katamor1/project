#include "test_common.h"
#include "system_shared.h"
#include "nc_synchronization.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 100;
    b.axis_target[0] = 600;
    b.axis_start[1] = 1000;
    b.axis_target[1] = 1000;
    return b;
}

static int test_master_slave_setter_masks_master_and_rejects_empty_slave(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, NcSynchronization_SetMasterSlave(AXIS_MAX, 0x01U));
    TEST_ASSERT_EQ_INT(-1, NcSynchronization_SetMasterSlave(0U, 0x01U));
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x03U));
    TEST_ASSERT_EQ_UINT(0U, g_ncSynchronizationStatus.master_axis);
    TEST_ASSERT_EQ_UINT(0x02U, g_ncSynchronizationStatus.slave_axis_mask);
    return 0;
}

static int test_mode_command_enables_and_cancels_sync_mode(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    memset(&b, 0, sizeof(b));
    b.line_no = 1791U;
    b.g_code10 = NC_G_CODE10(51, 4);
    NcSynchronization_OnParsedBlockTs(&b);
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.sync_enabled);
    TEST_ASSERT_TRUE((g_ncSynchronizationStatus.active_mode_bits & 0x0001U) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.mode_switch_events);
    b.g_code10 = NC_G_CODE10(50, 4);
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncSynchronizationStatus.sync_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.mode_cancel_events);
    return 0;
}

static int test_master_slave_applies_missing_slave_target_and_tracks_delta(void)
{
    NC_EXEC_BLOCK b = make_motion(1792U);
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x02U));
    g_ncSynchronizationStatus.sync_enabled = 1U;
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_TRUE((b.axis_mask & 0x02U) != 0U);
    TEST_ASSERT_EQ_INT(1500, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(500, g_ncSynchronizationStatus.last_master_delta);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.synchronized_motion_blocks);
    return 0;
}

static int test_overlay_and_compound_tracking_apply_to_motion_block(void)
{
    NC_EXEC_BLOCK b = make_motion(1793U);
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetOverlayAxis(2U, 250));
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetCompoundPathMask(0x05U));
    g_ncSynchronizationStatus.overlay_enabled = 1U;
    g_ncSynchronizationStatus.compound_enabled = 1U;
    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_TRUE((b.axis_mask & 0x04U) != 0U);
    TEST_ASSERT_EQ_INT(250, b.axis_target[2]);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.overlay_motion_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.compound_motion_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_master_slave_setter_masks_master_and_rejects_empty_slave);
    RUN_TEST(test_mode_command_enables_and_cancels_sync_mode);
    RUN_TEST(test_master_slave_applies_missing_slave_target_and_tracks_delta);
    RUN_TEST(test_overlay_and_compound_tracking_apply_to_motion_block);
    printf("test_nc_synchronization_apply_more: PASS\n");
    return 0;
}

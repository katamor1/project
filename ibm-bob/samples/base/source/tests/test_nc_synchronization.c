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
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 10000;
    return b;
}

static int test_setters_validate_and_mask_inputs(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, NcSynchronization_SetMasterSlave(AXIS_MAX, 0x01U));
    TEST_ASSERT_EQ_INT(-1, NcSynchronization_SetMasterSlave(0U, 0x01U));
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x02U));
    TEST_ASSERT_EQ_UINT(0U, g_ncSynchronizationStatus.master_axis);
    TEST_ASSERT_EQ_UINT(0x02U, g_ncSynchronizationStatus.slave_axis_mask);

    TEST_ASSERT_EQ_INT(-1, NcSynchronization_SetOverlayAxis(AXIS_MAX, 100));
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetOverlayAxis(2U, 1234));
    TEST_ASSERT_TRUE((g_ncSynchronizationStatus.overlay_axis_mask & 0x04U) != 0U);
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetCompoundPathMask(0xffffffffUL));
    TEST_ASSERT_EQ_UINT(((1UL << AXIS_MAX) - 1UL), g_ncSynchronizationStatus.compound_path_mask);
    return 0;
}

static int test_sync_mode_applies_master_delta_to_slave_axis(void)
{
    NC_EXEC_BLOCK mode;
    NC_EXEC_BLOCK b = make_linear(71U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcSynchronization_SetMasterSlave(0U, 0x02U));
    memset(&mode, 0, sizeof(mode));
    mode.line_no = 70U;
    mode.g_code10 = (uint16_t)NC_G_CODE10(51, 4);
    NcSynchronization_ApplyBlockRt(&mode);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.sync_enabled);

    b.axis_start[1] = 100;
    NcSynchronization_ApplyBlockRt(&b);

    TEST_ASSERT_TRUE((b.axis_mask & 0x02U) != 0U);
    TEST_ASSERT_EQ_INT(10100, b.axis_target[1]);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.synchronized_motion_blocks);
    return 0;
}

static int test_overlay_and_compound_counters(void)
{
    NC_EXEC_BLOCK mode;
    NC_EXEC_BLOCK b = make_linear(72U);
    setup();
    (void)NcSynchronization_SetOverlayAxis(0U, 500);
    (void)NcSynchronization_SetCompoundPathMask(0x01U);
    memset(&mode, 0, sizeof(mode));
    mode.g_code10 = (uint16_t)NC_G_CODE10(51, 6);
    NcSynchronization_ApplyBlockRt(&mode);
    memset(&mode, 0, sizeof(mode));
    mode.g_code10 = (uint16_t)NC_G_CODE10(51, 5);
    NcSynchronization_ApplyBlockRt(&mode);

    NcSynchronization_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_INT(10500, b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.overlay_motion_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncSynchronizationStatus.compound_motion_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_setters_validate_and_mask_inputs);
    RUN_TEST(test_sync_mode_applies_master_delta_to_slave_axis);
    RUN_TEST(test_overlay_and_compound_counters);
    printf("test_nc_synchronization: PASS\n");
    return 0;
}

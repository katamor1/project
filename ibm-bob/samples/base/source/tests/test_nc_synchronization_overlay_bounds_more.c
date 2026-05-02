#include "test_common.h"
#include "system_shared.h"
#include "nc_synchronization.h"
static int test_overlay_axis_rejects_out_of_range(void){SystemShared_Initialize();NcSynchronization_Reset();TEST_ASSERT_EQ_INT(-1,NcSynchronization_SetOverlayAxis(AXIS_MAX,10));return 0;}
static int test_overlay_axis_sets_mask_and_offset(void){SystemShared_Initialize();NcSynchronization_Reset();TEST_ASSERT_EQ_INT(0,NcSynchronization_SetOverlayAxis(2U,-55));TEST_ASSERT_TRUE((g_ncSynchronizationStatus.overlay_axis_mask & (1UL<<2U))!=0U);TEST_ASSERT_EQ_INT(-55,g_ncSynchronizationStatus.overlay_offset[2]);return 0;}
int main(void){RUN_TEST(test_overlay_axis_rejects_out_of_range);RUN_TEST(test_overlay_axis_sets_mask_and_offset);return 0;}

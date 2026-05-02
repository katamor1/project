#include "test_common.h"
#include "system_shared.h"
#include "nc_interference.h"
static int test_enabled_axis_limit_sets_warning_and_slowdown(void){
    NC_EXEC_BLOCK b; SystemShared_Initialize(); NcInterference_Reset(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0,NcInterference_SetEnabled(1U));
    b.motion_type=NC_MOTION_LINEAR; b.axis_mask=1U; b.axis_target[0]=NC_INTERFERENCE_AXIS_LIMIT + 1; b.line_no=9; b.feed_override_percent=100U;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U,g_ncInterferenceStatus.axis_limit_warnings);
    TEST_ASSERT_EQ_UINT(1U,g_ncInterferenceStatus.slowdown_applied_blocks);
    TEST_ASSERT_TRUE(b.feed_override_percent < 100U);
    return 0;
}
int main(void){RUN_TEST(test_enabled_axis_limit_sets_warning_and_slowdown);return 0;}

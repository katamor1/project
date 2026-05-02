#include "test_common.h"
#include "system_shared.h"
#include "nc_feed.h"
extern int32_t NcFeed_PrepareMotionTs(NC_EXEC_BLOCK* pBlock, double pathLengthUnits, NC_ERROR_CODE* pOutError);
static int test_zero_override_prepares_hold_like_min_ticks(void){
    NC_EXEC_BLOCK b; NC_ERROR_CODE e=NC_ERR_NONE; SystemShared_Initialize(); NcFeed_Reset(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0,NcFeed_SetOverride(0U));
    b.motion_type=NC_MOTION_LINEAR; b.feed=1000;
    TEST_ASSERT_EQ_INT(0,NcFeed_PrepareMotionTs(&b,100.0,&e));
    TEST_ASSERT_EQ_UINT(NC_INTERP_MIN_TICKS,b.interp_ticks);
    TEST_ASSERT_EQ_UINT(0U,b.feed_override_percent);
    return 0;
}
int main(void){RUN_TEST(test_zero_override_prepares_hold_like_min_ticks);return 0;}

#include "test_common.h"
#include "system_shared.h"
#include "nc_feed.h"
extern int32_t NcFeed_PrepareDwellTs(NC_EXEC_BLOCK* pBlock, uint32_t dwellTicks, NC_ERROR_CODE* pOutError);
static int test_zero_dwell_becomes_one_tick(void){NC_EXEC_BLOCK b; NC_ERROR_CODE e=NC_ERR_NONE; SystemShared_Initialize();NcFeed_Reset();memset(&b,0,sizeof(b));TEST_ASSERT_EQ_INT(0,NcFeed_PrepareDwellTs(&b,0U,&e));TEST_ASSERT_EQ_UINT(1U,b.interp_ticks);TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_DWELL,b.feed_profile);return 0;}
static int test_too_large_dwell_sets_range_error(void){NC_EXEC_BLOCK b; NC_ERROR_CODE e=NC_ERR_NONE; memset(&b,0,sizeof(b));TEST_ASSERT_EQ_INT(-1,NcFeed_PrepareDwellTs(&b,NC_DWELL_MAX_TICKS+1U,&e));TEST_ASSERT_EQ_UINT(NC_ERR_RANGE,e);return 0;}
int main(void){RUN_TEST(test_zero_dwell_becomes_one_tick);RUN_TEST(test_too_large_dwell_sets_range_error);return 0;}

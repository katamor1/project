#include "test_common.h"
#include "system_shared.h"
#include "nc_cycle.h"
#include "nc_program.h"
extern int32_t NcCycle_BuildSegmentPlanRt(const NC_EXEC_BLOCK* pBlock, NC_ACTIVE_SEGMENT* pSegments, uint32_t* pSegmentCount);
static int test_segment_builder_rejects_nulls(void){NC_ACTIVE_SEGMENT segs[4]; uint32_t n=0; SystemShared_Initialize();TEST_ASSERT_EQ_INT(-1,NcCycle_BuildSegmentPlanRt(0,segs,&n));return 0;}
static int test_repeat_word_rounds_to_nearest(void){NC_EXEC_BLOCK b; memset(&b,0,sizeof(b));TEST_ASSERT_EQ_INT(0,NcCycle_SetRepeatWord(2.4,&b));TEST_ASSERT_EQ_UINT(2U,b.cycle_repeat);return 0;}
int main(void){RUN_TEST(test_segment_builder_rejects_nulls);RUN_TEST(test_repeat_word_rounds_to_nearest);return 0;}

#include "test_common.h"
#include "system_shared.h"
#include "nc_cycle.h"
#include "nc_program.h"
extern int32_t NcCycle_FinalizeParserBlock(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
static int test_modal_cycle_is_replayed_for_next_cycle_block(void){
    NC_EXEC_BLOCK first,next; NC_ERROR_CODE e=NC_ERR_NONE; SystemShared_Initialize(); NcCycle_ResetParserModal(); memset(&first,0,sizeof(first)); memset(&next,0,sizeof(next));
    first.motion_type=NC_MOTION_CANNED_CYCLE; NcCycle_ApplyCycleGCode(NC_G_CODE_WHOLE(81), &first); first.r_value=100; first.q_value=5; first.p_word=3; first.feed=1000; first.axis_mask=(1UL<<2U); first.feature_flags|=NC_FEATURE_FLAG_PARAM_R;
    TEST_ASSERT_EQ_INT(0,NcCycle_FinalizeParserBlock(&first,&e));
    next.motion_type=NC_MOTION_CANNED_CYCLE; NcCycle_InitParserBlock(&next);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(81), next.cycle_code10);
    TEST_ASSERT_TRUE((next.cycle_flags & NC_CYCLE_FLAG_MODAL_REPLAY)!=0U);
    TEST_ASSERT_EQ_INT(100,next.r_value);
    return 0;
}
int main(void){RUN_TEST(test_modal_cycle_is_replayed_for_next_cycle_block);return 0;}

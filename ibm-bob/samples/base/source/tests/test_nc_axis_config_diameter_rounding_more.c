#include "test_common.h"
#include "system_shared.h"
#include "nc_axis_config.h"
extern int32_t NcAxisConfig_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
static int test_positive_and_negative_diameter_half_away_from_zero(void){
    NC_EXEC_BLOCK b; NC_ERROR_CODE e=NC_ERR_NONE; SystemShared_Initialize(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetDiameterMode(0U,1U));
    b.motion_type=NC_MOTION_LINEAR; b.axis_mask=1U; b.axis_start[0]=5; b.axis_target[0]=-5; b.arc_center[0]=3;
    TEST_ASSERT_EQ_INT(0,NcAxisConfig_ApplyBlockTs(&b,&e));
    TEST_ASSERT_EQ_INT(3,b.axis_start[0]);
    TEST_ASSERT_EQ_INT(-3,b.axis_target[0]);
    TEST_ASSERT_EQ_INT(2,b.arc_center[0]);
    TEST_ASSERT_EQ_UINT(1U,g_ncAxisConfigStatus.diameter_converted_blocks);
    return 0;
}
int main(void){RUN_TEST(test_positive_and_negative_diameter_half_away_from_zero);return 0;}

#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate_transform.h"
extern int32_t NcCoordinateTransform_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
static int test_three_corrections_are_summed_on_axis(void){
    NC_EXEC_BLOCK b; NC_ERROR_CODE e=NC_ERR_NONE; SystemShared_Initialize(); NcCoordinateTransform_Reset(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0,NcCoordinateTransform_SetEnabled(1U,1U,1U));
    TEST_ASSERT_EQ_INT(0,NcCoordinateTransform_SetDynamicFixtureOffset(0U,10));
    TEST_ASSERT_EQ_INT(0,NcCoordinateTransform_SetWorkMountError(0U,20));
    TEST_ASSERT_EQ_INT(0,NcCoordinateTransform_SetRotaryTableOffset(0U,30));
    b.motion_type=NC_MOTION_LINEAR; b.axis_mask=1U; b.axis_start[0]=100; b.axis_target[0]=200; b.line_no=77;
    TEST_ASSERT_EQ_INT(0,NcCoordinateTransform_ApplyBlockTs(&b,&e));
    TEST_ASSERT_EQ_INT(160,b.axis_start[0]);
    TEST_ASSERT_EQ_INT(260,b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(1U,g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_UINT(77U,g_ncCoordinateTransformStatus.last_line_no);
    return 0;
}
int main(void){RUN_TEST(test_three_corrections_are_summed_on_axis);return 0;}

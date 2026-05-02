#include "test_common.h"
#include "system_shared.h"
#include "nc_axis_config.h"
extern int32_t NcAxisConfig_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
static int test_path_filter_removes_non_path_axis(void) {
    NC_EXEC_BLOCK b; NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetPathAxisMask(0x3U));
    b.motion_type = NC_MOTION_LINEAR; b.axis_mask = 0x7U;
    b.axis_start[2] = 123; b.axis_target[2] = 999;
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_ApplyBlockTs(&b, &err));
    TEST_ASSERT_TRUE((b.axis_mask & (1UL<<2U)) == 0U);
    TEST_ASSERT_EQ_INT(123, b.axis_target[2]);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.path_filtered_blocks);
    return 0;
}
static int test_null_arguments_are_rejected(void){NC_ERROR_CODE e=NC_ERR_NONE;SystemShared_Initialize();TEST_ASSERT_EQ_INT(-1,NcAxisConfig_ApplyBlockTs(0,&e));return 0;}
int main(void){RUN_TEST(test_path_filter_removes_non_path_axis);RUN_TEST(test_null_arguments_are_rejected);return 0;}

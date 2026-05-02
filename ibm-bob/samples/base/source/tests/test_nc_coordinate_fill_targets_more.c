#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"
extern int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex, int32_t programPosition, NC_EXEC_BLOCK* pBlock);
static int test_fill_targets_backfills_unspecified_axes(void){
    NC_EXEC_BLOCK b; SystemShared_Initialize(); NcCoordinate_Reset(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0,NcCoordinate_SetProgramAxis(0,1000,&b));
    g_ncCoordinateState.machine_position[1]=222;
    b.axis_mask = 1U;
    NcCoordinate_FillTargets(&b);
    TEST_ASSERT_EQ_INT(222,b.axis_target[1]);
    TEST_ASSERT_EQ_INT(1000,b.axis_target[0]);
    return 0;
}
static int test_fill_targets_null_is_noop(void){SystemShared_Initialize();NcCoordinate_FillTargets(0);return 0;}
int main(void){RUN_TEST(test_fill_targets_backfills_unspecified_axes);RUN_TEST(test_fill_targets_null_is_noop);return 0;}

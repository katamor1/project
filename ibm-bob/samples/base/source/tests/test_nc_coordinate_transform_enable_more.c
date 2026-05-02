#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate_transform.h"
static int test_enable_flags_update_active_mask(void){
    SystemShared_Initialize(); NcCoordinateTransform_Reset();
    TEST_ASSERT_EQ_INT(0,NcCoordinateTransform_SetEnabled(0U,1U,0U));
    TEST_ASSERT_EQ_UINT(0U,g_ncCoordinateTransformStatus.dynamic_fixture_enabled);
    TEST_ASSERT_EQ_UINT(1U,g_ncCoordinateTransformStatus.work_mount_error_enabled);
    TEST_ASSERT_EQ_UINT(0U,g_ncCoordinateTransformStatus.rotary_fixture_enabled);
    return 0;
}
static int test_invalid_dynamic_fixture_axis_rejected(void){SystemShared_Initialize();NcCoordinateTransform_Reset();TEST_ASSERT_EQ_INT(-1,NcCoordinateTransform_SetDynamicFixtureOffset(AXIS_MAX,10));TEST_ASSERT_EQ_UINT(1U,g_ncCoordinateTransformStatus.api_rejected_calls);return 0;}
int main(void){RUN_TEST(test_enable_flags_update_active_mask);RUN_TEST(test_invalid_dynamic_fixture_axis_rejected);return 0;}

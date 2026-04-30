#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_interp_math.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static int test_plane_axes_mapping(void)
{
    uint32_t a = 99U, b = 99U;
    setup();
    NcInterp_PlaneAxes(NC_PLANE_XY, &a, &b);
    TEST_ASSERT_EQ_UINT(0U, a);
    TEST_ASSERT_EQ_UINT(1U, b);
    NcInterp_PlaneAxes(NC_PLANE_ZX, &a, &b);
    TEST_ASSERT_EQ_UINT(2U, a);
    TEST_ASSERT_EQ_UINT(0U, b);
    NcInterp_PlaneAxes(NC_PLANE_YZ, &a, &b);
    TEST_ASSERT_EQ_UINT(1U, a);
    TEST_ASSERT_EQ_UINT(2U, b);
    return 0;
}

static int test_prepare_arc_rejects_conflicting_or_missing_geometry(void)
{
    NC_EXEC_BLOCK arc;
    double path = 0.0;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    memset(&arc, 0, sizeof(arc));
    arc.motion_type = NC_MOTION_ARC_CW;
    arc.plane = NC_PLANE_XY;
    arc.modal_flags = NC_ARC_FLAG_IJK | NC_ARC_FLAG_R;
    TEST_ASSERT_EQ_INT(-1, NcInterp_PrepareArcTs(&arc, &path, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_ARC_GEOMETRY, err);

    memset(&arc, 0, sizeof(arc));
    err = NC_ERR_NONE;
    arc.motion_type = NC_MOTION_ARC_CW;
    arc.plane = NC_PLANE_XY;
    TEST_ASSERT_EQ_INT(-1, NcInterp_PrepareArcTs(&arc, &path, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_ARC_GEOMETRY, err);
    return 0;
}

static int test_prepare_arc_with_ijk_calculates_radius_and_length(void)
{
    NC_EXEC_BLOCK arc;
    double path = 0.0;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    memset(&arc, 0, sizeof(arc));
    arc.motion_type = NC_MOTION_ARC_CCW;
    arc.plane = NC_PLANE_XY;
    arc.modal_flags = NC_ARC_FLAG_IJK;
    arc.axis_start[0] = 10000;
    arc.axis_start[1] = 0;
    arc.axis_target[0] = 0;
    arc.axis_target[1] = 10000;
    arc.arc_center[0] = 0;
    arc.arc_center[1] = 0;

    TEST_ASSERT_EQ_INT(0, NcInterp_PrepareArcTs(&arc, &path, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_TRUE(arc.arc_radius >= 9990 && arc.arc_radius <= 10010);
    TEST_ASSERT_TRUE(path > 1.0);
    return 0;
}

int main(void)
{
    RUN_TEST(test_plane_axes_mapping);
    RUN_TEST(test_prepare_arc_rejects_conflicting_or_missing_geometry);
    RUN_TEST(test_prepare_arc_with_ijk_calculates_radius_and_length);
    printf("test_nc_interp_math: PASS\n");
    return 0;
}

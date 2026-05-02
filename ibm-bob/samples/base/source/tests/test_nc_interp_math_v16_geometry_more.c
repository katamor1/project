#include "test_common.h"
#include <math.h>
#include "system_shared.h"
#include "nc_interp_math.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_arc(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.motion_type = NC_MOTION_ARC_CCW;
    b.plane = NC_PLANE_XY;
    b.axis_start[0] = 10000;
    b.axis_start[1] = 0;
    b.axis_target[0] = 0;
    b.axis_target[1] = 10000;
    b.arc_center[0] = 0;
    b.arc_center[1] = 0;
    b.modal_flags = NC_ARC_FLAG_IJK;
    return b;
}

static int test_plane_axes_returns_expected_axis_pairs(void)
{
    uint32_t a = 99U;
    uint32_t b = 99U;
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

static int test_signed_sweep_has_expected_direction_for_cw_and_ccw(void)
{
    double ccw = NcInterp_SignedSweep(NC_MOTION_ARC_CCW, 10000.0, 0.0, 0.0, 10000.0, 0.0, 0.0);
    double cw = NcInterp_SignedSweep(NC_MOTION_ARC_CW, 10000.0, 0.0, 0.0, 10000.0, 0.0, 0.0);
    TEST_ASSERT_TRUE(ccw > 1.5 && ccw < 1.6);
    TEST_ASSERT_TRUE(cw < -4.7 && cw > -4.8);
    return 0;
}

static int test_prepare_arc_with_ijk_reports_quarter_circle_length_units(void)
{
    NC_EXEC_BLOCK b = make_arc();
    NC_ERROR_CODE err = NC_ERR_NONE;
    double length = 0.0;
    TEST_ASSERT_EQ_INT(0, NcInterp_PrepareArcTs(&b, &length, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_INT(10000, b.arc_radius);
    TEST_ASSERT_TRUE(length > 1.56 && length < 1.58);
    return 0;
}

static int test_prepare_arc_rejects_conflicting_ijk_and_r_flags(void)
{
    NC_EXEC_BLOCK b = make_arc();
    NC_ERROR_CODE err = NC_ERR_NONE;
    double length = 0.0;
    b.modal_flags = NC_ARC_FLAG_IJK | NC_ARC_FLAG_R;
    b.arc_radius = 10000;
    TEST_ASSERT_EQ_INT(-1, NcInterp_PrepareArcTs(&b, &length, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_ARC_GEOMETRY, err);
    return 0;
}

int main(void)
{
    RUN_TEST(test_plane_axes_returns_expected_axis_pairs);
    RUN_TEST(test_signed_sweep_has_expected_direction_for_cw_and_ccw);
    RUN_TEST(test_prepare_arc_with_ijk_reports_quarter_circle_length_units);
    RUN_TEST(test_prepare_arc_rejects_conflicting_ijk_and_r_flags);
    printf("test_nc_interp_math_v16_geometry_more: PASS\n");
    return 0;
}

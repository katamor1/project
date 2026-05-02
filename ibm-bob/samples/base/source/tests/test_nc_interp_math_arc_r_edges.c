#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_interp_math.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static NC_EXEC_BLOCK make_arc_r(uint32_t lineNo, NC_MOTION_TYPE motion, int32_t radius)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = motion;
    b.plane = NC_PLANE_XY;
    b.modal_flags = NC_ARC_FLAG_R;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 10000;
    b.axis_target[1] = 0;
    b.arc_radius = radius;
    return b;
}

static int test_r_radius_rejects_too_small_radius(void)
{
    NC_EXEC_BLOCK b = make_arc_r(211U, NC_MOTION_ARC_CCW, 4000);
    double path = 0.0;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    TEST_ASSERT_EQ_INT(-1, NcInterp_PrepareArcTs(&b, &path, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_ARC_GEOMETRY, err);
    return 0;
}

static int test_r_radius_positive_small_arc_selects_center_and_length(void)
{
    NC_EXEC_BLOCK b = make_arc_r(212U, NC_MOTION_ARC_CCW, 10000);
    double path = 0.0;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    TEST_ASSERT_EQ_INT(0, NcInterp_PrepareArcTs(&b, &path, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_TRUE(b.arc_radius >= 9990 && b.arc_radius <= 10010);
    TEST_ASSERT_TRUE(b.arc_center[0] >= 4990 && b.arc_center[0] <= 5010);
    TEST_ASSERT_TRUE(path > 0.5 && path < 20.0);
    return 0;
}

static int test_same_endpoint_ijk_arc_allows_full_circle_with_out_of_plane_delta(void)
{
    NC_EXEC_BLOCK b;
    double path = 0.0;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 213U;
    b.motion_type = NC_MOTION_ARC_CW;
    b.plane = NC_PLANE_XY;
    b.modal_flags = NC_ARC_FLAG_IJK;
    b.axis_start[0] = 10000;
    b.axis_start[1] = 0;
    b.axis_target[0] = 10000;
    b.axis_target[1] = 0;
    b.axis_target[2] = 5000;
    b.arc_center[0] = 0;
    b.arc_center[1] = 0;

    TEST_ASSERT_EQ_INT(0, NcInterp_PrepareArcTs(&b, &path, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_TRUE(path >= 0.49 && path <= 0.51);
    return 0;
}

int main(void)
{
    RUN_TEST(test_r_radius_rejects_too_small_radius);
    RUN_TEST(test_r_radius_positive_small_arc_selects_center_and_length);
    RUN_TEST(test_same_endpoint_ijk_arc_allows_full_circle_with_out_of_plane_delta);
    printf("test_nc_interp_math_arc_r_edges: PASS\n");
    return 0;
}

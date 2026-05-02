#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_parser_internal.h"

static void setup_block(NC_EXEC_BLOCK* block, NC_PARSE_CONTEXT* ctx)
{
    SystemShared_Initialize();
    memset(block, 0, sizeof(*block));
    memset(ctx, 0, sizeof(*ctx));
}

static int test_smoothing_accepts_once_and_rejects_duplicate(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(8), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(8), block.g_code10);
    TEST_ASSERT_TRUE((block.modal_flags & NC_MODAL_FLAG_G08) != 0U);
    TEST_ASSERT_EQ_UINT(1U, ctx.smoothing_seen);

    TEST_ASSERT_EQ_INT(-3, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(5), &block, &ctx));
    return 0;
}

static int test_feature_g_sets_sync_compound_overlay_and_advanced_flags(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(51, 4), &block, &ctx));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_SYNC) != 0U);
    TEST_ASSERT_EQ_UINT(NC_MOTION_NONE, block.motion_type);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(50, 5), &block, &ctx));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_COMPOUND) != 0U);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(51, 6), &block, &ctx));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_OVERLAY) != 0U);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(6, 2), &block, &ctx));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_ADVANCED_INTERP) != 0U);
    TEST_ASSERT_EQ_UINT(NC_MOTION_ADVANCED_INTERP, block.motion_type);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(1, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(123), &block, &ctx));
    TEST_ASSERT_EQ_UINT(0U, block.feature_flags);
    return 0;
}

static int test_feature_g_sets_coordinate_and_compensation_commands(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(68, 2), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_KINEMATICS_CMD_TILT_ENABLE, block.kinematics_command);
    TEST_ASSERT_EQ_UINT(NC_PARSE_COORD_TILTED_PLANE, ctx.coord_mode);
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_TILTED_PLANE) != 0U);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(53, 1), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_TOOL_AXIS_CMD_ENABLE, block.tool_axis_command);
    TEST_ASSERT_EQ_UINT(NC_PARSE_COORD_TOOL_AXIS, ctx.coord_mode);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(51, 1), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_MIRROR_CMD_ENABLE, block.mirror_command);
    TEST_ASSERT_EQ_UINT(NC_PARSE_COORD_MIRROR_SETTING, ctx.coord_mode);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE10(10, 6), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_AXIS_RETRACT_CMD_RETRACT, block.axis_retract_command);
    TEST_ASSERT_EQ_UINT(NC_MOTION_REFERENCE_RETURN, block.motion_type);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(16), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_POLAR_CMD_ENABLE, block.polar_command);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(42), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_CUTTER_COMP_CMD_RIGHT, block.cutter_comp_command);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(44), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_TOOL_LENGTH_CMD_NEGATIVE, block.tool_length_command);

    setup_block(&block, &ctx);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyFeatureG(NC_G_CODE_WHOLE(68), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_ROTATION_CMD_ENABLE, block.rotation_command);
    TEST_ASSERT_EQ_UINT(NC_PARSE_COORD_ROTATION_CENTER, ctx.coord_mode);
    return 0;
}

static int test_finalize_g08_and_g05_error_and_success_paths(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;
    NC_ERROR_CODE err = NC_ERR_NONE;

    setup_block(&block, &ctx);
    block.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    TEST_ASSERT_EQ_INT(-1, NcParser_FinalizeFeatureBlock(&block, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);

    setup_block(&block, &ctx);
    block.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    block.feature_flags = NC_FEATURE_FLAG_PARAM_P;
    block.p_word = 2U;
    TEST_ASSERT_EQ_INT(0, NcParser_FinalizeFeatureBlock(&block, &err));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_SMOOTHING) != 0U);
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_HIGH_PRECISION) != 0U);

    setup_block(&block, &ctx);
    block.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    block.feature_flags = NC_FEATURE_FLAG_PARAM_P;
    block.p_word = 10000U;
    TEST_ASSERT_EQ_INT(0, NcParser_FinalizeFeatureBlock(&block, &err));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_HPCC) != 0U);
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_SMOOTHING) != 0U);

    setup_block(&block, &ctx);
    block.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    block.feature_flags = NC_FEATURE_FLAG_PARAM_P;
    block.p_word = 3U;
    TEST_ASSERT_EQ_INT(0, NcParser_FinalizeFeatureBlock(&block, &err));
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_HIGH_SPEED_MODE) != 0U);

    setup_block(&block, &ctx);
    block.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    block.feature_flags = NC_FEATURE_FLAG_PARAM_P;
    block.p_word = 17U;
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParser_FinalizeFeatureBlock(&block, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);

    setup_block(&block, &ctx);
    block.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    block.feature_flags = NC_FEATURE_FLAG_PARAM_P;
    block.p_word = 10000U;
    g_ncCompensationStatus.cutter_comp_mode = 1U;
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcParser_FinalizeFeatureBlock(&block, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_MODAL_CONFLICT, err);
    return 0;
}

int main(void)
{
    RUN_TEST(test_smoothing_accepts_once_and_rejects_duplicate);
    RUN_TEST(test_feature_g_sets_sync_compound_overlay_and_advanced_flags);
    RUN_TEST(test_feature_g_sets_coordinate_and_compensation_commands);
    RUN_TEST(test_finalize_g08_and_g05_error_and_success_paths);
    printf("test_nc_parser_feature_direct: PASS\n");
    return 0;
}

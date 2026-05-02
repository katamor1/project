#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_parser_internal.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcParse_ResetModal();
}

static int test_init_block_uses_modal_defaults_and_line_no(void)
{
    NC_EXEC_BLOCK block;

    setup();
    NcParser_InitBlock(&block, 123U);
    TEST_ASSERT_EQ_UINT(123U, block.line_no);
    TEST_ASSERT_EQ_UINT(NC_MOTION_NONE, block.motion_type);
    TEST_ASSERT_EQ_UINT(NC_PLANE_XY, block.plane);
    TEST_ASSERT_EQ_UINT(NC_FEED_MODE_PER_MIN, block.feed_mode);
    TEST_ASSERT_EQ_INT(0, block.feed);

    NcParser_SetFeed(50000, &block);
    NcParser_SetSpindle(1200U, &block);
    TEST_ASSERT_EQ_INT(50000, block.feed);
    TEST_ASSERT_EQ_UINT(1200U, block.spindle_speed);
    TEST_ASSERT_TRUE((block.aux_flags & NC_AUX_FLAG_SPINDLE) != 0U);

    NcParser_InitBlock(&block, 124U);
    TEST_ASSERT_EQ_INT(50000, block.feed);
    TEST_ASSERT_EQ_UINT(1200U, block.spindle_speed);
    TEST_ASSERT_EQ_UINT(0U, block.aux_flags);
    return 0;
}

static int test_apply_gcode_modal_groups_and_conflicts(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 10U);

    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(17), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_PLANE_XY, block.plane);
    TEST_ASSERT_EQ_INT(-3, NcParser_ApplyGCode(NC_G_CODE_WHOLE(18), &block, &ctx));

    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 11U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(90), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_DISTANCE_ABSOLUTE, g_ncCoordinateState.distance_mode);
    TEST_ASSERT_EQ_INT(-3, NcParser_ApplyGCode(NC_G_CODE_WHOLE(91), &block, &ctx));

    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 12U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(1), &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, block.motion_type);
    TEST_ASSERT_EQ_INT(-3, NcParser_ApplyGCode(NC_G_CODE_WHOLE(0), &block, &ctx));
    return 0;
}

static int test_finalize_turns_empty_motion_into_noop_or_aux_only(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;
    NC_ERROR_CODE err = NC_ERR_NONE;

    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 20U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(1), &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_FinalizeBlock(&block, &err));
    TEST_ASSERT_EQ_UINT(NC_MOTION_NONE, block.motion_type);

    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 21U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(1), &block, &ctx));
    NcParser_SetSpindle(1500U, &block);
    TEST_ASSERT_EQ_INT(0, NcParser_FinalizeBlock(&block, &err));
    TEST_ASSERT_EQ_UINT(NC_MOTION_AUX_ONLY, block.motion_type);
    TEST_ASSERT_TRUE((block.aux_flags & NC_AUX_FLAG_SPINDLE) != 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_init_block_uses_modal_defaults_and_line_no);
    RUN_TEST(test_apply_gcode_modal_groups_and_conflicts);
    RUN_TEST(test_finalize_turns_empty_motion_into_noop_or_aux_only);
    printf("test_nc_parser_modal_direct: PASS\n");
    return 0;
}

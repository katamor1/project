#include "test_common.h"
#include "system_shared.h"
#include "nc_parser_internal.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcParser_ResetModalState();
}

static int test_init_block_uses_modal_defaults_and_requested_line_number(void)
{
    NC_EXEC_BLOCK b;
    setup();
    NcParser_InitBlock(&b, 1651U);
    TEST_ASSERT_EQ_UINT(1651U, b.line_no);
    TEST_ASSERT_EQ_UINT(NC_MOTION_NONE, b.motion_type);
    TEST_ASSERT_EQ_UINT(NC_PLANE_XY, b.plane);
    TEST_ASSERT_EQ_UINT(NC_FEED_MODE_PER_MIN, b.feed_mode);
    TEST_ASSERT_EQ_UINT(0U, b.axis_mask);
    return 0;
}

static int test_set_feed_and_spindle_update_block_and_aux_flags(void)
{
    NC_EXEC_BLOCK b;
    setup();
    NcParser_InitBlock(&b, 1652U);
    NcParser_SetFeed(12345, &b);
    NcParser_SetSpindle(3500U, &b);
    TEST_ASSERT_EQ_INT(12345, b.feed);
    TEST_ASSERT_EQ_UINT(3500U, b.spindle_speed);
    TEST_ASSERT_TRUE((b.aux_flags & NC_AUX_FLAG_SPINDLE) != 0U);
    return 0;
}

static int test_duplicate_motion_group_is_rejected_in_single_block_context(void)
{
    NC_EXEC_BLOCK b;
    NC_PARSE_CONTEXT ctx;
    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&b, 1653U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(1), &b, &ctx));
    TEST_ASSERT_EQ_INT(-3, NcParser_ApplyGCode(NC_G_CODE_WHOLE(0), &b, &ctx));
    return 0;
}

static int test_g53_machine_direct_is_rejected_when_distance_mode_is_incremental(void)
{
    NC_EXEC_BLOCK b;
    NC_PARSE_CONTEXT ctx;
    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&b, 1654U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyGCode(NC_G_CODE_WHOLE(91), &b, &ctx));
    memset(&ctx, 0, sizeof(ctx));
    TEST_ASSERT_EQ_INT(-4, NcParser_ApplyGCode(NC_G_CODE_WHOLE(53), &b, &ctx));
    return 0;
}

int main(void)
{
    RUN_TEST(test_init_block_uses_modal_defaults_and_requested_line_number);
    RUN_TEST(test_set_feed_and_spindle_update_block_and_aux_flags);
    RUN_TEST(test_duplicate_motion_group_is_rejected_in_single_block_context);
    RUN_TEST(test_g53_machine_direct_is_rejected_when_distance_mode_is_incremental);
    printf("test_nc_parser_internal_modal_more: PASS\n");
    return 0;
}

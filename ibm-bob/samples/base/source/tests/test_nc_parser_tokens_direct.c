#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_parser_internal.h"
#include "nc_capability.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcParse_ResetModal();
    NcCapability_Reset();
}

static int test_axis_feed_spindle_tool_and_mcode_tokens(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 30U);

    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('G', 1.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('X', 1.25, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('Y', -2.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('F', 123.456, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('S', 3000.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('T', 7.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('M', 6.0, &block, &ctx));

    TEST_ASSERT_EQ_UINT(NC_MOTION_LINEAR, block.motion_type);
    TEST_ASSERT_TRUE((block.axis_mask & 0x03U) == 0x03U);
    TEST_ASSERT_EQ_INT(12500, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(-20000, block.axis_target[1]);
    TEST_ASSERT_EQ_INT(1234560, block.feed);
    TEST_ASSERT_EQ_UINT(3000U, block.spindle_speed);
    TEST_ASSERT_EQ_UINT(7U, block.tool_no);
    TEST_ASSERT_EQ_UINT(6U, block.aux_m_code);
    TEST_ASSERT_TRUE((block.aux_flags & NC_AUX_FLAG_SPINDLE) != 0U);
    TEST_ASSERT_TRUE((block.aux_flags & NC_AUX_FLAG_TOOL) != 0U);
    TEST_ASSERT_TRUE((block.aux_flags & NC_AUX_FLAG_M_CODE) != 0U);
    return 0;
}

static int test_token_error_paths_record_unsupported_and_negative_aux(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 31U);

    TEST_ASSERT_EQ_INT(-1, NcParser_ApplyToken('?', 1.0, &block, &ctx));
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_address_events);

    TEST_ASSERT_EQ_INT(-2, NcParser_ApplyToken('S', -1.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(-2, NcParser_ApplyToken('M', -1.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(-2, NcParser_ApplyToken('G', 1000.0, &block, &ctx));
    return 0;
}

static int test_dwell_pword_and_arc_radius_contexts(void)
{
    NC_EXEC_BLOCK block;
    NC_PARSE_CONTEXT ctx;

    setup();
    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 32U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('G', 4.0, &block, &ctx));
    TEST_ASSERT_EQ_UINT(NC_MOTION_DWELL, block.motion_type);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('P', 25.0, &block, &ctx));
    TEST_ASSERT_EQ_UINT(25U, block.interp_ticks);

    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 33U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('G', 2.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('R', 5.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(50000, block.arc_radius);
    TEST_ASSERT_TRUE((block.modal_flags & NC_ARC_FLAG_R) != 0U);

    memset(&ctx, 0, sizeof(ctx));
    NcParser_InitBlock(&block, 34U);
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('G', 81.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(0, NcParser_ApplyToken('R', 3.0, &block, &ctx));
    TEST_ASSERT_EQ_INT(30000, block.r_value);
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_PARAM_R) != 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_feed_spindle_tool_and_mcode_tokens);
    RUN_TEST(test_token_error_paths_record_unsupported_and_negative_aux);
    RUN_TEST(test_dwell_pword_and_arc_radius_contexts);
    printf("test_nc_parser_tokens_direct: PASS\n");
    return 0;
}

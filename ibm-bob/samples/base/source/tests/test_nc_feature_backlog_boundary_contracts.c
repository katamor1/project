/*
 * IBM-Bob additional unit tests v44_40.
 *
 * Adds another 40 unit test cases for continuation modules v35-v42.
 * This set intentionally avoids duplicating v43_40 and focuses on:
 *   1) self-check contract
 *   2) feature ID boundary enable/disable behavior
 *   3) NULL RT tick warning and no tick-count side effect
 *   4) safe RT tick with zeroed inputs
 *   5) disabled-feature exclusion followed by re-enable and TS re-apply
 */
#include "nc_feature_backlog_aux_safety.h"
#include "nc_feature_backlog_lube_brake_thermal.h"
#include "nc_feature_backlog_readiness_sensors.h"
#include "nc_feature_backlog_pallet_fixture_safety.h"
#include "nc_feature_backlog_tool_utility_monitors.h"
#include "nc_feature_backlog_process_safety.h"
#include "nc_feature_backlog_service_interlocks.h"
#include "nc_feature_backlog_loader_environment.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t g_tests_run = 0u;
static uint32_t g_tests_failed = 0u;

static void test_begin(const char *name)
{
    g_tests_run++;
    printf("[TEST %02u] %s\n", g_tests_run, name);
}

static void fail_u32(const char *expr, uint32_t actual, uint32_t expected, const char *file, int line)
{
    g_tests_failed++;
    printf("  FAIL %s:%d %s actual=%u expected=%u\n", file, line, expr, actual, expected);
}

static void fail_i32(const char *expr, int32_t actual, int32_t expected, const char *file, int line)
{
    g_tests_failed++;
    printf("  FAIL %s:%d %s actual=%d expected=%d\n", file, line, expr, actual, expected);
}

static void fail_true(const char *expr, const char *file, int line)
{
    g_tests_failed++;
    printf("  FAIL %s:%d %s\n", file, line, expr);
}

#define EXPECT_EQ_U32(actual_, expected_) do { \
    uint32_t a__ = (uint32_t)(actual_); \
    uint32_t e__ = (uint32_t)(expected_); \
    if (a__ != e__) { fail_u32(#actual_, a__, e__, __FILE__, __LINE__); } \
} while (0)

#define EXPECT_EQ_I32(actual_, expected_) do { \
    int32_t a__ = (int32_t)(actual_); \
    int32_t e__ = (int32_t)(expected_); \
    if (a__ != e__) { fail_i32(#actual_, a__, e__, __FILE__, __LINE__); } \
} while (0)

#define EXPECT_TRUE(expr_) do { \
    if (!(expr_)) { fail_true(#expr_, __FILE__, __LINE__); } \
} while (0)

#define EXPECT_ZERO_U32(value_) EXPECT_EQ_U32((value_), 0u)
#define EXPECT_NONZERO_U32(value_) do { \
    if ((uint32_t)(value_) == 0u) { fail_true(#value_ " != 0", __FILE__, __LINE__); } \
} while (0)

#define DEFINE_V44_TESTS(VNUM) \
static uint32_t sum_hits_v##VNUM(const NC_FEATURE_BACKLOG_V##VNUM##_STATUS *status) \
{ \
    uint32_t i; \
    uint32_t sum = 0u; \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        sum += status->per_feature_hits[i]; \
    } \
    return sum; \
} \
static void fill_block_v##VNUM(NC_FEATURE_BACKLOG_V##VNUM##_BLOCK *block, int32_t base_value) \
{ \
    uint32_t i; \
    memset(block, 0, sizeof(*block)); \
    block->feed_override_percent = 100; \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_AXIS_COUNT; ++i) { \
        block->axis_target[i] = base_value + (int32_t)i; \
    } \
} \
static void test_v##VNUM##_v44_01_selfcheck_contract(void) \
{ \
    int rc; \
    test_begin("v" #VNUM " self-check callable return contract"); \
    rc = Api_RunNcFeatureBacklogV##VNUM##SelfCheck(); \
    EXPECT_TRUE((rc == NC_FEATURE_BACKLOG_V##VNUM##_OK) || (rc == NC_FEATURE_BACKLOG_V##VNUM##_ERR_RANGE) || (rc == NC_FEATURE_BACKLOG_V##VNUM##_ERR_NULL)); \
} \
static void test_v##VNUM##_v44_02_feature_id_boundaries(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    uint32_t last = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u; \
    test_begin("v" #VNUM " feature ID boundaries"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(base - 1u), 0u); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(last + 1u), 0u); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(last, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, 1u); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(last), 1u); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base - 1u, 1u), NC_FEATURE_BACKLOG_V##VNUM##_ERR_RANGE); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(last + 1u, 1u), NC_FEATURE_BACKLOG_V##VNUM##_ERR_RANGE); \
} \
static void test_v##VNUM##_v44_03_null_rt_tick_warning_no_tick(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " NULL RT tick warning without rt count"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    NcFeatureBacklogV##VNUM##_RunRtTick((const NC_FEATURE_BACKLOG_V##VNUM##_RT_IO *)0); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.warning_count, 1u); \
    EXPECT_EQ_U32(status.rt_tick_count, 0u); \
    EXPECT_EQ_U32(sum_hits_v##VNUM(&status), 0u); \
} \
static void test_v##VNUM##_v44_04_zero_rt_tick_is_safe(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_RT_IO io; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " zeroed RT tick safe path"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    memset(&io, 0, sizeof(io)); \
    io.cycle_counter = 123u; \
    NcFeatureBacklogV##VNUM##_RunRtTick(&io); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.rt_tick_count, 1u); \
    EXPECT_EQ_U32(status.warning_count, 0u); \
    EXPECT_EQ_U32(sum_hits_v##VNUM(&status), 0u); \
} \
static void test_v##VNUM##_v44_05_disable_excludes_then_reenable_ts_apply(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    test_begin("v" #VNUM " TS disabled feature exclusion and re-enable"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base, 0u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    fill_block_v##VNUM(&block, (int32_t)(VNUM * 100)); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u); \
    EXPECT_EQ_U32(status.ts_apply_count, 1u); \
    EXPECT_EQ_U32(status.per_feature_hits[0], 0u); \
    EXPECT_EQ_U32(status.per_feature_hits[NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u], 1u); \
    EXPECT_NONZERO_U32(sum_hits_v##VNUM(&status)); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    fill_block_v##VNUM(&block, (int32_t)(VNUM * 100 + 50)); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
    EXPECT_EQ_U32(status.ts_apply_count, 2u); \
    EXPECT_EQ_U32(status.per_feature_hits[0], 1u); \
    EXPECT_EQ_U32(status.per_feature_hits[NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u], 2u); \
    EXPECT_EQ_I32(status.last_x, block.axis_target[0]); \
}

DEFINE_V44_TESTS(35)
DEFINE_V44_TESTS(36)
DEFINE_V44_TESTS(37)
DEFINE_V44_TESTS(38)
DEFINE_V44_TESTS(39)
DEFINE_V44_TESTS(40)
DEFINE_V44_TESTS(41)
DEFINE_V44_TESTS(42)

int main(void)
{
    test_v35_v44_01_selfcheck_contract();
    test_v35_v44_02_feature_id_boundaries();
    test_v35_v44_03_null_rt_tick_warning_no_tick();
    test_v35_v44_04_zero_rt_tick_is_safe();
    test_v35_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v36_v44_01_selfcheck_contract();
    test_v36_v44_02_feature_id_boundaries();
    test_v36_v44_03_null_rt_tick_warning_no_tick();
    test_v36_v44_04_zero_rt_tick_is_safe();
    test_v36_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v37_v44_01_selfcheck_contract();
    test_v37_v44_02_feature_id_boundaries();
    test_v37_v44_03_null_rt_tick_warning_no_tick();
    test_v37_v44_04_zero_rt_tick_is_safe();
    test_v37_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v38_v44_01_selfcheck_contract();
    test_v38_v44_02_feature_id_boundaries();
    test_v38_v44_03_null_rt_tick_warning_no_tick();
    test_v38_v44_04_zero_rt_tick_is_safe();
    test_v38_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v39_v44_01_selfcheck_contract();
    test_v39_v44_02_feature_id_boundaries();
    test_v39_v44_03_null_rt_tick_warning_no_tick();
    test_v39_v44_04_zero_rt_tick_is_safe();
    test_v39_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v40_v44_01_selfcheck_contract();
    test_v40_v44_02_feature_id_boundaries();
    test_v40_v44_03_null_rt_tick_warning_no_tick();
    test_v40_v44_04_zero_rt_tick_is_safe();
    test_v40_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v41_v44_01_selfcheck_contract();
    test_v41_v44_02_feature_id_boundaries();
    test_v41_v44_03_null_rt_tick_warning_no_tick();
    test_v41_v44_04_zero_rt_tick_is_safe();
    test_v41_v44_05_disable_excludes_then_reenable_ts_apply();

    test_v42_v44_01_selfcheck_contract();
    test_v42_v44_02_feature_id_boundaries();
    test_v42_v44_03_null_rt_tick_warning_no_tick();
    test_v42_v44_04_zero_rt_tick_is_safe();
    test_v42_v44_05_disable_excludes_then_reenable_ts_apply();

    printf("UNIT_TEST_SUMMARY tests=%u failures=%u\n", g_tests_run, g_tests_failed);
    if (g_tests_run != 40u) {
        printf("FAILED expected exactly 40 test cases\n");
        return 1;
    }
    if (g_tests_failed != 0u) {
        printf("FAILED failures=%u\n", g_tests_failed);
        return 1;
    }
    printf("PASS additional_unit_tests_v44_40\n");
    return 0;
}

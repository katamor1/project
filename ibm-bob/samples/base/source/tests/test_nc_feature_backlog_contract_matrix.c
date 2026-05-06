/*
 * IBM-Bob additional unit tests v43_40.
 *
 * Adds 40 unit test cases for continuation modules v35-v42.
 * Each module gets 5 test cases:
 *   1) initialization/status defaults
 *   2) API validation and feature-name fallback
 *   3) enable/disable/configured_count behavior
 *   4) NULL TS block warning behavior
 *   5) TS block effect and per-feature hit recording
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

#define EXPECT_NONZERO_U32(value_) do { \
    if ((uint32_t)(value_) == 0u) { fail_true(#value_ " != 0", __FILE__, __LINE__); } \
} while (0)

#define EXPECT_ZERO_U32(value_) do { \
    if ((uint32_t)(value_) != 0u) { fail_u32(#value_, (uint32_t)(value_), 0u, __FILE__, __LINE__); } \
} while (0)

#define DEFINE_V_TESTS(VNUM) \
static void test_v##VNUM##_01_init_status_defaults(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " init/status defaults"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.implemented_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
    EXPECT_EQ_U32(status.configured_count, 0u); \
    EXPECT_EQ_U32(status.enabled_mask, 0u); \
    EXPECT_EQ_U32(status.ts_apply_count, 0u); \
    EXPECT_EQ_U32(status.rt_tick_count, 0u); \
    EXPECT_EQ_I32(status.last_feed_override_percent, 100); \
} \
static void test_v##VNUM##_02_api_validation_and_names(void) \
{ \
    test_begin("v" #VNUM " API validation and unknown feature name"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status((NC_FEATURE_BACKLOG_V##VNUM##_STATUS *)0), NC_FEATURE_BACKLOG_V##VNUM##_ERR_NULL); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT, 1u), NC_FEATURE_BACKLOG_V##VNUM##_ERR_RANGE); \
    EXPECT_TRUE(strcmp(NcFeatureBacklogV##VNUM##_GetFeatureName(NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID), "unknown") != 0); \
    EXPECT_TRUE(strcmp(NcFeatureBacklogV##VNUM##_GetFeatureName(NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT), "unknown") == 0); \
} \
static void test_v##VNUM##_03_enable_disable_configured_count(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t first = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    test_begin("v" #VNUM " enable/disable configured_count"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(first, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(first), 1u); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, 1u); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(first, 0u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(first), 0u); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, 0u); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
    EXPECT_NONZERO_U32(status.enabled_mask); \
} \
static void test_v##VNUM##_04_null_block_warning(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " NULL TS block warning"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs((NC_FEATURE_BACKLOG_V##VNUM##_BLOCK *)0); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.warning_count, 1u); \
    EXPECT_EQ_U32(status.ts_apply_count, 0u); \
} \
static void test_v##VNUM##_05_ts_block_effects_all_features(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t i; \
    test_begin("v" #VNUM " TS block effects and per-feature hits"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    memset(&block, 0, sizeof(block)); \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_AXIS_COUNT; ++i) { \
        block.axis_target[i] = (int32_t)((VNUM * 100) + (int32_t)i); \
    } \
    block.feed_override_percent = 100; \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.ts_apply_count, 1u); \
    EXPECT_EQ_U32(status.configured_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
    EXPECT_NONZERO_U32(status.cat_mask); \
    EXPECT_EQ_U32(status.per_feature_hits[0], 1u); \
    EXPECT_EQ_U32(status.per_feature_hits[NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u], 1u); \
    EXPECT_NONZERO_U32(block.hold_request | block.interlock_mask | block.sequence_gate_mask | block.diagnostics_mask | block.trace_event_code | block.aux_request_bits); \
    EXPECT_TRUE(block.feed_override_percent <= 100); \
    EXPECT_EQ_I32(status.last_x, block.axis_target[0]); \
}

DEFINE_V_TESTS(35)
DEFINE_V_TESTS(36)
DEFINE_V_TESTS(37)
DEFINE_V_TESTS(38)
DEFINE_V_TESTS(39)
DEFINE_V_TESTS(40)
DEFINE_V_TESTS(41)
DEFINE_V_TESTS(42)

int main(void)
{
    test_v35_01_init_status_defaults();
    test_v35_02_api_validation_and_names();
    test_v35_03_enable_disable_configured_count();
    test_v35_04_null_block_warning();
    test_v35_05_ts_block_effects_all_features();

    test_v36_01_init_status_defaults();
    test_v36_02_api_validation_and_names();
    test_v36_03_enable_disable_configured_count();
    test_v36_04_null_block_warning();
    test_v36_05_ts_block_effects_all_features();

    test_v37_01_init_status_defaults();
    test_v37_02_api_validation_and_names();
    test_v37_03_enable_disable_configured_count();
    test_v37_04_null_block_warning();
    test_v37_05_ts_block_effects_all_features();

    test_v38_01_init_status_defaults();
    test_v38_02_api_validation_and_names();
    test_v38_03_enable_disable_configured_count();
    test_v38_04_null_block_warning();
    test_v38_05_ts_block_effects_all_features();

    test_v39_01_init_status_defaults();
    test_v39_02_api_validation_and_names();
    test_v39_03_enable_disable_configured_count();
    test_v39_04_null_block_warning();
    test_v39_05_ts_block_effects_all_features();

    test_v40_01_init_status_defaults();
    test_v40_02_api_validation_and_names();
    test_v40_03_enable_disable_configured_count();
    test_v40_04_null_block_warning();
    test_v40_05_ts_block_effects_all_features();

    test_v41_01_init_status_defaults();
    test_v41_02_api_validation_and_names();
    test_v41_03_enable_disable_configured_count();
    test_v41_04_null_block_warning();
    test_v41_05_ts_block_effects_all_features();

    test_v42_01_init_status_defaults();
    test_v42_02_api_validation_and_names();
    test_v42_03_enable_disable_configured_count();
    test_v42_04_null_block_warning();
    test_v42_05_ts_block_effects_all_features();

    printf("UNIT_TEST_SUMMARY tests=%u failures=%u\n", g_tests_run, g_tests_failed);
    if (g_tests_run != 40u) {
        printf("FAILED expected exactly 40 test cases\n");
        return 1;
    }
    if (g_tests_failed != 0u) {
        printf("FAILED failures=%u\n", g_tests_failed);
        return 1;
    }
    printf("PASS additional_unit_tests_v43_40\n");
    return 0;
}

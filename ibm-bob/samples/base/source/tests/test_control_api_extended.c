#include "test_common.h"
#include "stub_external_io.h"
#include "control_api.h"

/*
 * Public API wrapper tests for functions that were not covered by the first
 * control_api unit test.  The dependency modules are linked as stubs here so
 * this file verifies API validation/copy semantics and return-code propagation
 * without pulling unrelated TS/RT behavior into the unit.
 */

static int test_coordinate_transform_enable_and_snapshot_copy(void)
{
    NC_COORDINATE_TRANSFORM_STATUS st;

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, Api_GetNcCoordinateTransformStatus(NULL));

    TEST_ASSERT_EQ_INT(0, Api_SetNcCoordinateTransformEnabled(1U, 0U, 2U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcCoordinateTransformStatus(&st));
    TEST_ASSERT_EQ_UINT(1U, st.dynamic_fixture_enabled);
    TEST_ASSERT_EQ_UINT(0U, st.work_mount_error_enabled);
    TEST_ASSERT_EQ_UINT(1U, st.rotary_fixture_enabled);
    TEST_ASSERT_TRUE(st.generation > 0U);
    return 0;
}

static int test_reference_precision_and_axis_config_api_paths(void)
{
    NC_PRECISION_STATUS precision;
    NC_AXIS_CONFIG_STATUS axis;

    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(-1, Api_SetNcReferenceAxisConfig(AXIS_MAX, 0, 1, 1000, 0U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcReferenceAxisConfig(0U, 1234, 1, 1000, 1U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcOneDirectionApproach(AXIS_MAX, 100));
    TEST_ASSERT_EQ_INT(0, Api_SetNcOneDirectionApproach(0U, 100));

    TEST_ASSERT_EQ_INT(-1, Api_GetNcPrecisionStatus(NULL));
    TEST_ASSERT_EQ_INT(0, Api_SetNcLearningControl(1U, 30, 2U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcVibrationSuppression(1U, 120U, 20U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcPreviewControl(1U, 4U, 10U));
    g_ncPrecisionStatus.learning_enabled = 1U;
    g_ncPrecisionStatus.preview_lookahead_blocks = 4U;
    TEST_ASSERT_EQ_INT(0, Api_GetNcPrecisionStatus(&precision));
    TEST_ASSERT_EQ_UINT(1U, precision.learning_enabled);
    TEST_ASSERT_EQ_UINT(4U, precision.preview_lookahead_blocks);

    TEST_ASSERT_EQ_INT(-1, Api_GetNcAxisConfigStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcAxisDefinition(AXIS_MAX, 'X', NC_AXIS_TYPE_LINEAR, 0U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDefinition(0U, 'X', NC_AXIS_TYPE_LINEAR, 0U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcPathAxisMask(0U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcPathAxisMask(0x03U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDetachedMask(0x04U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcAxisDiameterMode(AXIS_MAX, 1U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDiameterMode(1U, 1U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcAxisConfigStatus(&axis));
    TEST_ASSERT_EQ_UINT(0x03U, axis.active_path_axis_mask);
    TEST_ASSERT_EQ_UINT(0x04U, axis.detached_axis_mask);
    TEST_ASSERT_TRUE((axis.diameter_axis_mask & 0x02U) != 0U);
    return 0;
}

static int test_additional_getters_copy_null_and_state(void)
{
    PRESTART_INTERLOCK_STATUS interlock;
    NC_AUX_STATUS aux;
    NC_EVENT_RING ring;
    IO_TRACE_BUFFER trace;

    Test_ResetSystem();
    g_prestartInterlockStatus.ok = 1U;
    g_prestartInterlockStatus.mask = 0x55U;
    g_ncAuxStatus.active_m_code = 130U;
    g_ioTraceBuffer.frozen = 1U;
    g_ioTraceBuffer.write_index = 3U;
    NcEvent_Push(0xCAFEU, 1, 2, 99U);

    TEST_ASSERT_EQ_INT(-1, Api_GetPrestartInterlockStatus(NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetPrestartInterlockStatus(&interlock));
    TEST_ASSERT_EQ_UINT(1U, interlock.ok);
    TEST_ASSERT_EQ_UINT(0x55U, interlock.mask);

    TEST_ASSERT_EQ_INT(-1, Api_GetNcAuxStatus(NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetNcAuxStatus(&aux));
    TEST_ASSERT_EQ_UINT(130U, aux.active_m_code);

    TEST_ASSERT_EQ_INT(-1, Api_GetIoTraceBuffer(NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetIoTraceBuffer(&trace));
    TEST_ASSERT_EQ_UINT(1U, trace.frozen);
    TEST_ASSERT_EQ_UINT(3U, trace.write_index);

    TEST_ASSERT_EQ_INT(-1, Api_GetNcEventRing(NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetNcEventRing(&ring));
    TEST_ASSERT_EQ_UINT(1U, ring.captured_count);
    TEST_ASSERT_EQ_UINT(0xCAFEU, ring.items[0].code);
    TEST_ASSERT_EQ_UINT(99U, ring.items[0].line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_coordinate_transform_enable_and_snapshot_copy);
    RUN_TEST(test_reference_precision_and_axis_config_api_paths);
    RUN_TEST(test_additional_getters_copy_null_and_state);
    printf("test_control_api_extended: PASS\n");
    return 0;
}

/* ibm-bob/samples/base/source/src/nc_interp_math.c */
/* Calculates fixed-point arc centers, sweeps, and path lengths for TS. */
/* This exists so RT receives compact geometry and avoids NC text decisions. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_interp_math.h, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/nc_program.h */
#include <math.h>
#include "nc_interp_math.h"
#include "nc_program.h"

#define NC_PI (3.14159265358979323846)
/**
 * @brief Handle round fixed for this module.
 * @param value Numeric value being converted, clamped, or tested.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t RoundFixed(double value)
{
    return (int32_t)(value + ((value >= 0.0) ? 0.5 : -0.5));
}

/**
 * @brief Handle nc interp plane axes for this module.
 * @param plane Input value for plane.
 * @param pA First point used by the vector calculation.
 * @param pB Second point used by the vector calculation.
 */
void NcInterp_PlaneAxes(NC_PLANE plane, uint32_t* pA, uint32_t* pB)
{
    if (plane == NC_PLANE_ZX) {
        *pA = 2U;
        *pB = 0U;
    } else if (plane == NC_PLANE_YZ) {
        *pA = 1U;
        *pB = 2U;
    } else {
        *pA = 0U;
        *pB = 1U;
    }
}

/**
 * @brief Handle nc interp signed sweep for this module.
 * @param motion Motion type being tested by the helper.
 * @param su Input value for su.
 * @param sv Input value for sv.
 * @param eu Input value for eu.
 * @param ev Input value for ev.
 * @param cu Input value for cu.
 * @param cv Input value for cv.
 * @return Function-specific result value.
 */

double NcInterp_SignedSweep(NC_MOTION_TYPE motion,
                            double su,
                            double sv,
                            double eu,
                            double ev,
                            double cu,
                            double cv)
{
    double start = atan2(sv - cv, su - cu);
    double end = atan2(ev - cv, eu - cu);
    double sweep = end - start;

    if (motion == NC_MOTION_ARC_CW) {
        if (sweep >= 0.0) {
            sweep -= (2.0 * NC_PI);
        }
    } else if (sweep <= 0.0) {
        sweep += (2.0 * NC_PI);
    }
    return sweep;
}

/**
 * @brief Handle prepare arc center from r for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param pBlock NC execution block read or updated by the helper.
 * @param a First numeric value used by the helper.
 * @param b Second numeric value used by the helper.
 * @param pOutError Output pointer that receives the NC error code.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t PrepareArcCenterFromR(NC_EXEC_BLOCK* pBlock,
                                     uint32_t a,
                                     uint32_t b,
                                     NC_ERROR_CODE* pOutError)
{
    /* Prepare local state used by the following processing stage. */
    double su = (double)pBlock->axis_start[a];
    double sv = (double)pBlock->axis_start[b];
    double eu = (double)pBlock->axis_target[a];
    double ev = (double)pBlock->axis_target[b];
    double radius = fabs((double)pBlock->arc_radius);
    double dx = eu - su;
    double dy = ev - sv;
    double chord = sqrt((dx * dx) + (dy * dy));
    double h;
    double cx1;
    double cy1;
    double sweep1;
    uint8_t wantSmall;

    if ((radius <= 0.0) || (chord <= (double)NC_ARC_TOLERANCE) ||
        (chord > ((2.0 * radius) + (double)NC_ARC_TOLERANCE))) {
        *pOutError = NC_ERR_ARC_GEOMETRY;
        return -1;
    }

    h = (radius * radius) - ((chord * chord) * 0.25);
    h = (h > 0.0) ? sqrt(h) : 0.0;
    cx1 = ((su + eu) * 0.5) + ((-dy / chord) * h);
    cy1 = ((sv + ev) * 0.5) + ((dx / chord) * h);
    sweep1 = fabs(NcInterp_SignedSweep(pBlock->motion_type,
                                       /* Apply the next logical update for this processing stage. */
                                       su, sv, eu, ev, cx1, cy1));
    wantSmall = (uint8_t)(pBlock->arc_radius > 0);

    if (((sweep1 <= (NC_PI + 0.000001)) && (wantSmall != 0U)) ||
        ((sweep1 > (NC_PI + 0.000001)) && (wantSmall == 0U))) {
        pBlock->arc_center[a] = RoundFixed(cx1);
        pBlock->arc_center[b] = RoundFixed(cy1);
    } else {
        pBlock->arc_center[a] =
            RoundFixed(((su + eu) * 0.5) - ((-dy / chord) * h));
        pBlock->arc_center[b] =
            RoundFixed(((sv + ev) * 0.5) - ((dx / chord) * h));
    }
    pBlock->arc_radius = RoundFixed(radius);
    return 0;
}

/**
 * @brief Handle nc interp prepare arc ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pPathLengthUnits Pointer to path length units used by the function.
 * @param pOutError Output pointer that receives the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterp_PrepareArcTs(NC_EXEC_BLOCK* pBlock,
                              double* pPathLengthUnits,
                              NC_ERROR_CODE* pOutError)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t a;
    uint32_t b;
    double rs;
    double re;
    double sweep;
    double arcLen;
    double outSum = 0.0;
    uint32_t i;
    uint8_t samePlaneEndpoint;

    if (((pBlock->modal_flags & NC_ARC_FLAG_IJK) != 0U) &&
        ((pBlock->modal_flags & NC_ARC_FLAG_R) != 0U)) {
        *pOutError = NC_ERR_ARC_GEOMETRY;
        return -1;
    }
    NcInterp_PlaneAxes(pBlock->plane, &a, &b);
    if ((pBlock->modal_flags & NC_ARC_FLAG_R) != 0U) {
        if (PrepareArcCenterFromR(pBlock, a, b, pOutError) != 0) {
            return -1;
        }
    } else if ((pBlock->modal_flags & NC_ARC_FLAG_IJK) == 0U) {
        *pOutError = NC_ERR_ARC_GEOMETRY;
        return -1;
    }

    rs = hypot((double)pBlock->axis_start[a] - (double)pBlock->arc_center[a],
               (double)pBlock->axis_start[b] - (double)pBlock->arc_center[b]);
    re = hypot((double)pBlock->axis_target[a] - (double)pBlock->arc_center[a],
               (double)pBlock->axis_target[b] - (double)pBlock->arc_center[b]);
    if ((rs <= 0.0) || (fabs(rs - re) > (double)NC_ARC_TOLERANCE)) {
        *pOutError = NC_ERR_ARC_GEOMETRY;
        return -1;
    }
    samePlaneEndpoint =
        (uint8_t)((fabs((double)pBlock->axis_start[a] -
                        /* Apply the next logical update for this processing stage. */
                        (double)pBlock->axis_target[a]) <=
                   (double)NC_ARC_TOLERANCE) &&
                  (fabs((double)pBlock->axis_start[b] -
                        (double)pBlock->axis_target[b]) <=
                   (double)NC_ARC_TOLERANCE));
    if (((pBlock->modal_flags & NC_ARC_FLAG_R) != 0U) &&
        (samePlaneEndpoint != 0U)) {
        *pOutError = NC_ERR_ARC_GEOMETRY;
        return -1;
    }

    pBlock->arc_radius = RoundFixed(rs);
    for (i = 0U; i < AXIS_MAX; ++i) {
        if ((i != a) && (i != b)) {
            double d =
                (double)pBlock->axis_target[i] - (double)pBlock->axis_start[i];
            outSum += d * d;
        }
    }
    if (samePlaneEndpoint != 0U) {
        *pPathLengthUnits = sqrt(outSum) / (double)NC_POSITION_SCALE;
        return 0;
    }
    sweep = NcInterp_SignedSweep(pBlock->motion_type,
                                 (double)pBlock->axis_start[a],
                                 (double)pBlock->axis_start[b],
                                 (double)pBlock->axis_target[a],
                                 (double)pBlock->axis_target[b],
                                 (double)pBlock->arc_center[a],
                                 (double)pBlock->arc_center[b]);
    arcLen = fabs(sweep) * rs;
    *pPathLengthUnits = sqrt((arcLen * arcLen) + outSum) /
                        /* Apply the next logical update for this processing stage. */
                        (double)NC_POSITION_SCALE;
    return 0;
}

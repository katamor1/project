/* ibm-bob/samples/base/source/inc/nc_interp_math.h */
/* Declares small arc geometry helpers used by NC interpolation. */
/* This exists so interpolation stepping stays short and readable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_interp_math.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_INTERP_MATH_H
#define NC_INTERP_MATH_H

#include "system_shared.h"

/**
 * @brief Handle NC interp plane axes for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param plane Input value for plane.
 * @param pA Pointer to a used by this operation.
 * @param pB Pointer to b used by this operation.
 */
void NcInterp_PlaneAxes(NC_PLANE plane, uint32_t* pA, uint32_t* pB);
/**
 * @brief Handle NC interp signed sweep for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param motion Input value for motion.
 * @param su Input value for su.
 * @param sv Input value for sv.
 * @param eu Input value for eu.
 * @param ev Input value for ev.
 * @param cu Input value for cu.
 * @param cv Input value for cv.
 * @return Computed floating-point result for the current operation.
 */
double NcInterp_SignedSweep(NC_MOTION_TYPE motion,
                            double su,
                            double sv,
                            double eu,
                            double ev,
                            double cu,
                            double cv);
/**
 * @brief Prepare arc TS for later execution.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pPathLengthUnits Pointer to path length units used by this operation.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterp_PrepareArcTs(NC_EXEC_BLOCK* pBlock,
                              double* pPathLengthUnits,
                              NC_ERROR_CODE* pOutError);

#endif /* NC_INTERP_MATH_H */

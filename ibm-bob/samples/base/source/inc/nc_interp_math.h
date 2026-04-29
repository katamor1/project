/* ibm-bob/samples/base/source/inc/nc_interp_math.h */
/* Declares small arc geometry helpers used by NC interpolation. */
/* This exists so interpolation stepping stays short and readable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_interp_math.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_INTERP_MATH_H
#define NC_INTERP_MATH_H

#include "system_shared.h"

void NcInterp_PlaneAxes(NC_PLANE plane, uint32_t* pA, uint32_t* pB);
double NcInterp_SignedSweep(NC_MOTION_TYPE motion,
                            double su,
                            double sv,
                            double eu,
                            double ev,
                            double cu,
                            double cv);
int32_t NcInterp_PrepareArcTs(NC_EXEC_BLOCK* pBlock,
                              double* pPathLengthUnits,
                              NC_ERROR_CODE* pOutError);

#endif /* NC_INTERP_MATH_H */

/* ibm-bob/samples/base/source/inc/nc_coordinate.h */
/* Declares TS-side coordinate state helpers for NC program parsing. */
/* This exists so parser code converts program coordinates before RT consumption. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_coordinate.c, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_COORDINATE_H
#define NC_COORDINATE_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC coordinate state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCoordinate_Reset(void);
/**
 * @brief Set unit mode in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param mode Input value for mode.
 */
void NcCoordinate_SetUnitMode(NC_UNIT_MODE mode);
/**
 * @brief Set distance mode in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param mode Input value for mode.
 */
void NcCoordinate_SetDistanceMode(NC_DISTANCE_MODE mode);
/**
 * @brief Handle NC coordinate select work for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param workIndex Index selecting work.
 */
void NcCoordinate_SelectWork(uint8_t workIndex);
/**
 * @brief Clear local shift state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCoordinate_ClearLocalShift(void);
/**
 * @brief Set local shift axis in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisIndex Index selecting axis.
 * @param shift Input value for shift.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetLocalShiftAxis(int32_t axisIndex, int32_t shift);
/**
 * @brief Set work offset axis in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param workIndex Index selecting work.
 * @param axisIndex Index selecting axis.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex,
                                       int32_t axisIndex,
                                       int32_t offset);
/**
 * @brief Set program axis in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisIndex Index selecting axis.
 * @param programPosition Input value for program position.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex,
                                    int32_t programPosition,
                                    NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set machine axis in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisIndex Index selecting axis.
 * @param machinePosition Input value for machine position.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetMachineAxis(int32_t axisIndex,
                                    int32_t machinePosition,
                                    NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set temporary axis in NC coordinate.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisIndex Index selecting axis.
 * @param programPosition Input value for program position.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetTemporaryAxis(int32_t axisIndex,
                                      int32_t programPosition);
/**
 * @brief Handle NC coordinate fill targets for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcCoordinate_FillTargets(NC_EXEC_BLOCK* pBlock);

#endif /* NC_COORDINATE_H */

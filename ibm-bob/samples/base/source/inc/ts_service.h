/* ibm-bob/samples/base/source/inc/ts_service.h */
/* Declares non-real-time service slices for logs, prefetch, and UI snapshots. */
/* This exists so slower service work stays outside the fixed RT dispatcher. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/ts_service.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef TS_SERVICE_H
#define TS_SERVICE_H

/**
 * @brief Run one TS service time-share service slice.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void TsService_ExecuteSlice(void);

/**
 * @brief Handle TS service handle log flush for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void TsService_HandleLogFlush(void);

/**
 * @brief Write pending logs for the smoke-test scenario.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void TsService_WritePendingLogs(void);

/**
 * @brief Handle TS service execute prefetch for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void TsService_ExecutePrefetch(void);

/**
 * @brief Build UI snapshot from current shared state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void TsService_BuildUiSnapshot(void);

#endif /* TS_SERVICE_H */

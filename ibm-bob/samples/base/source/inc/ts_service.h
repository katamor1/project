#ifndef TS_SERVICE_H
#define TS_SERVICE_H

void TsService_ExecuteSlice(void);
void TsService_HandleLogFlush(void);
void TsService_WritePendingLogs(void);
void TsService_ExecutePrefetch(void);
void TsService_BuildUiSnapshot(void);

#endif /* TS_SERVICE_H */

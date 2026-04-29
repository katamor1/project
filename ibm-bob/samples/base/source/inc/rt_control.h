#ifndef RT_CONTROL_H
#define RT_CONTROL_H

void RtDispatcher_ExecuteCycle(void);
void RtInput_ScanInputs(void);
void RtControl_UpdateMachineState(void);
void RtControl_UpdatePrestartInterlock(void);
void RtControl_CaptureIoTrace(void);
void RtControl_ProcessModeRequest(void);
void RtControl_UpdateInterlock(void);
void RtControl_UpdateAxisTargets(void);
void RtOutput_CommitOutputs(void);

#endif /* RT_CONTROL_H */

/* ibm-bob/samples/base/source/inc/rt_control.h */
/* Declares the fixed-cycle real-time dispatcher and RT control stages. */
/* This exists so RT stage order stays visible to callers and test code. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef RT_CONTROL_H
#define RT_CONTROL_H

/**
 * @brief Handle RT dispatcher execute cycle for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtDispatcher_ExecuteCycle(void);

/**
 * @brief Handle RT input scan inputs for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtInput_ScanInputs(void);

/**
 * @brief Update machine state from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtControl_UpdateMachineState(void);

/**
 * @brief Update prestart interlock from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtControl_UpdatePrestartInterlock(void);

/**
 * @brief Handle RT control capture IO trace for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtControl_CaptureIoTrace(void);

/**
 * @brief Process pending mode request work.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtControl_ProcessModeRequest(void);

/**
 * @brief Update interlock from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtControl_UpdateInterlock(void);

/**
 * @brief Update axis targets from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtControl_UpdateAxisTargets(void);

/**
 * @brief Commit outputs into the active queue or output state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtOutput_CommitOutputs(void);

#endif /* RT_CONTROL_H */

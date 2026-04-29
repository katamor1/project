# IBM-Bob design-document implementation report

## Scope implemented

The uploaded basic/detail design documents describe a broad NC/machine-control feature set. This implementation adds a compiled, RT/TS-safe first implementation layer to the supplied `source` tree, focused on the shared execution model, NC block normalization, and fixed-cycle RT consumption rather than full machine-geometry fidelity for every specialized CNC function.

This continuation builds on the previous implementation and adds the next safety/control layer requested by the design documents: pre-start interlock aggregation, M-code response wait handling, alarm I/O trace capture, and more decimal G-code coverage.

## Main changes from the first implementation

- Added pre-start interlock aggregation shared memory and API.
  - New `PRESTART_INTERLOCK_STATUS g_prestartInterlockStatus`.
  - New `Api_GetPrestartInterlockStatus()`.
  - RT cycle now evaluates emergency stop, servo alarm, ready switch, pressure range, machine alarm, and NC buffer readiness before AUTO/NC start acceptance.
- Added M-code MFIN response-wait handling.
  - `M06` is identified as an MFIN-wait M-code through the M-code table.
  - Parser now sets `NC_AUX_FLAG_MFIN_WAIT` on fixed `NC_EXEC_BLOCK` output.
  - RT consumption outputs the M-code, stops consuming subsequent NC blocks, and resumes only after `SENSOR_SEQ_MFIN_BIT` appears in `g_ioIn.seq_bits`.
  - New `NC_AUX_STATUS g_ncAuxStatus` and `Api_GetNcAuxStatus()` expose active M-code, wait line, wait cycles, completed M-codes, and generation.
- Added alarm-triggered I/O trace ring buffer.
  - New `IO_TRACE_BUFFER g_ioTraceBuffer` and `Api_GetIoTraceBuffer()`.
  - RT captures sensor bits, sequence bits, command bits, aux command, spindle/tool outputs, axis targets, run mode, machine state, NC state, and alarm code each cycle.
  - On machine alarm, the ring freezes with trigger cycle/alarm metadata so TS/UI/API can inspect the pre-alarm window without doing file I/O in RT.
- Added decimal/advanced G-code coverage.
  - Added `G06.2` for NURBS-like fixed-block handling.
  - Added `G07.1`, `G12.1`, and `G13.1` for cylindrical/polar/advanced interpolation mode signaling.
  - Added grinding-cycle range `G70.7` through `G75.7` as canned-cycle fixed blocks.
  - Added `G76.7` as a special threading fixed block.
- Extended smoke coverage.
  - The smoke NC program now includes `M06 T2`, `G70.7`, `G06.2`, and `G12.1` in addition to the previous canned cycle, skip, high-precision smoothing, sync on/off, and decimal interpolation samples.

## Main changes from the uploaded baseline source

- Added normalized G-code handling for decimal G-codes such as `G02.3`, `G03.3`, `G05.2`, `G06.2`, `G07.1`, `G10.6`, `G12.1`, `G13.1`, `G12.4`, `G13.4`, `G50.4`, `G51.4`, `G50.5`, `G51.5`, `G50.6`, `G51.6`, and `G70.7` through `G76.7`.
- Expanded the G-code table to cover canned cycles, grinding cycles, threading, skip/reference return, high-precision/smoothing, sync/compound/overlay, coordinate rotation, tool compensation, feed modes, return-plane modes, and advanced interpolation modes.
- Added support for NC comments, percent delimiters, sequence/program numbers (`N`, `O`), `Q` words, and incremental `U/V/W` axis words.
- Added shared-memory feature status `g_ncFeatureStatus` and public API `Api_GetNcFeatureStatus()`.
- Extended `NC_EXEC_BLOCK` so TS parsing can pass feature flags, cycle codes, R/Q/P parameters, normalized G-code identity, and MFIN-wait auxiliary command identity to RT.
- Extended RT block consumption so feature-state changes and M-code handshake waits are committed in RT and exposed to UI/API snapshots without doing file I/O or text parsing in RT.
- Extended interpolation so canned-cycle, thread, skip, reference-return, and advanced-interpolation blocks execute through the fixed-size block pipeline using safe linearized target stepping in this sample implementation.
- Updated the smoke program to cover comments, sequence numbers, canned cycle `G83`, grinding cycle `G70.7`, NURBS-like `G06.2`, polar/cylindrical sample `G12.1`, `G80`, MFIN-wait `M06`, `G05.2`, `G51.4/G50.4`, `G31`, and `G02.3`.

## Verification

The source was compiled and executed in the container with:

```bash
gcc -std=c99 -Wall -Wextra -Iinc src/*.c -lm -o build_test
./build_test
```

Observed smoke-test output:

```text
nc_state=6 nc_exec=27 nc_x=1200 nc_y=2300 nc_z=3400 nc_m=130
interp_segments=11 feed_profile=1 feed_override=100 dwell_ticks=1
coord_unit=0 coord_distance=0 coord_work=0 coord_local=0 coord_work_x=1000
feature_blocks=21 feature_flags=97 canned=2 skip=1 advanced=3
aux_completed=1 aux_wait_cycles=43 prestart_ok=0 prestart_mask=0x00000014
trace_frozen=1 trace_captured=32 trace_trigger_cycle=141
mode=1 state=3 alarm=9001 cycle=160
coord_error_checks=5
```

Interpretation:

- `aux_completed=1` confirms the `M06`/MFIN wait path completed.
- `aux_wait_cycles=43` confirms RT stopped consuming later NC blocks while waiting for MFIN.
- `trace_frozen=1` and `trace_captured=32` confirm the alarm-triggered I/O trace ring froze after the pressure-high alarm.
- `feature_blocks=21`, `canned=2`, and `advanced=3` confirm the additional G70.7/G06.2/G12.1 path is reaching the feature-status layer; `G06.2` and `G12.1` also execute as active advanced-interpolation blocks in the fixed RT pipeline.
- `coord_error_checks=5` confirms the retained parser error checks still pass.

## Known limits

- Specialized geometric algorithms such as exact NURBS, true 3D interference checking, free-form high-precision contouring, grinding-cycle path decomposition, and machine-specific synchronized multi-axis behavior are represented by feature flags/status and safe linearized execution blocks in this sample layer.
- The MFIN handshake currently uses a single active auxiliary wait slot, which matches the fixed-block sample architecture. A production machine-control implementation should add machine-specific timeout values, retry policy, and PLC sequence-code mapping.
- The I/O trace ring captures fixed shared-memory snapshots only. Long-term persistence remains a TS-side responsibility.
- The implementation keeps the original fixed-size shared-memory and RT constraints; no file I/O, dynamic allocation, or text parsing was added to RT code.

# IBM-Bob additional feature implementation report v3

## Scope

This continuation builds on v2 and deepens the execution layer for design-document functions that were previously represented mainly as G-code registration and fixed-block status exposure.

## Implemented additions in v3

### 1. Canned-cycle RT segment planner

Added an RT-safe internal segment planner in `src/nc_interpolation.c`.

Supported behavior:
- Converts a canned-cycle block into fixed-size internal segments.
- Uses a static `NC_ACTIVE_SEGMENT` array; no dynamic memory is used.
- Supports rapid approach to R plane, feed to depth, optional peck drilling via `Q`, optional dwell via `P`, and retract according to `G98/G99`.
- Keeps RT-side text parsing and file I/O prohibited.

Primary target examples:
- `G81`, `G82`, `G83`, `G73`, `G85`, `G86`, `G88`, `G89`
- `G70.7` and other registered canned/grinding cycle placeholders execute through the same segment layer when represented as canned-cycle blocks.

### 2. G31 skip/probe RT detection

Added skip/probe completion handling for `G31`.

Behavior:
- Detects `SENSOR_SEQ_SKIP_BIT` during RT interpolation.
- Completes the active skip block immediately.
- Stores the detected axis target position in shared status.
- Does not require TS-side polling.

### 3. NC cycle shared status and public API

Added:
- `NC_CYCLE_STATUS`
- `g_ncCycleStatus`
- `Api_GetNcCycleStatus()`

Status includes:
- active flag
- line number
- cycle/G-code identifier
- segment phase index and phase count
- completed canned cycle count
- completed skip count
- last skip cycle
- last skip position

### 4. Smoke test expansion

Updated `src/main.c` to confirm:
- canned-cycle segment execution
- peck cycle execution
- G31 skip detection
- M06/MFIN wait completion
- post-completion alarm trace freeze
- NC program completion before intentional alarm injection

## RT/TS safety notes

- All file I/O and NC text parsing remain in TS processing.
- RT receives fixed-size `NC_EXEC_BLOCK` data only.
- The new cycle plan uses static arrays sized by `NC_CANNED_MAX_SEGMENTS`.
- No malloc/free, Windows API calls, file access, or unbounded text processing were added to RT.
- The peck planner has an upper segment bound and returns an error if the bound would be exceeded.

## Remaining limitations

This is still a sample implementation layer, not a full machine-tool geometry kernel.

Not fully implemented:
- complete NURBS/free-surface interpolation mathematics
- full grinding-cycle machine semantics for all G70.7-G76.7 variants
- full cutter compensation and 3D tool compensation geometry
- exact machine-specific acceleration/deceleration and servo constraints
- full M-code PLC sequence tables beyond sample MFIN wait handling

The implementation now provides a stronger extension point: parsed design-document functions can be represented as fixed execution blocks, internal RT segment plans, and shared/public status snapshots.

# IBM-Bob source continuation v8 implementation report

## Scope

This v8 continuation implements the additional motion-controller command smoothing requirement and hardens the v7 diagnostic outputs so they affect execution behavior.

## Implemented items

### 1. Two-stage RT motion smoothing filter

Added `nc_motion_filter.h` / `nc_motion_filter.c`.

The filter is applied to interpolated per-axis position targets immediately before writing the command stream to the motion-controller output area.

Processing order:

1. Interpolator outputs the raw per-axis target for the current RT cycle.
2. The raw position delta is treated as the per-cycle velocity command.
3. Stage 1 applies a moving-average filter to the velocity command.
4. Stage 2 applies either:
   - moving-average filter, or
   - fixed FIR filter `{1, 2, 3, 2, 1}`
5. The second-stage velocity is integrated back into a smoothed position target.
6. The final interpolation tick is snapped to the exact block endpoint to avoid residual position error in this sample implementation.

RT constraints:

- No file I/O.
- No dynamic allocation.
- Fixed-size per-axis ring buffers only.
- Filter state is exposed through shared memory.

### 2. Motion filter API and shared memory

Added:

- `NC_MOTION_FILTER_STATUS`
- `g_ncMotionFilterStatus`
- `Api_SetNcMotionFilterConfig()`
- `Api_GetNcMotionFilterStatus()`

The API allows selecting stage-2 moving average or FIR smoothing and configuring the stage windows within fixed maximum sizes.

### 3. Axis overload staged deceleration applied to execution

v7 exposed staged deceleration as a diagnostic request. v8 now applies it to the active interpolation plan by scaling `interp_ticks`, segment end ticks, acceleration ticks, and deceleration ticks according to `decel_override_percent`.

This keeps RT execution deterministic while slowing the motion command stream when load warning/limit states are active.

### 4. Tool-life expired operation hold

Tool-life expiry now moves NC execution to `NC_HOLD` and sets `NC_ERR_TOOL_LIFE_EXPIRED`.

Added event/log output for the hold request so operation can distinguish preventive hold from parser or interlock errors.

### 5. Fixed-length NC event ring

Added:

- `NC_EVENT_ITEM`
- `NC_EVENT_RING`
- `g_ncEventRing`
- `NcEvent_Push()`
- `Api_GetNcEventRing()`

Events currently include:

- staged deceleration applied
- tool-life expired hold
- motion filter first application

### 6. Smoke test updates

`main.c` now verifies:

- existing v7 NC program still reaches `NC_COMPLETED`
- motion filter status is populated
- FIR mode is selected
- staged deceleration is applied to feed/interpolation
- a second short NC program triggers tool-life expiration and `NC_HOLD`
- the NC event ring records the tool-life hold event
- I/O trace still freezes on later alarm

## Verification command

```text
gcc -std=c99 -Wall -Wextra -Iinc src/control_api.c src/main.c src/nc_buffer.c src/nc_codes.c src/nc_compensation.c src/nc_coordinate.c src/nc_diagnostics.c src/nc_feed.c src/nc_interp_math.c src/nc_interpolation.c src/nc_kinematics.c src/nc_lookahead.c src/nc_motion_filter.c src/nc_parser.c src/nc_parser_modal.c src/nc_parser_tokens.c src/nc_program.c src/rt_control.c src/system_shared.c src/ts_service.c -lm -o /tmp/build_v8_test
/tmp/build_v8_test
```

See `verification_output_v8.txt` for the captured output.

## Notes and limitations

The motion smoothing filter is a deterministic sample implementation. For a real machine controller, the following should be refined in the detailed control layer:

- filter delay compensation
- endpoint snap avoidance by lookahead queue blending
- jerk-limited S-curve generation
- per-axis servo limit tables
- machine-dependent FIR coefficients
- interaction with tool radius compensation and 5-axis TCP

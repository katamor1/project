# IBM-Bob source additional implementation v19

## Scope
v19 adds an axis configuration layer for design items that were still thin in the v18 source:

- Axis setting: linear/rotary axis type and logical axis name ownership
- Axis name / coordinate group assignment status
- Flexible path-axis allocation mask
- Control-axis detach mask
- Diameter/radius command conversion at the TS fixed-block stage
- Public API and shared-memory status exposure for the above

## Added files

- `source/inc/nc_axis_config.h`
- `source/src/nc_axis_config.c`

## Modified files

- `source/inc/system_config.h`
- `source/inc/system_shared.h`
- `source/inc/control_api.h`
- `source/src/system_shared.c`
- `source/src/control_api.c`
- `source/src/nc_parser_modal.c`
- `source/src/main.c`
- `source/scripts/build_vc2026.bat`
- `source/scripts/build_clang.bat`

## Design notes

The module is placed in the TS-side fixed-block preparation path. It updates `NC_EXEC_BLOCK` before RT execution so that the RT cycle does not need to perform axis-table lookup, path-axis filtering, detached-axis decisions, or diameter/radius conversion.

`NC_AXIS_CONFIG_STATUS` is a fixed-size shared-memory status structure. It records axis type/name, active path mask, diameter-mode mask, detached-axis mask, per-block conversion counters, rejected API calls, and the latest input/output axis targets.

## Verification

The following changed translation units were checked with GCC syntax-only builds:

- `src/nc_axis_config.c`
- `src/system_shared.c`
- `src/control_api.c`
- `src/nc_parser_modal.c`
- `src/main.c`

A full all-source link was attempted in this execution environment, but the large batch build command repeatedly timed out. The v19 package therefore includes the source updates and the syntax-check log for the changed files.

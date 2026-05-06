# IBM-Bob source implementation report v24_20

## Scope

Based on the v23_20 source package, this update implements 20 additional items that were still only partially represented by design/backlog entries. The implementation keeps the IBM-Bob baseline constraints: TS-side parsing and fixed-block transformation, RT-side bounded observation/execution accounting, no dynamic allocation in RT paths, and no file I/O or Windows API calls in RT code.

## Changed files

- `source/inc/system_config.h`
  - `NC_IMPL_BACKLOG_FEATURE_COUNT` changed from `120U` to `140U`.
  - `NC_IMPL_BACKLOG_WORD_COUNT` changed from `4U` to `5U`.
  - Added `NC_IMPL_BACKLOG_V24_FIRST_FEATURE` and `NC_IMPL_BACKLOG_V24_ADDED_FEATURES`.
- `source/src/nc_feature_backlog.c`
  - Added descriptors for feature IDs `120` to `139`.
  - Added deterministic TS-side policies for feature IDs `120` to `139`.
  - Added bounded helper functions for safe division and feature/tick adjustment.
- `source/scripts/build_gcc_smoke_v24.sh`
  - Added GCC smoke build script.
- `source/scripts/test_gcc_smoke_v24.sh`
  - Added GCC smoke test script and verification output capture.

## Added 20 features

| ID | Feature | TS-side implementation summary |
|---:|---|---|
| 120 | 5-axis RTCP tool-tip correction | Applies tool-axis command, normalizes target axes, and extends interpolation ticks. |
| 121 | Tilted work-plane B/C angle normalization | Applies tilt command, normalizes rotary axes, and marks tilted-plane processing. |
| 122 | TCPM tool-center-point feed correction | Applies guarded feed reduction and tool-axis direction flag. |
| 123 | High-speed tiny-segment compression | Detects small X travel and applies smoothing / tick compaction. |
| 124 | Curvature-continuous lookahead acceleration | Adds advanced interpolation flag, accel/decel margin, and feed reduction. |
| 125 | Jerk-limited S-curve acceleration | Uses trapezoid-compatible profile state, extends accel/decel ticks, and marks smoothing. |
| 126 | Automatic corner-radius insertion | Adds R-based corner adjustment and minimum interpolation ticks. |
| 127 | Automatic chamfer insertion | Adds Q-based chamfer end-point trim and smoothing. |
| 128 | Lookahead low-watermark HOLD-avoidance slowdown | Applies conservative feed reduction and longer interpolation duration. |
| 129 | NC sequence-search resume-position verification | Sets sequence P word, filters path axis mask, and adds minimum ticks. |
| 130 | Dual PLC auxiliary-code queue guard | Adds M-code/MFIN wait guard and bounded dwell. |
| 131 | Coolant / air-blow M-code interlock | Adds MFIN guarded M-code and minor feed reduction. |
| 132 | Constant surface speed spindle zero-diameter guard | Guards X diameter zero, sets spindle fallback, and uses feed-per-rev. |
| 133 | Automatic spindle gear range selection | Selects M41/M42-style gear command based on spindle speed. |
| 134 | ATC home-position confirmation | Adds M06/MFIN wait, tool fallback, and retract flag. |
| 135 | Tool wear fine adjustment | Applies H offset fallback and small Z-axis correction. |
| 136 | Probe measurement result work-offset reflection | Converts to skip motion and applies small XYZ work-mount trim. |
| 137 | Grinding dresser amount compensation | Marks grinding-style canned cycle, applies Z dresser amount, and reduces feed. |
| 138 | Restart inhibit latch after emergency-stop recovery | Applies near-stop feed, MFIN wait latch, and long safe ticks. |
| 139 | Door-open safe speed monitoring | Applies guarded safe-speed override and longer interpolation duration. |

## Verification

Command executed:

```sh
cd source
bash scripts/test_gcc_smoke_v24.sh
```

Key result:

```text
implementation_backlog implemented=140 configured=140 ts=430 rt=198 synthetic=140 cat_mask=0x000003ff last_id=139 last_cat=9 last_policy=7 slowdown=20 cat0=84 cat9=68 last_x=1139
nc_state=4 nc_error=0
cycle_error_checks=4
```

## Notes

- GCC smoke build/test passed in this environment.
- Windows VC2026 / clang scripts were not executed in this Linux sandbox; run `scripts\\build_vc2026.bat` and `scripts\\build_clang.bat` after extracting on Windows.

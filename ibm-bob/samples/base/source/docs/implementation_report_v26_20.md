# IBM-Bob source implementation report v26_20

## Summary

Based on `ibm_bob_source_implemented_additional_features_v25_20.zip`, this update adds 20 more implementation-backlog features as v26_20.

The implementation keeps the IBM-Bob RT/TS separation policy:

- TS side: deterministic fixed-size `NC_EXEC_BLOCK` transformations in `NcFeatureBacklog_ApplyBlockTs()`.
- RT side: no dynamic allocation, no file I/O, no Windows API; only fixed-size status/event accounting through `NcFeatureBacklog_OnBlockRt()`.
- API/status side: fixed-size masks and counters through `NC_IMPLEMENTATION_BACKLOG_STATUS`.

## Changed files

- `source/inc/system_config.h`
- `source/inc/system_shared.h`
- `source/src/nc_feature_backlog.c`
- `source/scripts/build_gcc_smoke_v26.sh`
- `source/scripts/test_gcc_smoke_v26.sh`
- `source/docs/verification_output_v26_20.txt`
- `source/docs/continuation_v26_20_diff.patch`

## Configuration changes

- `NC_IMPL_BACKLOG_FEATURE_COUNT`: `160U` -> `180U`
- `NC_IMPL_BACKLOG_WORD_COUNT`: `5U` -> `6U`
- Added `NC_IMPL_BACKLOG_V26_FIRST_FEATURE` = `160U`
- Added `NC_IMPL_BACKLOG_V26_ADDED_FEATURES` = `20U`

## Newly implemented features ID 160-179

1. ID160: Cross-path contour blend gap clamp
2. ID161: Dry-run feed cap
3. ID162: Restart modal-state consistency check
4. ID163: M19 spindle-orientation position window monitor
5. ID164: Rigid tapping spindle/feed phase monitor
6. ID165: Thread retract chamfer-vector correction
7. ID166: Deep-hole chip evacuation dwell guard
8. ID167: Grinding spark-out dwell guard
9. ID168: Pallet-clamp confirmation interlock
10. ID169: Hydraulic-pressure staged deceleration
11. ID170: Servo following-error moving-average prewarning
12. ID171: Absolute-encoder battery warning latch
13. ID172: Soft-limit precheck path clamp
14. ID173: Stroke-end override prohibition
15. ID174: External feed-hold input debounce
16. ID175: Optional stop M01 state gate
17. ID176: Block-skip command decision stabilization
18. ID177: Single-block stop point generation
19. ID178: Override knob rate limiter
20. ID179: Alarm-reset interlock release sequence

## TS-side transformation policy

The v26 cases are implemented in `ApplyV23Policy()` because the backlog implementation path promotes all features from ID100 onward to deterministic TS transformations. The new handlers perform bounded transformations such as:

- axis target clamp/normalization;
- feed cap or override guard;
- interpolation/deceleration tick increase;
- auxiliary M-code handshake requirements;
- cycle flag and dwell guard insertion;
- modal consistency and generation update.

## Verification

GCC smoke build/test was executed successfully:

```bash
cd source
bash scripts/test_gcc_smoke_v26.sh
```

Key result:

```text
implementation_backlog implemented=180 configured=180 ts=480 rt=238 synthetic=180 cat_mask=0x000003ff last_id=179 last_cat=9 last_policy=7 slowdown=30 cat0=92 cat9=77 last_x=1179
nc_state=4 nc_error=0
cycle_error_checks=4
```

## Notes for Windows verification

This sandbox verified GCC smoke build/test only. For the Windows project workspace, run the existing scripts after ZIP extraction:

```bat
cd source
scripts\build_vc2026.bat
scripts\test_vc2026.bat
scripts\build_clang.bat
scripts\test_clang.bat
```

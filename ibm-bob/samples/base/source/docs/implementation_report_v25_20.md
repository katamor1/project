# IBM-Bob source implementation report v25_20

## Summary

This update is based on `ibm_bob_source_implemented_additional_features_v24_20.zip` and adds another 20 implementation-backlog features as deterministic, fixed-size C code.

The update keeps the existing RT/TS split:

- TS side: `NcFeatureBacklog_ApplyBlockTs()` applies deterministic fixed-block transformations through `ApplyV23Policy()` for feature IDs 140-159.
- RT side: `NcFeatureBacklog_OnBlockRt()` remains allocation-free and records execution counters/events only.
- Shared status/API side: existing `NC_IMPLEMENTATION_BACKLOG_STATUS` and `Api_GetNcImplementationBacklogStatus()` now cover 160 feature entries.

## Modified files

- `source/inc/system_config.h`
  - `NC_IMPL_BACKLOG_FEATURE_COUNT` changed from 140 to 160.
  - Added `NC_IMPL_BACKLOG_V25_FIRST_FEATURE` and `NC_IMPL_BACKLOG_V25_ADDED_FEATURES`.
- `source/inc/system_shared.h`
  - Backlog status comment updated from v22/v24 scale to v22-v25 / 160 entries.
- `source/src/nc_feature_backlog.c`
  - Added descriptors for feature IDs 140-159.
  - Added TS-side fixed-block implementation cases for feature IDs 140-159.
- `source/scripts/build_gcc_smoke_v25.sh`
  - Added GCC smoke build script for v25.
- `source/scripts/test_gcc_smoke_v25.sh`
  - Added GCC smoke test script for v25.

## Added feature IDs

| ID | Feature |
|---:|---|
| 140 | 5-axis tool posture singularity approach slowdown |
| 141 | Tilted-plane retract vector limiting |
| 142 | NURBS control-point weight range monitor |
| 143 | Involute interpolation base-radius guard |
| 144 | 3D arc plane-normal normalization |
| 145 | High-speed binary NC block consistency check |
| 146 | AI contour learning correction clamp |
| 147 | Free-form surface mesh boundary slowdown |
| 148 | HPCC error-band slowdown |
| 149 | Exact-stop block-end synchronization |
| 150 | Multi-path synchronization wait timeout |
| 151 | Overlay control duplicate-axis detection |
| 152 | Double-table exchange-position interlock |
| 153 | Axis retract/return position clamp |
| 154 | Mirror-axis cutter compensation sign inversion |
| 155 | G54.1 extended work-coordinate number guard |
| 156 | Macro-variable safe assignment boundary guard |
| 157 | Subprogram nesting limit monitor |
| 158 | NC alarm-history event compression |
| 159 | Servo-ready wait auxiliary M-code monitoring |

## Verification

Executed in this environment:

```bash
cd source
bash scripts/test_gcc_smoke_v25.sh
```

Key output:

```text
implementation_backlog implemented=160 configured=160 ts=453 rt=218 synthetic=160 cat_mask=0x000003ff last_id=159 last_cat=4 last_policy=3 slowdown=25 cat0=92 cat9=68 last_x=1159
nc_state=4 nc_error=0
cycle_error_checks=4
```

## Notes

The added implementations are deterministic sample implementations aligned with the existing IBM-Bob simulation layer. They intentionally avoid dynamic allocation, file I/O, and OS calls inside RT-side handling. Windows VC2026 / clang validation should be run after extracting the ZIP on the target Windows development environment.

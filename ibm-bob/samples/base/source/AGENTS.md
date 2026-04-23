<!-- ibm-bob/samples/base/source/AGENTS.md -->
<!-- Defines repository-specific build, test, and editing rules for the IBM BOB baseline sample workspace. -->
<!-- This exists so IBM-Bob and other agents use the pinned scripts and respect the real-time code boundaries. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/docs/workspace_profile_commands.md, ibm-bob/samples/base/source/scripts/build_vc2026.bat, ibm-bob/samples/base/source/scripts/build_clang.bat -->
# AGENTS.md

## Repository purpose
This repository is a baseline sample for IBM BOB custom-agent evaluation.
The agent should inspect both documents and code, then assess impact from a newly supplied basic design.

## Build / test policy
Claude does not have a dedicated `build_command` / `test_command` key in `config.toml`.
For this repo, always use the scripts below as the source of truth.

### Preferred environment selection
- If the current shell is a VC2026 Developer Command Prompt and `cl.exe` is available, prefer the VC2026 scripts.
- Otherwise, if `clang-cl.exe` is available on `PATH` or under the Visual Studio LLVM toolchain, use the clang scripts.
- If `cl.exe` is not on `PATH`, the VC2026 scripts may bootstrap the environment through `C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1`.
- If `clang-cl.exe` is not on `PATH`, the clang scripts may use the Visual Studio LLVM install path directly or bootstrap through the same `Launch-VsDevShell.ps1`.
- Do not invent alternative compile commands unless the user explicitly asks.

### Workspace profiles and commands
#### Profile: `ibm-bob-vc2026`
- Build command: `cmd /c scripts\build_vc2026.bat`
- Test command: `cmd /c scripts\test_vc2026.bat`

#### Profile: `ibm-bob-clang-win`
- Build command: `cmd /c scripts\build_clang.bat`
- Test command: `cmd /c scripts\test_clang.bat`

## Test pass criteria
A test run passes only when all of the following are true:
1. The build exits with code 0.
2. `build\ibm_bob_sample.exe` exists.
3. Running the executable exits with code 0.
4. Standard output contains: `mode=1 state=3 alarm=9001 cycle=16`
5. `runtime_log.csv` is generated.

## Notes for edits
- Keep source files in `src/` and headers in `inc/`.
- Avoid adding Windows system calls into `rt_control.c`; those belong in `ts_service.c` or other non-real-time modules.
- Preserve the current baseline behavior unless the task explicitly requires changing it.

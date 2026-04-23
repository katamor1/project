<!-- ibm-bob/samples/base/source/docs/workspace_profile_commands.md -->
<!-- Lists the pinned workspace profile commands for the IBM BOB baseline sample. -->
<!-- This exists so agents can map profile ids to the exact script-based build and test commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/AGENTS.md, ibm-bob/samples/base/source/.claude/config.toml, ibm-bob/samples/base/source/scripts/build_vc2026.bat -->
# IBM BOB baseline sample: workspace profile commands

## Claude profile note
Claude supports project-scoped `.claude/config.toml` profiles, but build/test commands are documented for the agent through repository instructions such as `AGENTS.md`.
This repository therefore defines profiles in `.claude/config.toml` and pins the executable commands in `AGENTS.md`.

## Profiles
### ibm-bob-vc2026
- Build: `cmd /c scripts\build_vc2026.bat`
- Test: `cmd /c scripts\test_vc2026.bat`
- Intended shell: VC2026 Developer Command Prompt or a shell where `Launch-VsDevShell.ps1` can bootstrap `cl.exe`

### ibm-bob-clang-win
- Build: `cmd /c scripts\build_clang.bat`
- Test: `cmd /c scripts\test_clang.bat`
- Intended shell: environment where `clang-cl.exe` is on `PATH`, under the Visual Studio LLVM toolchain, or reachable after `Launch-VsDevShell.ps1`

## Smoke-test expectation
The sample binary is expected to print:
`mode=1 state=3 alarm=9001 cycle=16`
and also generate `runtime_log.csv`.

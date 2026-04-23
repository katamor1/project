@echo off
setlocal
cd /d "%~dp0.."

call scripts\build_clang.bat
if errorlevel 1 exit /b 1

if exist runtime_log.csv del /q runtime_log.csv
if exist build\test_output.txt del /q build\test_output.txt

echo [test_clang] Running smoke test...
build\ibm_bob_sample.exe > build\test_output.txt
if errorlevel 1 (
  echo ERROR: executable returned failure.
  exit /b 1
)

findstr /c:"mode=1 state=3 alarm=9001 cycle=16" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected output was not found.
  type build\test_output.txt
  exit /b 1
)

if not exist runtime_log.csv (
  echo ERROR: runtime_log.csv was not generated.
  exit /b 1
)

echo [test_clang] PASS
exit /b 0

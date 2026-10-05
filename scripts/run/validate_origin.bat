@echo off
setlocal
cd /d "%~dp0\..\.."
call scripts\run\build_origin.bat
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir build --output-on-failure -C Release
if errorlevel 1 exit /b %errorlevel%
if exist build\Release\origin.exe (
  build\Release\origin.exe
) else if exist build\origin.exe (
  build\origin.exe
) else (
  echo Origin executable not found.
  exit /b 1
)
if errorlevel 1 exit /b %errorlevel%
echo Origin validation completed.
exit /b 0

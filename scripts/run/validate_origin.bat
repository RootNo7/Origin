@echo off
setlocal
cd /d "%~dp0\..\.."
if not exist build\Release\origin.exe call scripts\run\build_origin.bat
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir build --output-on-failure -C Release
if errorlevel 1 exit /b %errorlevel%
build\Release\origin.exe
if errorlevel 1 exit /b %errorlevel%
echo Origin validation completed.

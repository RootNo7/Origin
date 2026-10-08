@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0.."
set "GODOT="
if defined GODOT3_EXECUTABLE set "GODOT=%GODOT3_EXECUTABLE%"
if not defined GODOT if exist "%~dp0Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0..\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0..\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT (
    for /f "delims=" %%G in ('where godot3.exe 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
    echo Origin requires Godot 3.6.x for tests.
    echo A generic "godot.exe" is intentionally rejected.
    echo Set GODOT3_EXECUTABLE to your Godot 3.6.x executable.
    exit /b 1
)
set "VERSION_FILE=%TEMP%\origin_godot_test_version_%RANDOM%.txt"
"%GODOT%" --version > "%VERSION_FILE%" 2>&1
set "VERSION="
set /p VERSION=<"%VERSION_FILE%"
del /q "%VERSION_FILE%" >nul 2>&1
echo Detected Godot: %VERSION%
echo.%VERSION%| findstr /r /b /c:"3\.6\." >nul
if errorlevel 1 (
    echo ERROR: selected executable is not Godot 3.6.x
    echo Selected executable: %GODOT%
    exit /b 2
)
"%GODOT%" --path "%CD%" --no-window --script res://tools/godot_self_test.gd
exit /b %ERRORLEVEL%

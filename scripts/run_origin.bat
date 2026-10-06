@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0.."
set "GODOT="
if defined GODOT3_EXECUTABLE set "GODOT=%GODOT3_EXECUTABLE%"
if not defined GODOT if exist "%~dp0Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0..\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0..\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT (
    for /f "delims=" %%G in ('where godot3 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
    echo.
    echo Origin requires Godot 3.6.x.
    echo A generic "godot" PATH entry is intentionally NOT used because it may be Godot 4.
    echo.
    echo Set GODOT3_EXECUTABLE to your Godot 3.6.x executable, or place
    echo Godot_v3.6.3-stable_win64.exe beside this project.
    exit /b 1
)

set "VERSION_FILE=%TEMP%\origin_godot_version_%RANDOM%.txt"
"%GODOT%" --version > "%VERSION_FILE%" 2>&1
set "VERSION="
set /p VERSION=<"%VERSION_FILE%"
del /q "%VERSION_FILE%" >nul 2>&1

echo Detected Godot: %VERSION%
echo.%VERSION%| findstr /r /b /c:"3\.6\." >nul
if errorlevel 1 (
    echo.
    echo ERROR: The selected executable is NOT Godot 3.6.x.
    echo Origin is locked to Godot 3.6.x and will not launch under Godot 4.
    echo Selected executable: %GODOT%
    exit /b 2
)

"%GODOT%" --path "%CD%"
exit /b %ERRORLEVEL%

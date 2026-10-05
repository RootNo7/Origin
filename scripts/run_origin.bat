@echo off
setlocal
cd /d "%~dp0\.."
set "GODOT="
if defined GODOT_EXECUTABLE set "GODOT=%GODOT_EXECUTABLE%"
if not defined GODOT if exist "%~dp0\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0..\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0..\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT (
    for /f "delims=" %%G in ('where godot3 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
    for /f "delims=" %%G in ('where godot 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
    echo Godot 3.6.x was not found.
    echo Download the standard Godot 3.6.3 Windows build and place it beside this project or add it to PATH.
    exit /b 1
)
"%GODOT%" --path "%CD%"
exit /b %ERRORLEVEL%

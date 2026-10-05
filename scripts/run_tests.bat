@echo off
setlocal
cd /d "%~dp0\.."
set "GODOT="
if defined GODOT_EXECUTABLE set "GODOT=%GODOT_EXECUTABLE%"
if not defined GODOT if exist "%~dp0\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0..\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0..\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0\godot3.exe" set "GODOT=%~dp0\godot3.exe"
if not defined GODOT if exist "%~dp0\godot.exe" set "GODOT=%~dp0\godot.exe"
if not defined GODOT (
  for /f "delims=" %%G in ('where godot3.exe 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
  for /f "delims=" %%G in ('where godot.exe 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
  echo Godot 3.6.x was not found.
  exit /b 1
)
"%GODOT%" --path "%CD%" --no-window --script res://tools/godot_self_test.gd
exit /b %ERRORLEVEL%

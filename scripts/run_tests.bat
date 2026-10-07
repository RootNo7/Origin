@echo off
setlocal
cd /d "%~dp0\.."
set "GODOT="
if defined GODOT_EXECUTABLE set "GODOT=%GODOT_EXECUTABLE%"
if not defined GODOT if exist "%~dp0\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0..\Godot_v3.6.3-stable_win64.exe" set "GODOT=%~dp0..\Godot_v3.6.3-stable_win64.exe"
if not defined GODOT if exist "%~dp0\godot3.exe" set "GODOT=%~dp0\godot3.exe"
if not defined GODOT (
  for /f "delims=" %%G in ('where godot3.exe 2^>nul') do if not defined GODOT set "GODOT=%%G"
)
if not defined GODOT (
  echo Godot 3.6.x was not found.
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
  echo ERROR: Tests require Godot 3.6.x.
  echo Selected executable: %GODOT%
  exit /b 2
)
"%GODOT%" --path "%CD%" --no-window --script res://tools/godot_self_test.gd
exit /b %ERRORLEVEL%

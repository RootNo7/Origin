@echo off
setlocal
cd /d "%~dp0\..\.."
call scripts\run\build_origin.bat
if errorlevel 1 exit /b %errorlevel%
if not exist observer\bridge mkdir observer\bridge
if not exist storage\saves mkdir storage\saves
if exist build\Release\origin.exe (
  build\Release\origin.exe --bridge
) else if exist build\origin.exe (
  build\origin.exe --bridge
) else (
  echo Origin executable not found.
  exit /b 1
)

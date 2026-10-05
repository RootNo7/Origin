@echo off
setlocal
cd /d "%~dp0\..\.."
call scripts\run\build_origin.bat
if errorlevel 1 exit /b %errorlevel%
if exist build\Release\origin.exe (
  build\Release\origin.exe
) else if exist build\origin.exe (
  build\origin.exe
) else (
  echo Origin executable not found.
  exit /b 1
)
exit /b %errorlevel%

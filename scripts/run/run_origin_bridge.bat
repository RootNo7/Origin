@echo off
cd /d "%~dp0\..\.."
if not exist build\Release\origin.exe call scripts\run\build_origin.bat
build\Release\origin.exe --bridge

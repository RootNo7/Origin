@echo off
setlocal
cmake --build build --config Release
if errorlevel 1 exit /b %errorlevel%
if not exist observer\bridge mkdir observer\bridge
if not exist storage\saves mkdir storage\saves
build\Release\origin.exe --bridge

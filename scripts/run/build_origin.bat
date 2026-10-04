@echo off
cd /d "%~dp0\..\.."
cmake -S . -B build || exit /b 1
cmake --build build --config Release || exit /b 1
pause

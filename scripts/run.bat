@echo off
cd /d "%~dp0\.."
where godot 2>nul >nul
if errorlevel 1 (
 echo Godot was not found in PATH.
 echo Import project.godot manually in Godot 3.x.
 pause
 exit /b 1
)
godot --editor project.godot

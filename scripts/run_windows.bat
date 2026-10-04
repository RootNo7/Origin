@echo off
cd /d "%~dp0\.."
echo Opening Origin in Godot 3...
where godot 2>nul >nul
if errorlevel 1 (
  echo Godot was not found in PATH.
  echo Open project.godot manually with Godot 3.x instead.
  pause
  exit /b 1
)
godot --editor project.godot

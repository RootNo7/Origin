#!/usr/bin/env python3
"""Strict dependency-free QA for Origin's Godot 3.6 project."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "observer" / "godot"
TEXTURE_ROOT = SCRIPT_ROOT / "assets" / "textures"
FORBIDDEN = (
    "Node3D", "CharacterBody3D", "RigidBody3D", "StaticBody3D", "MeshInstance3D",
    "Camera3D", "@onready", "@export", "@tool", "class_name", ":=", "await ",
    "shader_type", "ShaderMaterial", "OpenSimplexNoise", "FastNoiseLite", "RenderingServer",
    "Time.get_", "PackedVector", "Vector2i", "Vector3i", "super.",
)
REQUIRED_TEXTURES = (
    "terrain_albedo.png", "sand_albedo.png", "rock_albedo.png", "wood_albedo.png",
    "leaf_albedo.png", "soil_albedo.png", "water_albedo.png",
)

def strip_strings(text: str) -> str:
    text = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    text = re.sub(r"'(?:\\.|[^'\\])*'", "''", text)
    return text

def balanced(text: str, path: Path):
    cleaned = strip_strings(text)
    pairs = {')': '(', ']': '[', '}': '{'}
    stack = []
    for line_no, ch in enumerate(cleaned, 1):
        if ch in '([{':
            stack.append((ch, line_no))
        elif ch in ')]}':
            if not stack or stack[-1][0] != pairs[ch]:
                raise AssertionError(f"{path}:{line_no}: unmatched {ch}")
            stack.pop()
    if stack:
        ch, line_no = stack[-1]
        raise AssertionError(f"{path}:{line_no}: unclosed {ch}")

def main() -> int:
    failures = []
    scripts = sorted(SCRIPT_ROOT.rglob("*.gd"))
    if not scripts:
        failures.append("no .gd files found")
    for path in scripts:
        text = path.read_text(encoding="utf-8")
        clean = strip_strings(text)
        for token in FORBIDDEN:
            if token in clean:
                failures.append(f"{path}: forbidden Godot 4/newer token {token}")
        if re.search(r'\?[^\n:]*:', clean):
            failures.append(f"{path}: possible C-style ternary")
        if re.search(r'\.get\([^\n]*\)\s*=(?!=)', clean):
            failures.append(f"{path}: possible assignment to get() result")
        try:
            balanced(text, path)
        except AssertionError as exc:
            failures.append(str(exc))
        for i, line in enumerate(text.splitlines(), 1):
            if '\t' in line:
                failures.append(f"{path}:{i}: tab indentation")
        lines = text.splitlines()
        extends_lines = [(i, line.strip()) for i, line in enumerate(lines, 1) if line.strip().startswith("extends ")]
        if not extends_lines:
            failures.append(f"{path}: missing explicit Godot 3.x extends declaration")
        for i, stripped in extends_lines:
            allowed = {"Spatial", "Reference", "KinematicBody", "SceneTree"}
            parts = stripped.split()
            if len(parts) < 2 or parts[1] not in allowed:
                failures.append(f"{path}:{i}: unexpected base class {parts[1] if len(parts) > 1 else '<missing>'}")

    project = (ROOT / "project.godot").read_text(encoding="utf-8")
    if 'config_version=4' not in project:
        failures.append('project.godot: missing Godot 3.x config_version=4')
    if 'config/features=PoolStringArray("3.6")' not in project:
        failures.append('project.godot: missing Godot 3.6 feature tag')
    if 'quality/driver/driver_name="GLES2"' not in project:
        failures.append('project.godot: active renderer is not GLES2')
    if 'run/main_scene="res://observer/godot/scenes/Main.tscn"' not in project:
        failures.append('project.godot: main scene reference missing')

    scene = (ROOT / "observer/godot/scenes/Main.tscn").read_text(encoding='utf-8')
    if 'type="Spatial"' not in scene or 'res://observer/godot/scripts/Main.gd' not in scene:
        failures.append('Main.tscn: not a valid Godot 3 Spatial root scene')

    for tex in REQUIRED_TEXTURES:
        path = TEXTURE_ROOT / tex
        if not path.exists() or path.stat().st_size == 0:
            failures.append(f'missing texture: {path}')

    for launcher in (ROOT / "scripts/run_origin.bat", ROOT / "scripts/run_tests.bat"):
        text = launcher.read_text(encoding='utf-8').lower()
        if 'where godot.exe' in text or 'where godot\n' in text:
            failures.append(f'{launcher}: generic Godot launcher fallback is forbidden')

    if failures:
        print('ORIGIN STRICT VALIDATION: FAIL')
        for item in failures:
            print(' -', item)
        return 1
    print('ORIGIN STRICT VALIDATION: PASS')
    print(f'scripts={len(scripts)}')
    print('godot_target=3.6.x')
    print('renderer=GLES2')
    print(f'textures={len(REQUIRED_TEXTURES)}')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())

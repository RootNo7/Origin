#!/usr/bin/env python3
"""Final dependency-free release audit for Origin Godot 3.6."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
GODOT = ROOT / "observer" / "godot"
SCRIPTS = GODOT / "scripts"
MAIN_SCENE = GODOT / "scenes" / "Main.tscn"
TEXTURES = GODOT / "assets" / "textures"

FORBIDDEN = [
    "Node3D", "CharacterBody3D", "RigidBody3D", "StaticBody3D", "MeshInstance3D", "Camera3D",
    "DirectionalLight3D", "WorldEnvironment3D", "@onready", "@export", "@tool", "class_name",
    ":=", "await ", "shader_type", "ShaderMaterial", "OpenSimplexNoise", "FastNoiseLite",
    "RenderingServer", "Time.get_", "PackedVector", "Vector2i", "Vector3i", "ProceduralSky",
]
ALLOWED_EXTENDS = {"Spatial", "Reference", "KinematicBody", "SceneTree"}
REQUIRED_TEXTURES = {
    "terrain_albedo.png", "sand_albedo.png", "rock_albedo.png", "wood_albedo.png",
    "leaf_albedo.png", "soil_albedo.png", "water_albedo.png",
}


def strip_strings(text):
    text = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    return re.sub(r"'(?:\\.|[^'\\])*'", "''", text)


def balance(text, path):
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
        raise AssertionError(f"{path}:{stack[-1][1]}: unclosed {stack[-1][0]}")


def main():
    failures = []
    gd_files = sorted(SCRIPTS.glob('*.gd'))
    expected = {SCRIPTS / 'Main.gd', SCRIPTS / 'OriginRuntime.gd', SCRIPTS / 'Player.gd'}
    if set(gd_files) != expected:
        failures.append(f"active GDScript set mismatch: {[p.name for p in gd_files]}")

    for path in gd_files:
        text = path.read_text(encoding='utf-8')
        clean = strip_strings(text)
        lines = text.splitlines()
        extends = [line.strip().split() for line in lines if line.strip().startswith('extends ')]
        if len(extends) != 1 or len(extends[0]) < 2 or extends[0][1] not in ALLOWED_EXTENDS:
            failures.append(f"{path}: invalid/missing Godot 3 extends declaration")
        for token in FORBIDDEN:
            if token in clean:
                failures.append(f"{path}: forbidden newer-Godot token {token}")
        if re.search(r'\?[^\n:]*:', clean):
            failures.append(f"{path}: C-style ternary")
        if re.search(r'\.get\([^\n]*\)\s*=(?!=)', clean):
            failures.append(f"{path}: assignment to get() result")
        balance(text, path)
        names = re.findall(r'^func\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', text, re.M)
        if len(names) != len(set(names)):
            failures.append(f"{path}: duplicate function name")

    project = (ROOT / 'project.godot').read_text(encoding='utf-8')
    required_project_bits = [
        'config_version=4',
        'config/features=PoolStringArray("3.6")',
        'config/version="0.8.1-dev"',
        'quality/driver/driver_name="GLES2"',
        'run/main_scene="res://observer/godot/scenes/Main.tscn"',
    ]
    for bit in required_project_bits:
        if bit not in project:
            failures.append(f'project.godot missing: {bit}')

    scene = MAIN_SCENE.read_text(encoding='utf-8')
    if 'format=2' not in scene or 'type="Spatial"' not in scene or 'res://observer/godot/scripts/Main.gd' not in scene:
        failures.append('Main.tscn: invalid Godot 3 scene contract')

    for tex in REQUIRED_TEXTURES:
        p = TEXTURES / tex
        if not p.is_file() or p.stat().st_size == 0:
            failures.append(f'missing texture: {tex}')

    # Check every explicit res:// reference points at a packaged file.
    candidates = list(ROOT.rglob('*.gd')) + list(ROOT.rglob('*.tscn')) + list(ROOT.rglob('*.bat')) + [ROOT/'project.godot']
    for p in candidates:
        text = p.read_text(encoding='utf-8', errors='ignore')
        for ref in re.findall(r'res://[^\s"\']+', text):
            if ref.endswith(('.', ',', ')', ']', ':')):
                ref = ref[:-1]
            target = ROOT / ref[6:]
            if not target.exists():
                failures.append(f'{p}: missing res:// target {ref}')

    for bat in [ROOT/'scripts/run_origin.bat', ROOT/'scripts/run_tests.bat', ROOT/'RUN_ORIGIN.bat', ROOT/'RUN_ORIGIN_GODOT3.6.bat']:
        text = bat.read_text(encoding='utf-8', errors='ignore').lower()
        if 'where godot.exe' in text or "where godot '" in text or "where godot\n" in text:
            failures.append(f'{bat}: generic Godot fallback')

    native = []
    for suffix in ('*.cpp', '*.hpp', '*.h', 'CMakeLists.txt'):
        native.extend(ROOT.rglob(suffix))
    if native:
        failures.append('native/CMake files present in active package: ' + ', '.join(str(p.relative_to(ROOT)) for p in native))

    if failures:
        print('ORIGIN FINAL RELEASE QA: FAIL')
        for failure in failures:
            print(' -', failure)
        return 1
    print('ORIGIN FINAL RELEASE QA: PASS')
    print('godot_target=3.6.x')
    print('renderer=GLES2')
    print('scripts=3')
    print('required_textures=7')
    print('native_runtime_files=0')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())

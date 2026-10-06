#!/usr/bin/env python3
"""Static safety checks for the active Origin Godot 3.x project.

This is intentionally dependency-free. It cannot replace the Godot parser, but it
catches the exact classes of mistakes that caused recent regressions before F5.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "observer" / "godot"
FORBIDDEN = (
    "Node3D", "CharacterBody3D", "RigidBody3D", "@onready", "@export",
    "extends Node3D", "extends CharacterBody3D", "extends RigidBody3D",
    "shader_type", "ShaderMaterial", "OpenSimplexNoise",
)

def read_scripts():
    return sorted(SCRIPT_ROOT.rglob("*.gd"))

def balanced(text, path):
    cleaned = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    pairs = {')':'(', ']':'[', '}':'{'}
    stack=[]
    for line_no, ch in enumerate(cleaned, 1):
        if ch in '([{':
            stack.append((ch, line_no))
        elif ch in ')]}':
            if not stack or stack[-1][0] != pairs[ch]:
                raise AssertionError(f"{path}:{line_no}: unmatched {ch}")
            stack.pop()
    if stack:
        ch, line_no=stack[-1]
        raise AssertionError(f"{path}:{line_no}: unclosed {ch}")

def main():
    scripts=read_scripts()
    assert scripts, "no Godot scripts found"
    failures=[]
    for path in scripts:
        text=path.read_text(encoding='utf-8')
        for token in FORBIDDEN:
            if token in text:
                failures.append(f"{path}: forbidden token {token}")
        if re.search(r'\?[^\n:]*:', text):
            failures.append(f"{path}: possible C-style ternary")
        if re.search(r'\.get\([^\n]*\)\)\s*=(?!=)', text):
            failures.append(f"{path}: assignment to get() result")
        try:
            balanced(text, path)
        except AssertionError as exc:
            failures.append(str(exc))
        for i,line in enumerate(text.splitlines(),1):
            if '\t' in line:
                failures.append(f"{path}:{i}: tab indentation")
    project=(ROOT/'project.godot').read_text(encoding='utf-8')
    if 'quality/driver/driver_name="GLES2"' not in project:
        failures.append('project.godot: active renderer is not GLES2')
    if 'config_version=4' not in project:
        failures.append('project.godot: missing Godot 3.x config_version=4')
    if 'config/features=PoolStringArray("3.6")' not in project:
        failures.append('project.godot: missing explicit Godot 3.6 feature tag')
    scene=(ROOT/'observer/godot/scenes/Main.tscn').read_text(encoding='utf-8')
    if 'res://observer/godot/scripts/Main.gd' not in scene:
        failures.append('Main.tscn: Main.gd reference missing')
    if failures:
        print('ORIGIN STATIC VALIDATION: FAIL')
        for item in failures:
            print(' -', item)
        return 1
    print('ORIGIN STATIC VALIDATION: PASS')
    print('scripts=', len(scripts))
    print('renderer=GLES2')
    return 0

if __name__ == '__main__':
    sys.exit(main())

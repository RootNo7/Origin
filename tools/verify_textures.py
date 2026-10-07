#!/usr/bin/env python3
"""Verify Origin's shipped material texture set without external dependencies."""
from pathlib import Path
from PIL import Image
import numpy as np

ROOT = Path(__file__).resolve().parents[1]
ASSET = ROOT / 'observer' / 'godot' / 'assets' / 'textures'
REQUIRED = {
    'terrain_albedo.png': (512, 512),
    'terrain_normal.png': (512, 512),
    'sand_albedo.png': (512, 512),
    'sand_normal.png': (512, 512),
    'rock_albedo.png': (512, 512),
    'rock_normal.png': (512, 512),
    'wood_albedo.png': (512, 512),
    'wood_normal.png': (512, 512),
    'leaf_albedo.png': (512, 512),
    'soil_albedo.png': (512, 512),
    'soil_normal.png': (512, 512),
    'water_albedo.png': (512, 512),
    'water_normal.png': (512, 512),
    'sky_gradient.png': (512, 256),
}
failures = []
for name, expected_size in REQUIRED.items():
    path = ASSET / name
    if not path.exists():
        failures.append(f'missing {name}')
        continue
    try:
        with Image.open(path) as image:
            image.load()
            if image.size != expected_size:
                failures.append(f'{name}: expected {expected_size}, got {image.size}')
            if image.mode not in ('RGB', 'RGBA', 'L'):
                failures.append(f'{name}: unsupported mode {image.mode}')
            if image.width >= 512 and image.height >= 512:
                data = np.asarray(image.convert('RGB')).astype(np.int16)
                h_edge = float(np.abs(data[:, 0] - data[:, -1]).mean())
                v_edge = float(np.abs(data[0] - data[-1]).mean())
                if h_edge > 12.0 or v_edge > 12.0:
                    failures.append(f'{name}: tile edge mismatch too large ({h_edge:.2f}, {v_edge:.2f})')
    except Exception as exc:
        failures.append(f'{name}: unreadable ({exc})')
if failures:
    print('ORIGIN TEXTURE VALIDATION: FAIL')
    for failure in failures:
        print(' -', failure)
    raise SystemExit(1)
print('ORIGIN TEXTURE VALIDATION: PASS')
print('textures=', len(REQUIRED))

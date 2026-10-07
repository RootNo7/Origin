# Verification — Origin 0.8.0-dev

## Godot 3.6 compatibility audit

- Active runtime is Godot/GDScript only.
- No C++/CMake build is required.
- `project.godot` explicitly declares the Godot 3.6 feature set and GLES2 renderer.
- No Godot 4-only nodes/APIs or annotation syntax detected in active `.gd` files.
- No C-style ternary syntax remains in active `.gd` files.
- Main scene is `observer/godot/scenes/Main.tscn`.
- Test launcher refuses executables that do not report Godot 3.6.x.

## Graphics regression fixes

- Corrected the terrain normal formula from the previous swapped x/z derivatives. This was a concrete source of the bright/striped terrain lighting artifact visible in the user's screenshots.
- Removed the active terrain normal-map dependency from the GLES2 path; terrain uses geometric normals plus material albedo.
- Added separate terrain surfaces for normal ground, coastal sand, and steep/high rock areas.
- Added 512x512 original material textures for terrain, sand, rock, bark, foliage, soil, and water.
- Added matching normal maps for secondary materials and retained terrain/water maps for future use without forcing them onto the stable terrain/water render paths.
- Removed depth fog and exposure adjustment from the active low-end scene to prevent the washed-out foreground/horizon presentation.
- Lowered direct/ambient lighting and water specular energy to prevent the previous near-white clipping.
- Reworked terrain generation with coherent multi-scale value noise and restored a meaningful water/land ratio.
- Improved player movement smoothing and floor-angle handling.

## Texture validation

`tools/verify_textures.py` verifies that all shipped PNG assets are readable, have the intended dimensions, and have bounded tile-edge mismatch.

## Runtime limitation

The development container does not include the Godot 3.6 executable, so graphical F5/F6 execution cannot be truthfully marked as container-tested. Static compatibility checks, texture validation, source audits, and deterministic world-distribution checks are performed here.

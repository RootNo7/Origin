## 0.8.1 recovery gate
- Active runtime was rebuilt from the last confirmed-running Godot path before texture integration.
- Every active GDScript now requires an explicit Godot 3.x `extends` declaration.
- Strict validator rejects missing `extends`, Godot 4 APIs, C-style ternaries, bad `get() =` assignments, invalid project settings, missing textures, and generic Godot launch fallbacks.

# Verification — Origin 0.8.1-dev

## Godot 3.6 compatibility audit

- Active runtime is Godot/GDScript only.
- No C++/CMake build is required.
- Removed the C-style `condition ? a : b` expression from `OriginRuntime.gd`.
- No `?` ternary syntax remains in active `.gd` files.
- No Godot 4-only tokens detected in active `.gd` files.
- Main scene is `observer/godot/scenes/Main.tscn`.

## User-reported 0.6.1 failure

The reported parser error at `OriginRuntime.gd` line 408 was caused by a C-style ternary expression in `set_speed()`. Godot 3.x uses the `value_if_true if condition else value_if_false` form, not `condition ? a : b`.

The invalid expression has been replaced with explicit Godot 3-compatible control flow.

The grey window was a downstream startup failure: `OriginRuntime.gd` could not compile, so `Main.gd` could not initialize the runtime/world.

## Local limitation

The development container does not include the Godot 3.6 executable, so graphical F5/F6 execution cannot be truthfully marked as container-tested. Static compatibility checks and source audits are performed here.

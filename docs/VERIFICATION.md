# Verification — Origin 0.6.0-dev

Static checks completed:

- `project.godot` contains one application section and points to the Godot 3.6 main scene.
- All `res://` references resolve to files in the active project.
- Active GDScript contains no Godot 4-only classes/syntax detected by the release audit.
- No active `.cpp/.hpp/.h`, CMake project, native bridge, or native executable remains in the runtime tree.
- GDScript delimiter balance and basic structure checks pass.
- Persistence validation covers malformed dictionaries, bounds, IDs, inventory, environment and save-size limits.

Runtime diagnostics included in the project:

- F1 self-test runner
- F1 10,000-tick benchmark
- in-process screenshot capture
- full scene smoke test from the developer console
- automatic resume/autosave/save-on-exit
- previous-format and legacy-save migration paths

Godot runtime execution can only be marked verified on a machine where Godot 3.6.x is available. The development environment used to assemble this package does not contain a Godot executable, so the final package is statically compatibility-checked but not falsely marked as visually playtested here.

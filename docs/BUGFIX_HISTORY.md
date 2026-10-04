# Bugfix History

## fix-03
- Removed every `get_font()` call.
- Replaced Node2D text rendering with Godot 3 `Label` controls.
- Removed Godot 4 rendering settings.
- Corrected Godot 2D gravity direction.
- Corrected dictionary/entity updates.
- Kept `project.godot` in the root.
- Made the first boot path independent of the unfinished C++ bridge.

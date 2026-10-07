# Godot Tools

`godot_self_test.gd` runs OriginRuntime's state-restoring self-test suite and 10,000-tick benchmark through the same Godot-only runtime used by the game.

`validate_godot_project.py` performs dependency-free Godot 3.6 compatibility and project-structure checks.

`verify_textures.py` validates the shipped material texture set.

`check_terrain_distribution.py` verifies the deterministic world generator produces both land and water with meaningful relief.

# Virtual Earth

A persistent, deterministic 2D Earth-like world built in Godot. **This build deliberately contains no LLM or autonomous humans yet.** The goal is to establish the world itself as a stable simulation substrate.

## What exists now

- Large 8192×8192 continuous world.
- Deterministic procedural land/ocean/biome map from a fixed seed.
- Day/night cycle and an accelerated virtual clock.
- Calendar, seasons, weather and temperature simulation.
- Persistent save file in `user://virtual_earth_save.json`.
- Physics sandbox using `RigidBody2D` objects with collisions and momentum.
- World boundaries and camera limits.
- Procedurally generated trees, rocks and shrubs.
- Observer camera, zoom and simulation controls.
- No external art assets or Python dependencies.
- Compatibility renderer for older PCs.

## Important design rule

The world does not teach a future agent through hidden shortcuts. Future AI agents will interact with the world through an explicit perception/action interface, while the simulation remains the authority for physical consequences.

## Controls

- **W A S D** — move observer camera
- **Mouse wheel** — zoom
- **Space** — pause/resume virtual time
- **+ / K** — increase simulation speed
- **- / J** — decrease simulation speed
- **F** — spawn a physics object at the mouse location and give it an impulse
- **R** — reset camera to the world center

## Open in Godot

Use **Godot 4.7.2 stable** or another compatible Godot 4.7 build. The project uses only built-in Godot systems and GDScript.

1. Extract/open this folder in Godot.
2. Import `project.godot`.
3. Press **F6/F5** to run the project.
4. Watch the virtual clock advance, move around the world, and test physics with **F**.

## Save location

Godot stores the persistent runtime save under its normal per-user `user://` directory. The save contains the simulation clock/weather and dynamic physics-object states.

## Reality note

This is an Earth-*like* simulation, not a geophysics-accurate replica of the real planet. The architecture is intended to let the world rules become progressively richer without changing the future AI interface.

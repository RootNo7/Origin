# Verification

# Verification — Origin 0.3.0-dev

## Verified in this environment

- CMake configuration and C++20 Release build succeed.
- CTest: 1/1 test passed.
- AddressSanitizer + UndefinedBehaviorSanitizer test run: 1/1 passed.
- `origin` runs and writes the version-3 persistent save format.
- `origin --resume` continues saved simulation time instead of silently restarting.
- Save/load round-trip preserves seed, world revisions, entity state, inventory and simulation clock state.
- Gather actions are authoritative and validate actor, target, amount, range and inventory capacity.
- External action bridge accepts valid commands and publishes actual simulation results.
- Invalid external actor commands are rejected by the simulation.
- State bridge publishes `ORIGIN_STATE 3` and separates terrain cache from high-frequency state.
- Terrain cache remains unchanged across ticks when only time/entity state changes.
- Physics uses bounded substeps and recovers non-finite dynamic state.
- A 10,000-tick 96×96 benchmark completed in about 1.77 s in the current development container (~5.6k simulation ticks/s; environment-dependent).

## Godot 3.6 validation status

Godot 3.6 is not installed in this development environment, so the first-person client is **runtime-unverified** here. Its core 3.x choices were checked against Godot 3.6 documentation: `KinematicBody.move_and_slide`, `ArrayMesh.add_surface_from_arrays`, and `CubeMesh.size` are documented 3.x APIs. citeturn996370search5turn179805search0turn996370search3

The remaining client validation step is to open the repository in Godot 3.6 and verify first-person movement, mouse look, terrain collision, water rendering, resource markers and live state updates.

# Verification — Origin 0.4.0-dev

## Verified in this environment

- Clean CMake Release configuration succeeds.
- C++ Release build succeeds with no compiler warnings in the final build log.
- CTest passes.
- Regression suite passes repeatedly.
- AddressSanitizer + UndefinedBehaviorSanitizer pass on the regression suite.
- C++ executable runs and writes `ORIGIN_SAVE 4`.
- `--resume` continues saved simulation time (`4s / tick 120` → `8s / tick 240` in the validation run).
- Save/load preserves the HumanTester identity and inventory state.
- Save/load rejects malformed numeric state and duplicate terrain coordinates without mutating the target simulation.
- Extreme clock inputs are clamped/safely converted.
- Physics recovers invalid dynamic body state and keeps positions/velocities finite.
- Normal agent actions cannot impersonate HumanTester.
- Agent bridge explicitly rejects unsupported and malformed commands.
- Developer bridge accepts an authoritative pose followed by gather.
- Live bridge integration confirmed `ORIGIN_STATE 4`, HumanTester ID 3, accepted pose, accepted gather, and updated test inventory.
- Terrain cache remains separate from high-frequency state updates.

## Godot 3.6 validation status

The development environment does not have Godot 3.6 installed, so the first-person client remains **runtime-unverified here**. The GDScript intentionally uses Godot 3.x APIs (`KinematicBody`, `Spatial`, `InputEventMouseButton`, `File`, `Directory`, `PoolVector3Array`, `ArrayMesh`, `StaticBody`, `CapsuleShape`, `PlaneMesh`, `CubeMesh`) and contains no detected Godot 4-style class/decorator usage.

The remaining client verification is to open the repository in Godot 3.6 and confirm first-person movement, mouse capture, terrain collision, resource targeting, pose publication, gather results and HUD updates.

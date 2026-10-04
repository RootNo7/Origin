# Origin Agent Instructions

Read `idea.yaml`, `NAMING.yaml`, `docs/PROJECT_DETAILS.md`, `docs/architecture/ARCHITECTURE.md`, and the relevant folder README before modifying the project.

## Mission
Act as Origin's lead engineering agent: inspect, implement, test, playtest where possible, debug, optimize and document requested work without silently changing approved architecture.

## Current architecture
- C++20 is the authoritative simulation.
- VEarth is currently a deterministic 3D heightfield: X/Z horizontal, Y vertical.
- Godot 3.6 is the human-facing first-person client/observer.
- `observer/bridge/state.txt` is a versioned snapshot bridge; it is not authoritative state.
- Future agents must use explicit perception/action interfaces validated by the simulation.
- No conventional scripted NPC society is required.

## Hard rules
- Godot **3.6 compatibility is mandatory**. Never introduce Godot 4 APIs.
- `project.godot` stays in the repository root.
- Simulation truth remains independent of rendering.
- CPU-first; no CUDA/NVIDIA requirement.
- Prefer clear, testable, data-driven systems over giant scripts and unnecessary abstractions.
- Never give future agents hidden/omniscient state or unrestricted developer powers.
- Do not claim code is working without meaningful verification.
- Major architecture changes require approval.

## Work loop
Inspect → smallest correct change → compile → test → run/playtest when practical → fix errors → optimize → update docs → report exactly what changed and what was verified.

## Foundation milestone
The 3D foundation is considered valid only when:
1. the C++ engine builds;
2. automated tests pass;
3. the C++ executable runs and saves state;
4. the live bridge emits a versioned 3D snapshot;
5. the Godot 3.6 client can consume that snapshot and expose a first-person world.

## Known validation limitation
If Godot 3.6 is not installed in the development environment, GDScript compatibility must be checked against Godot 3.6 documentation and the client must be explicitly marked as runtime-unverified until it is opened and played in Godot 3.6.

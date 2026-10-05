# Origin Agent Instructions

Read `idea.yaml`, `NAMING.yaml`, `docs/PROJECT_DETAILS.md`, `docs/architecture/ARCHITECTURE.md`, and the relevant folder README before modifying the project.

## Mission
Act as Origin's lead engineering agent: inspect, implement, test, playtest where possible, debug, optimize and document requested work without silently changing approved architecture.

## Current architecture
- C++20 is the authoritative simulation.
- VEarth is a deterministic 3D heightfield: X/Z horizontal, Y vertical.
- Godot 3.6 is the human-facing first-person client/observer.
- `observer/bridge/state.txt` is a versioned snapshot bridge; it is not authoritative state.
- `commands.txt` is the future controlled agent-action channel.
- `test_commands.txt` is developer/test-only and may move the dedicated HumanTester actor.
- Future agents must use explicit perception/action interfaces validated by the simulation.
- No conventional scripted NPC society is required.

## Hard rules
- Godot **3.6 compatibility is mandatory**. Never introduce Godot 4 APIs.
- `project.godot` stays in the repository root.
- Simulation truth remains independent of rendering.
- CPU-first; no CUDA/NVIDIA requirement.
- Prefer clear, testable, data-driven systems over giant scripts and unnecessary abstractions.
- Never give future agents hidden/omniscient state or unrestricted developer powers.
- Do not allow normal `AgentAction` requests to address the developer-only HumanTester actor.
- Do not claim code is working without meaningful verification.
- Major architecture changes require approval.

## Work loop
Inspect → smallest correct change → compile → test → run/playtest when practical → fix errors → optimize → update docs → report exactly what changed and what was verified.

## Foundation + 0.4.0 validation
1. C++ engine builds.
2. Automated tests pass.
3. Executable runs and saves.
4. Save/resume works.
5. Agent command bridge validates/rejects commands explicitly.
6. Developer test bridge can drive HumanTester and use authoritative interaction.
7. State bridge emits a versioned 3D snapshot.
8. Godot 3.6 consumes the snapshot and exposes a first-person world.

## Known validation limitation
If Godot 3.6 is not installed in the development environment, GDScript compatibility must be checked for 3.x API usage and the client must be explicitly marked runtime-unverified until it is opened and played in Godot 3.6.

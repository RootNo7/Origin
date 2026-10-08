# Origin Agent Instructions

Read `idea.yaml`, `NAMING.yaml`, `docs/PROJECT_DETAILS.md`, `docs/architecture/ARCHITECTURE.md`, and the relevant folder README before modifying the project.

## Mission
Act as Origin's lead engineering agent: inspect, implement, test, playtest where possible, debug, optimize and document requested work without silently changing approved architecture.

## Current architecture
- **Godot 3.6 is the active and only runtime.**
- `observer/godot/scripts/OriginRuntime.gd` is the authoritative simulation for the playable project.
- `observer/godot/scripts/Main.gd` is the first-person client, renderer and developer/test surface.
- Rendering consumes runtime state; it is not a second simulation.
- Persistence is handled by the Godot runtime under `user://origin/`.
- `OriginRuntime.gd` exposes a deliberately small controlled action interface for future agents.
- The previous native implementation is historical only and is not included in the active repository.
- No conventional scripted NPC society is required.

## Hard rules
- Godot **3.6 compatibility is mandatory**. Never introduce Godot 4 APIs.
- `project.godot` stays in the repository root.
- Runtime simulation truth stays independent of rendering code.
- CPU-first; no CUDA/NVIDIA requirement.
- Prefer clear, testable, data-driven systems over giant scripts and unnecessary abstractions.
- Never give future agents hidden/omniscient state or unrestricted developer powers.
- The developer-only `HumanTester` actor must never be controllable through the normal agent source.
- Never claim code is working without meaningful verification.
- Major architecture changes require approval.
- Do not reintroduce a required native executable/compiler into the normal run path.

## One-application workflow
- Open the repository in Godot 3.6.
- Or use the root `RUN_ORIGIN_GODOT3.6.bat` for the same Godot-only runtime.
- Press Play/F6 to run the complete world.
- F1 opens the developer/test console.
- The console can save/load, regenerate, run self-tests, benchmark, change simulation speed and capture screenshots.
- `scripts/run_tests.bat` invokes the same runtime tests through Godot with its no-window mode when Godot is available.

## Work loop
Inspect → smallest correct change → run static/compatibility checks → run Godot tests where practical → playtest → fix errors → optimize → update docs → report exactly what changed and what was verified.

## Current 0.8.1-dev validation targets
1. The project opens directly in Godot 3.6 with no C++ toolchain.
2. The Godot runtime generates deterministic 3D terrain.
3. First-person movement and terrain collision work.
4. Resource targeting/gathering is authoritative.
5. Developer and agent capabilities remain separated.
6. JSON save/load is transactional, supports bounded custom dimensions, and invalid loads are atomic.
7. Runtime self-tests, full scene smoke test and benchmark run from Godot.
8. Screenshots can be captured from the in-game developer console.
9. Previous format-5 JSON and ORIGIN_SAVE 3/4 migrations remain supported.

## Known environment limitation
If Godot 3.6 is not installed in the development environment, perform strict Godot 3.x API/static checks and clearly mark live Godot execution as unverified. Do not silently fall back to C++ as the project's runtime.


## 0.8.1-dev startup hardening note

- Do not use `seed` as an in-memory GDScript member name; keep `seed` only as a serialized schema key.
- Do not compile-time `preload` the authoritative runtime from `Main.gd`; load it at startup so a script error does not cascade into a misleading preload error.
- Startup failures must produce a visible diagnostic panel instead of an empty/grey scene.
- Godot 3.x compatibility must be checked before release; never introduce Godot 4 syntax into the active client.

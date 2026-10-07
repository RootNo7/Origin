# Origin 0.8.0-dev

Origin is a persistent virtual-world research environment. It is not primarily a conventional open-world game. The long-term target is a systemic environment that humans can test and LLM agents can eventually inhabit through controlled perception/action interfaces.

## 0.8.0-dev: single-application runtime + startup hardening

The active Origin runtime is now **Godot 3.6-only**.

You do **not** need:

- Visual Studio
- CMake
- a C++ compiler
- a separate C++ executable
- a second bridge process

Godot now owns the active runtime simulation, terrain, resource state, physics, time, persistence, human testing, developer controls, tests and rendering in one application.

The previous native implementation is no longer included in the active repository. Earlier 0.4/0.5 project packages remain the historical reference. This release has no native runtime dependency.

## Run

Open this repository in **Godot 3.6.x** and press Play/F5. The standard Godot 3 build is all that is required; no C++ toolchain is part of Origin.

Or run `scripts\\run_origin.bat` when a Godot executable is available on PATH or at the locations documented by the script.

## Controls

WASD = move  
Space = jump  
Left-click = gather the resource actually hit by the camera ray  
Esc = release/capture mouse  
F1 = developer/test console

The F1 console can:

- create a new world
- save/load the persistent world
- run the runtime self-tests
- run a full scene smoke test
- benchmark 10,000 simulation ticks
- capture a screenshot
- pause/resume simulation
- change simulation speed

## Architecture

```text
Godot 3.6
  ├─ First-person human test client
  ├─ Rendering
  ├─ Input / developer console
  ├─ Persistence
  └─ OriginRuntime.gd  ← authoritative simulation
        ├─ VEarth terrain
        ├─ time/calendar
        ├─ environment
        ├─ physics
        ├─ resources
        ├─ inventory
        └─ controlled action interface

No native backend is present in the active project. Historical 0.4/0.5 packages can be kept separately when comparing migrations.
```

The world remains authoritative. Rendering is a consumer of runtime state, not a second simulation.

## Verification

Use `scripts\\run_tests.bat` for the Godot headless self-test when Godot is available. The same checks are available from the in-game F1 console.

The runtime self-test covers world dimensions, terrain relief, water existence, safe HumanTester spawn, solar day/night contrast, ID uniqueness, deterministic terrain, developer gathering, agent/developer separation, both current and previous save formats, simulation-speed semantics, and invalid-save atomicity.

## Current scope

Implemented:

- deterministic 3D procedural terrain
- persistent resource deposits
- resource gathering
- inventory
- first-person controller
- time + Origin calendar
- day/night sunlight
- environmental temperature
- dynamic stone physics
- transactional JSON save/load
- legacy ORIGIN_SAVE 3/4 import plus automatic loading of the previous Godot JSON save format
- custom saved world dimensions within the bounded runtime limit
- developer/test console
- runtime self-tests
- integrated benchmark
- integrated screenshot capture

Future milestones can now build directly on the Godot runtime without maintaining a second executable.


## 0.8.0-dev stability note

This release is a regression recovery build based on the last confirmed-working 0.6.2 runtime. It keeps the Godot 3.6/GLES2 execution path, removes experimental shader-dependent presentation, and reintroduces only deterministic terrain/time changes.

Actual Godot graphical F5 execution must still be performed with Godot 3.6 on the target machine; the repository includes static compatibility checks and in-Godot self-tests for runtime verification.


## 0.8.0-dev graphics pass

This milestone introduces the first authored texture/material layer without leaving the Godot 3.6-only runtime: terrain, sand, rock, bark, leaves, soil, and water textures; normal maps for secondary materials where the mesh path supports them; deterministic trees and rocks; a procedural Godot 3 sky; conservative lighting; and opaque water rendering to avoid GLES2 transparency/sorting artifacts.

The release intentionally avoids custom shaders and third-party runtime dependencies.

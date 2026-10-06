# Changelog

## 0.7.2-dev

- Fixed the Godot 3.x parser failure caused by using `seed` as an in-memory member variable name; the serialized save key remains `seed`.
- Fixed the cascading `Main.gd` preload error by loading the runtime and player scripts at startup instead of compile-time preloading them.
- Added a visible startup error panel so script-load failures cannot degrade into an unexplained grey screen.
- Made the headless Godot self-test report a clear runtime-script load failure instead of a secondary preload error.
- Fixed the event log bound so `MAX_EVENT_LOG` is actually respected.
- Rebuilt the active 0.7.1 client from the last confirmed-working 0.6.2 Godot path instead of carrying forward the experimental 0.7.0 rendering/physics layer.
- Removed the experimental custom sky shader, shader parameters, and GLES3 renderer change from the active runtime.
- Added deterministic multi-scale terrain, safe HumanTester spawn selection, and a real 10-minute solar cycle.
- Added terrain relief, water-presence, spawn-safety, and solar-contrast regression checks.
- Water is presented as the world sea surface rather than an extra blue resource cube.


## 0.6.0-dev

- Completed the runtime consolidation: Origin is now a single Godot 3.6 application with no active C++ backend or compiler dependency.
- Added the root `RUN_ORIGIN_GODOT3.6.bat` one-click launcher.
- F1 developer/test console remains inside the same running world application.
- Added a full scene smoke test that verifies the runtime, player, camera, terrain, collision, water and resource visuals together.
- Added automatic resume from the persistent Godot JSON world.
- Added autosave on persistent changes and save-on-exit behavior.
- Preserved loading of previous Godot save format 5 while writing format 6.
- Loading now accepts saved world dimensions up to the current bounded runtime limit instead of requiring the default 96x96 size.
- Enforced per-material inventory capacity at the authoritative action boundary.
- Preserved import of older ORIGIN_SAVE 3/4 text saves, including automatic HumanTester recreation for version-3 saves.
- Hardened save loading with a 128 MiB file-size limit and required-field validation.
- Added persistent revision tracking for meaningful HumanTester movement.
- Bounded the in-memory simulation event log to prevent indefinite growth during long experiments.
- Increased the normal fixed-step processing budget so high simulation speeds can retain correct elapsed simulation time without immediately discarding backlog.
- Kept resource interaction physics-authoritative by moving gather ray queries into the physics tick.

## 0.5.0-dev

- Removed the C++ executable and bridge from the normal runtime workflow.
- Made `OriginRuntime.gd` the authoritative Godot simulation.
- Added integrated F1 developer/test console.
- Added Godot-only runtime self-tests and benchmark.
- Added screenshot capture from inside the application.
- Added authoritative direct resource gathering from the first-person client.
- Replaced line/file bridge interaction with in-process simulation interfaces for the active runtime.
- Added version 5 JSON persistence and transactional publish/load validation.
- Added legacy import path for native ORIGIN_SAVE 3/4 snapshots.
- Fixed simulation-speed semantics: speed scales the fixed-step accumulator rather than inflating the physics timestep.
- Added state-restoring self-tests and benchmark so diagnostics do not permanently mutate the world.
- Added resource collision bodies so cursor gathering respects actual scene occlusion.
- Disabled player movement while the developer console is open.
- Archived the previous native backend under `reference_cpp/`.

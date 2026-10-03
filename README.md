# Origin

Origin is a persistent artificial-universe project.

This repository currently contains the first runnable universe foundation:
VEarth, simulation time, deterministic world generation, simple physical
motion, environmental cycles, a small chemistry boundary, entities, events,
and inspectable save/load state.

The virtual world is intentionally independent from AI. AI/inhabitants will
connect through explicit perception/action interfaces later.

## Build

Requirements:

- CMake 3.20+
- C++20 compiler

From the repository root:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Run:

```bash
./build/origin
```

On a Visual Studio generator, the executable is usually under:
`build/Release/origin.exe`.

Run tests:

```bash
ctest --test-dir build --output-on-failure
```

## Current runtime

The executable generates VEarth from a fixed seed, advances the simulation
with a fixed time step, applies gravity and terrain collision to a few physical
objects, updates the day/night environment, prints a terminal view, and saves
state to `storage/saves/vearth.origin`.

This is a real foundation, not a claim that the complete scientific universe
has already been implemented. Physics and chemistry are deliberately scoped
to the models currently represented in code; future models must be explicit,
tested, and documented rather than faked.

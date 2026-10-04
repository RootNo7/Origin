# Origin 0.3.0-dev

Origin is a persistent virtual-world research environment. It is not primarily a conventional open-world game. The long-term target is a systemic environment that humans can test and LLM agents can eventually inhabit through controlled perception/action interfaces.

## Current 0.3.0-dev slice

- C++20 simulation authority
- deterministic seeded 3D heightfield terrain
- 3D entity physics with gravity and terrain collision
- continuous simulation clock
- environmental temperature/sunlight model
- C++ state bridge with versioned 3D snapshots
- Godot 3.6 first-person human test client
- authoritative resource deposits + inventory model
- validated gather action path and external action bridge
- versioned transactional save/load
- cached terrain bridge to reduce per-tick I/O
- virtual calendar derived from simulation time
- physics substeps and finite-state recovery
- executable C++ test suite

## Godot 3.6

Open this repository root as a Godot 3.6 project. Build/run the bridge first, then launch the Godot project. The Godot client reads `observer/bridge/state.txt` while C++ remains authoritative.

On Windows:

```text
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
scripts\\run\\run_origin_bridge.bat
```

Then press F5 in Godot 3.6. Runtime verification still needs to be performed in that editor because it is not installed in the current development environment.

Controls: WASD to move, mouse to look, Space to jump, Esc to release the mouse. Click the game window to recapture it.

## Status

This is a development foundation, not a completed virtual universe. Major systems such as crafting, construction, wildlife, advanced chemistry, streaming, richer agent perception/action transport, and long-horizon experimentation remain future milestones.


### External action protocol

The simulation exposes a deliberately narrow `gather <actor_id> <resource_id> <amount>` command through `observer/bridge/commands.txt`. The simulation validates the request and publishes the actual result to `results.txt`. See `observer/bridge/README.md`.

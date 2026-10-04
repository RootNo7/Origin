# Origin 0.2.0-dev

Origin is a persistent virtual-world research environment. It is not primarily a conventional open-world game. The long-term target is a systemic environment that humans can test and LLM agents can eventually inhabit through controlled perception/action interfaces.

## Current foundation

- C++20 simulation authority
- deterministic seeded 3D heightfield terrain
- 3D entity physics with gravity and terrain collision
- continuous simulation clock
- environmental temperature/sunlight model
- C++ state bridge with versioned 3D snapshots
- Godot 3.6 first-person human test client
- future agent action seam without developer powers
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

Then press F5 in Godot 3.6.

Controls: WASD to move, mouse to look, Space to jump, Esc to release the mouse. Click the game window to recapture it.

## Status

This is a development foundation, not a completed virtual universe. Major systems such as resource interaction, crafting, construction, wildlife, advanced chemistry, streaming, agent perception/action transport, and long-horizon experimentation remain future milestones.

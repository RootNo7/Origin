# Origin 0.4.0-dev

Origin is a persistent virtual-world research environment. It is not primarily a conventional open-world game. The long-term target is a systemic environment that humans can test and LLM agents can eventually inhabit through controlled perception/action interfaces.

## Current 0.4.0-dev slice

- C++20 simulation authority
- deterministic seeded 3D heightfield terrain
- 3D entity physics with bounded substeps and finite-state recovery
- continuous simulation clock + Origin calendar
- environmental temperature/sunlight model
- C++ state bridge with versioned 3D snapshots
- cached terrain bridge
- Godot 3.6 first-person human test client
- persistent resource deposits + inventory primitives
- authoritative gather action with agent/developer source separation
- atomic action/developer command claiming
- explicit bridge rejection results for malformed/unsupported commands
- transactional save/load with version 4 schema
- version 3 save migration support
- save validation for finite values, duplicate IDs/cells, inventory consistency and resource bounds
- human-test pose bridge and authoritative human gathering from Godot
- expanded automated regression coverage

## Running the C++ validation

On Windows, from the repository root:

```text
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

For the live bridge:

```text
scripts\\run\\run_origin_bridge.bat
```

Then open the repository root as a **Godot 3.6** project and press F5. Godot is the human-facing observer/client; C++ remains authoritative.

Controls: WASD to move, Space to jump, Esc to release the mouse, left-click while captured to attempt gathering the resource under the cursor.

The Godot client communicates with the separate developer test channel:

```text
observer/bridge/test_commands.txt
observer/bridge/test_results.txt
```

The agent channel remains:

```text
observer/bridge/commands.txt
observer/bridge/results.txt
```

An agent cannot use the developer-only HumanTester actor through the normal `AgentAction` interface.

## Status

This is an evolving foundation. Crafting, construction, richer item semantics, biology, volumetric terrain, chunk streaming, large-scale simulation LOD, networking, and LLM cognition remain future milestones.

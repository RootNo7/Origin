# Origin Project Details

Origin is a virtual-universe research project, not simply a game. VUniverse is the simulated reality; VEarth is its first world; Eris is intended to become its first intelligent inhabitant later.

## 0.3.0-dev interaction + persistence foundation

The simulation has crossed the architectural boundary from a 2D prototype to a 3D heightfield authority. X/Z are world-plane coordinates and Y is elevation. The Godot 3.6 client now provides a real first-person testing surface while C++ remains the source of truth.

Implemented:
- deterministic seeded terrain
- 3D entity state and gravity/terrain collision
- continuous simulation time
- temperature/sunlight environment
- versioned C++→Godot 3D state bridge
- persistent snapshot writer
- first-person Godot 3.6 test client
- future agent action contract
- resource deposits and inventory primitives
- validated gather actions
- external action bridge
- versioned transactional save/load
- cached terrain bridge
- Origin calendar (second → year)
- hardened physics integration
- C++ tests

Not implemented yet:
- molecular chemistry
- richer item/inventory rules and capacity semantics
- crafting/construction
- biology/life
- LLM runtime or cognition
- agent networking/transport
- world chunk streaming
- large-scale simulation LOD
- advanced save migration

Those systems will be added only when their prerequisites are stable.

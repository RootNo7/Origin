# Origin Project Details

Origin is a virtual-universe research project, not simply a game. VUniverse is the simulated reality; VEarth is its first world; Eris is intended to become its first intelligent inhabitant later.

## 0.2.0-dev foundation

The simulation has crossed the architectural boundary from a 2D prototype to a 3D heightfield authority. X/Z are world-plane coordinates and Y is elevation. The Godot 3.6 client now provides a real first-person testing surface while C++ remains the source of truth.

Implemented foundation:
- deterministic seeded terrain
- 3D entity state and gravity/terrain collision
- continuous simulation time
- temperature/sunlight environment
- versioned C++→Godot 3D state bridge
- persistent snapshot writer
- first-person Godot 3.6 test client
- future agent action contract
- C++ tests

Not implemented yet:
- molecular chemistry
- item/resource inventory systems
- crafting/construction
- biology/life
- LLM runtime or cognition
- agent networking/transport
- world chunk streaming
- large-scale simulation LOD
- advanced save migration

Those systems will be added only when their prerequisites are stable.

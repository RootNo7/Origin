# Origin Project Details

Origin is a virtual-universe research project, not simply a game. VUniverse is the simulated reality; VEarth is its first world; Eris is intended to become its first intelligent inhabitant later.

## 0.4.0-dev reliability + human interaction milestone

The 3D foundation now has a usable human interaction path without making Godot authoritative.

Implemented:
- deterministic seeded 3D terrain
- 3D entity state and physics
- continuous time + calendar
- environmental temperature/sunlight
- versioned C++→Godot state bridge
- cached terrain snapshot
- resource deposits + inventory
- authoritative gather action
- explicit Agent vs DeveloperTest action source separation
- dedicated HumanTester entity
- Godot test-channel pose synchronization
- Godot resource targeting and authoritative gathering
- transactional save/load version 4
- validation and version 3 migration
- hardened command parsing/publication
- expanded tests and sanitizer validation

Not implemented yet:
- crafting
- construction
- richer tools/material semantics
- molecular chemistry
- biology/life
- LLM runtime/cognition
- agent networking
- volumetric/chunked terrain
- world streaming and simulation LOD
- long-horizon experiment orchestration

The next milestone should add deeper world interaction only after these boundaries remain stable.

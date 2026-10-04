# Architecture

`project.godot` is deliberately in the repository root so Godot 3 opens the whole Origin project.

`engine/` = simulation authority; `world/` = concrete world definitions; `observer/` = bridge/visualization; `ai/` = future inhabitants; `storage/` = persistence; `experiments/` = research; `tests/` = verification; `tools/` = utilities; `docs/` = knowledge; `config/` = configuration; `scripts/` = developer commands.

Simulation truth stays in C++. Godot observes it. Future AI receives explicit perception/action interfaces, not hidden omniscient state.

The first build avoids heavy 3D rendering, CUDA, a large ECS framework, networking and ML runtimes in the core process.

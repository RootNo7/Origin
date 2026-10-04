# Architecture

Origin is a CPU-first persistent virtual-world platform. The simulation is authoritative; Godot is a human-facing 3D client and observer.

## Boundaries

`engine/` = simulation authority: time, world, environment, physics, entities, chemistry and future agent-facing rules.

`world/` = concrete world data/content definitions.

`observer/` = bridge, debug observer and Godot 3.6 client.

`ai/` = future LLM agent implementations and cognition modules; no conventional scripted NPC population.

`storage/` = persistence and save-format evolution.

`experiments/` = controlled research configurations.

`tests/` = executable verification.

## 3D foundation

VEarth currently uses a deterministic 3D heightfield: X/Z horizontal, Y vertical. The representation is intentionally lightweight and can later evolve toward chunked/volumetric terrain without giving rendering authority to Godot.

## Data flow

World state → simulation systems → bridge snapshot → Godot client.

Future agent path:

agent request → validated action interface → simulation → authoritative result → perception/feedback.

Godot developer/testing controls remain separate from agent capabilities.

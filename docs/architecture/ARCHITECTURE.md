# Architecture

Origin is a CPU-first persistent virtual-world platform. The simulation is authoritative; Godot is a human-facing 3D client and observer.

## Boundaries

`engine/` = simulation authority: time, world, environment, physics, entities, chemistry and agent-facing rules.

`world/` = concrete world data/content definitions.

`observer/` = bridge, debug observer and Godot 3.6 client.

`ai/` = future LLM agent implementations and cognition modules; no conventional scripted NPC population.

`storage/` = persistence and save-format evolution.

`experiments/` = controlled research configurations.

`tests/` = executable verification.

## 3D foundation

VEarth currently uses a deterministic 3D heightfield: X/Z horizontal, Y vertical. The representation is intentionally lightweight and can later evolve toward chunked or volumetric terrain without giving rendering authority to Godot.

## Data flow

World state → simulation systems → bridge snapshot → Godot client.

Future agent path:

agent request → AgentAction validation → simulation → authoritative result → perception/feedback.

Developer test path:

Godot/test tool → DeveloperBridge → simulation → developer result → state snapshot.

The two paths share authoritative world rules but have different capability boundaries.

## 0.4.0 boundaries

**World** — terrain, environmental cells, resources and persistent revisions.

**Entities** — physical bodies and inventory state. HumanTester is a developer-only actor and cannot be addressed by the normal agent action source.

**Agent action interface** — deliberately narrow and authoritative. Unknown/malformed external commands are explicitly rejected rather than silently disappearing.

**Developer test interface** — pose, respawn and test-only gather for human validation. It is not exposed as an agent capability.

**Observer** — versioned state/terrain snapshots and Godot visualization. The observer never becomes the source of truth.

Terrain revision remains separate from the broader world revision so resource/environment changes do not force geometry rebuilds.

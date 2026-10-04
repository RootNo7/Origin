# ADR 0002 — 3D Simulation Authority

## Decision
Origin is transitioning from the original 2D prototype into a 3D simulation authority while keeping the simulation core independent from Godot rendering.

## Boundaries
- `engine/`: authoritative world, physics, environment, time and future agent-facing rules.
- `observer/`: human-facing visualization and bridge serialization.
- `observer/godot/`: Godot 3.6 first-person test client.
- `ai/`: future agent implementations; no scripted NPC population.
- `storage/`: persistent world snapshots.

## Foundation representation
VEarth begins as a deterministic 3D heightfield. X/Z are horizontal world axes and Y is vertical. This is deliberately simpler than a voxel or volumetric planet so the project can validate interaction and persistence before adding heavier world representations.

## Agent boundary
Future agents will request actions through an explicit interface. The simulation validates actions and produces results; an agent never writes authoritative world state directly.

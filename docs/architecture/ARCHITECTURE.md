# Origin Architecture

## Purpose

Origin is organized by responsibility rather than programming language.
The repository separates the simulated universe, artificial intelligence,
world definitions, observation, persistence, experiments, tests, and tools.

## Top-level boundaries

- `engine/` — generic machinery that makes the universe run.
- `ai/` — inhabitants and intelligence systems; currently reserved for later.
- `world/` — definitions/data for concrete worlds such as VEarth.
- `observer/` — external human-facing visualization and inspection.
- `storage/` — persistence and state serialization.
- `experiments/` — controlled research programs.
- `tests/` — automated verification.
- `tools/` — development and research utilities.
- `docs/` — architecture, scientific assumptions, decisions, and research.
- `config/` — configuration data.
- `scripts/` — thin developer commands.

## Engine boundaries

`engine/` contains the fundamental simulation systems:

```text
Core
  ↓
Simulation
  ├── Time
  ├── World
  ├── Entities
  ├── Physics
  ├── Chemistry
  ├── Environment
  ├── Life
  └── Events
```

The systems exchange explicit state and services. No subsystem should become
an all-purpose manager.

## Reality and Observer

The simulation owns truth about the virtual universe. Observer only reads and
controls the simulation through a defined interface.

```text
VUniverse → Observer
```

not:

```text
Observer → simulation truth
```

Rendering must never be the authority for physical state.

## AI boundary

AI will later interact with the universe through explicit perception and action
interfaces. The simulation must not depend on a particular AI model.

```text
Universe
   │ observations
   ↓
 AI inhabitant
   │ actions
   ↓
Universe
```

An inhabitant must not receive hidden variables, source code, future events, or
omniscient world state merely because those values exist inside the engine.

## World versus engine

`engine/world/` contains generic world mechanics.

`world/earth/` contains VEarth-specific data and definitions.

This prevents Earth-specific assumptions from leaking into the generic engine.

## Persistence

Save files are versioned and inspectable. Loading a state must restore the
simulation state without silently regenerating the world.

## Language strategy

C++20+ is the initial engine language because the simulation needs predictable
performance, numerical control, and straightforward native execution.

The future AI layer may use Python and ML libraries. Language boundaries are
based on subsystem responsibility, not arbitrary source-directory naming.

## Design rules

1. Keep the root small and obvious.
2. Prefer direct code over abstraction layers that solve no current problem.
3. Keep simulation independent from rendering.
4. Keep the universe independent from AI.
5. Keep concrete world data independent from generic engine mechanics.
6. Keep experiments out of production code.
7. Measure before optimizing.
8. Test numerical and persistence behavior.
9. Document scientific approximations.
10. Do not add large frameworks until actual requirements justify them.

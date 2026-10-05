# Origin Architecture

## Active runtime (0.6.0-dev)

Origin is a **Godot 3.6-only application**. The runtime, simulation, rendering, human tester, persistence and developer/test tools live in one process.

```text
Godot 3.6
│
├── Main.gd
│   ├── first-person human tester
│   ├── terrain rendering/collision
│   ├── resource/entity visuals
│   └── developer/test console
│
└── OriginRuntime.gd  ← authoritative world + simulation
    ├── deterministic VEarth terrain
    ├── resources + inventory
    ├── entities + simple physics
    ├── environment + time/calendar
    ├── controlled action validation
    └── transactional persistence
```

There is no required C++ process, bridge executable, CMake build, or Visual Studio toolchain in the normal workflow.

## World authority

`OriginRuntime.gd` owns the truth. Rendering reads its current state. A requested action is not considered successful until the runtime validates and applies it.

The dedicated developer/test actor exists for human testing. The normal agent source is prevented from addressing that actor.

## Persistence

The runtime writes versioned JSON snapshots to `user://origin/`. The current save format is version 6. Format 5 saves load directly and are rewritten as version 6 on the next save. Older native `ORIGIN_SAVE 3/4` text saves can be imported from `user://origin/world.save` or the historical `storage/saves/world.save` location; version-3 saves automatically receive a new HumanTester actor during migration.

Saves are written to a temporary file and published through a backup/rename sequence. Loads validate the complete candidate state before mutating the live runtime.

## Developer/test workflow

The F1 console is inside the same application as the world. It can regenerate, save/load, run tests, benchmark, change simulation speed and capture screenshots without starting another runtime process.

## Native migration

The previous C++ implementation is not part of the active repository. Historical 0.4/0.5 packages can be consulted when investigating the migration. Keeping it out of the active project prevents the old toolchain from becoming an accidental dependency again.

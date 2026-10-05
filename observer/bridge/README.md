# Bridge Protocol

Origin has two deliberately separate external channels.

## Agent channel

`commands.txt` is the future controlled agent-action transport. `results.txt` contains authoritative results.

Current supported command:

```text
gather <actor_id> <resource_id> <amount>
```

The simulation validates actor identity, life state, target existence, numeric state, amount, range and inventory capacity.

Malformed or unsupported commands produce explicit rejection lines such as:

```text
rejected 0 malformed_command
rejected 0 unsupported_command
```

Commands are claimed by renaming `commands.txt` to a `.processing` file before reading, preventing the simulation from consuming a partially written command file. External producers should publish the command file through a temporary file followed by rename.

## Developer/test channel

`test_commands.txt` is reserved for the human Godot test client and development tooling. It supports:

```text
pose <human_test_actor_id> <x> <y> <z>
respawn <human_test_actor_id>
gather <human_test_actor_id> <resource_id> <amount>
```

The pose path is restricted to the current HumanTester actor and still clamps the actor to the world bounds/ground. Developer gathers use the same authoritative gather implementation but are marked `ActionSource::DeveloperTest` internally.

Results are written to `test_results.txt`.

## State

`state.txt` is the current simulation snapshot. `terrain.txt` is a cached terrain snapshot keyed by `terrain_revision`, so time/entity/resource-only changes do not force terrain geometry regeneration.

Current state protocol:

```text
ORIGIN_STATE 4
```

The snapshot includes world time, calendar, revisions, environment values, the HumanTester ID, test inventory, resources and visible entity state.

All published files are written completely to temporary files before publication. On Windows, replacement uses a temporary backup so a failed final rename can restore the previous target.

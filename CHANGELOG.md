# Changelog

## 0.4.0-dev — Reliability + Human Interaction

- Added a developer/test action channel separate from the future agent channel.
- Added authoritative HumanTester actor with pose/respawn controls.
- Added Godot-driven authoritative resource gathering.
- Prevented normal agent actions from impersonating the developer-only HumanTester.
- Hardened bridge command claiming to avoid truncating files while writers publish commands.
- Reworked agent command parsing so malformed and unsupported commands receive explicit rejection results.
- Added command file and line-size limits.
- Hardened save/load validation against non-finite values, duplicate terrain cells, duplicate IDs, invalid inventory stacks and invalid resource state.
- Upgraded save format to version 4 with HumanTester identity; version 3 migration is supported.
- Hardened save/bridge publication on Windows with backup/rollback handling.
- Clamped simulation timestep and speed and hardened calendar conversion at extreme values.
- Hardened physics against invalid body parameters and huge substep counts.
- World revision now reflects environment state updates as well as resource mutations.
- Godot HUD now displays simulation environment data and human-test inventory.
- Godot resource targeting/gathering no longer triggers when the initial click is only recapturing the mouse.
- Added regression tests for malformed commands, developer/agent isolation, corrupted saves, duplicate cells, extreme numeric values, live bridge behavior and finite physics recovery.

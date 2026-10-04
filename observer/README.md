# Observer

The observer layer exposes authoritative simulation state without owning it.

- `application/` — C++ command-line runtime/bridge process.
- `bridge/` — versioned text snapshot consumed by the Godot client.
- `rendering/` — lightweight console observer.
- `godot/` — Godot 3.6 first-person human testing client.

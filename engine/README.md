# Engine

The C++ engine is the authoritative simulation layer.

Current foundation: continuous time, deterministic 3D VEarth terrain, environment updates, 3D entity physics, chemistry placeholder, events, and versioned state/persistence interfaces.

Rendering clients and future AI adapters must consume or request state through explicit interfaces; they must not become a second source of truth.

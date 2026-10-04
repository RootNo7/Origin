# Architecture

Origin 0.1.0 keeps the first working build intentionally small.

```text
VEarth state + simulation clock
            |
            v
      simulation systems
            |
            v
        Godot Observer
```

`project.godot` is at the repository root so Godot imports the project directly.
`observer/Origin.gd` currently contains the small prototype simulation and its observer to minimize integration failure. Later systems can be extracted into `world/`, `simulation/`, and `ai/` without changing the root project entry point.

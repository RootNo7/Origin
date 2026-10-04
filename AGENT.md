# Origin Agent Instructions

Read `idea.yaml`, `NAMING.yaml`, and `docs/ARCHITECTURE.md` before changing the project.

- Target Godot 3.x. Never use Godot 4 APIs.
- The simulation is the source of truth; rendering is an observer.
- Keep the project CPU-first and lightweight.
- Prefer simple readable code and few dependencies.
- Use deterministic seeds and a fixed simulation timestep.
- Do not implement Eris, reproduction, or evolution until requested.
- After every change, run/import the project and fix parser/runtime errors before continuing.
- Do not hide errors with broad exception-like workarounds.
- Document scientific approximations.

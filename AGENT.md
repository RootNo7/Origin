# Origin Engineering Agent Rules

Read `idea.yaml` and `NAMING.yaml` before changing architecture. Preserve the
project's core principle: the universe exists independently of AI, and AI is
an inhabitant rather than the controller.

For this implementation stage, build only the virtual-world foundation.
Do not introduce AI behavior, agent cognition, memory, language, or evolution
logic into the engine just to make a demo work.

Rules:
- Prefer simple, readable, modular C++20 code over framework-heavy design.
- Keep simulation state independent from rendering and Observer code.
- Keep world definitions separate from generic engine mechanics.
- Use explicit units and stable numerical integration.
- Do not claim a scientific system exists when the implementation is only a stub.
- Document simplifications and assumptions.
- Fixed timestep and deterministic seeded generation are required foundations.
- Save/load formats must be versioned and inspectable.
- Add regression tests for important bug fixes.
- Never expose hidden simulation state through future AI interfaces.
- Do not add a new root directory without a real architectural reason.
- Do not introduce ECS, plugin frameworks, microservices, or other large patterns
  unless actual requirements justify them.

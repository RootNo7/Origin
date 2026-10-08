# Origin 0.8.1-dev bug-fix audit

This recovery build was produced after the 0.8.0 graphical/runtime regression reports. The active client is kept on the last confirmed-running Godot path and the new systems are constrained to Godot 3.6/GLES2 APIs.

## Root-level issues found and fixed

1. **Missing runtime base declaration** — `OriginRuntime.gd` had lost `extends Reference` during release-banner editing. The script now declares its Godot 3 base explicitly, and the release validator rejects any active script with no `extends` declaration.

2. **False atomic-load guarantee** — the JSON loader could commit world state before validating the final entity/environment fields. Validation is now complete before mutation; malformed state leaves the existing world untouched.

3. **Unbounded legacy-save loops** — legacy cell/resource/entity counts were not bounded before loops. They are now capped by the same world/runtime limits used by current saves.

4. **World rebuild node leak** — rebuilding/loading a world cleared resource/entity dictionaries without freeing the old scene nodes. The rebuild path now explicitly queues those nodes for deletion before constructing replacements.

5. **Terrain mesh failure path** — a failed array-to-surface conversion was not independently checked. The client now verifies that exactly one terrain surface exists before binding materials/collision.

6. **Godot 4 contamination risk** — the launcher and project are locked to Godot 3.6.x/GLES2; the static audit rejects Godot 4 APIs, unsupported syntax, and generic `godot.exe` launcher fallbacks.

7. **Rendering-risk rollback** — the unstable ProceduralSky/custom-shader path from the 0.8.0 experiment is excluded from the active recovery runtime. The stable path uses Godot 3 environment background color + DirectionalLight and albedo-only materials.

8. **Texture validation** — required medium-detail albedo assets are checked before runtime initialization, preventing missing texture imports from silently turning the scene incomplete.

## What is deliberately not claimed

The container used for this repository audit does not contain the Godot 3.6 executable, so the final graphical F5 launch is not marked as container-verified. The repository does include a Godot self-test and strict static release audit for use with the user's Godot 3.6 installation.

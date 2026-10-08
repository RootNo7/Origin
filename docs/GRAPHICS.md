# Graphics baseline — Origin 0.8.1-dev

Origin remains Godot 3.6-only and GLES2-compatible. The goal of this milestone is not photorealism; it is to replace placeholder geometry/color blocks with coherent, reusable materials while keeping the world light enough for older hardware.

## Material layers

Terrain uses repeated UV-mapped albedo with authoritative geometric normals. Rock, bark, soil, and water use dedicated medium-detail albedo materials. Normal maps remain packaged as optional art assets but are not bound by the active stable GLES2 renderer, avoiding tangent-space and import-related instability.

## World dressing

The current recovery build keeps world dressing intentionally conservative. Visible rock, wood, and soil objects correspond to runtime-owned resource records; rendering does not invent gameplay resources.

## Terrain rendering safeguards

The terrain mesh uses corrected geometric normals derived from the x/z height derivatives. The prior renderer swapped those derivatives, which could create the bright striped/faceted artifact visible in the reported screenshots. Terrain normal mapping is intentionally disabled on the stable GLES2 path; surface materials provide the detail instead.

## Lighting

The runtime uses a Godot 3 `Environment` background color and a DirectionalLight sun. Fog and post-processing are disabled on this recovery path. The simulation clock drives the solar phase, light direction, energy, color, and background transitions.

## Asset policy

No third-party asset is required for the 0.8.1-dev package. The included textures were procedurally authored for this release. External assets can be introduced later only after license/provenance is recorded in the repository.

# Graphics baseline — Origin 0.8.0-dev

Origin remains Godot 3.6-only and GLES2-compatible. The goal of this milestone is not photorealism; it is to replace placeholder geometry/color blocks with coherent, reusable materials while keeping the world light enough for older hardware.

## Material layers

Terrain uses repeated UV-mapped albedo with authoritative geometric normals; sand and cliff zones use dedicated material textures. Secondary meshes use low-strength normal maps where appropriate. Rock, bark, and soil use their own albedo/normal pairs. Leaves use a softer rough material. Water is intentionally opaque on the low-end GLES2 path to avoid transparency sorting artifacts; its texture UVs scroll slowly and the plane has a tiny deterministic vertical motion.

## World dressing

Trees and surface rocks are deterministic decorations derived from the world seed. They are visual-only for now; authoritative physical resources remain owned by the simulation. This keeps rendering from silently creating gameplay objects.

## Terrain rendering safeguards

The terrain mesh uses corrected geometric normals derived from the x/z height derivatives. The prior renderer swapped those derivatives, which could create the bright striped/faceted artifact visible in the reported screenshots. Terrain normal mapping is intentionally disabled on the stable GLES2 path; surface materials provide the detail instead.

## Lighting

The runtime uses the Godot 3 `ProceduralSky`, a DirectionalLight sun, controlled fog, and conservative exposure/contrast values. The simulation clock drives the sun/sky phase.

## Asset policy

No third-party asset is required for the 0.8.0-dev package. The included textures were procedurally authored for this release. External assets can be introduced later only after license/provenance is recorded in the repository.

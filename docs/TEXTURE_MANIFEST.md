# Origin 0.8.1-dev Texture Manifest

The 0.8.1 graphics pass uses original in-project assets generated and processed for Origin. No external texture or model download is required.

## Visible material set

- `terrain_albedo.png` — grass/earth ground base
- `sand_albedo.png` — coastal mineral/sand zone
- `rock_albedo.png` — stone/cliff and rock resources
- `wood_albedo.png` — bark/log resources
- `leaf_albedo.png` — foliage
- `soil_albedo.png` — exposed earth resources
- `water_albedo.png` — sea surface

## Normal maps

`rock_normal.png`, `wood_normal.png`, `soil_normal.png`, `sand_normal.png`, `water_normal.png`, and `terrain_normal.png` are included. The active terrain renderer intentionally does not use `terrain_normal.png`; it uses corrected geometric normals for GLES2 stability.

## Resolution

Base-color and normal maps are 512x512 PNGs, deliberately kept at medium detail to suit the project's low-end hardware target.

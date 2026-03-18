# Spine Godot 3D Port — Progress Tracker

Last updated: 2026-03-18

## Goal

Port Godot runtime presentation from 2D (`Node2D`/`CanvasItem`) to 3D (`Node3D`) while keeping Spine core animation logic untouched.

---

## Completed

### 1) New 3D runtime classes

- Added [`SpineMesh3D`](spine-godot/spine_godot/SpineSprite3D.h)
- Added [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.h)
- Implemented in [`SpineSprite3D.cpp`](spine-godot/spine_godot/SpineSprite3D.cpp)

### 2) Core runtime behavior working in 3D path

- Skeleton loading via `skeleton_data_res`
- Animation playback through existing `SpineAnimationState`
- Per-frame update loop and world transform update
- Per-slot draw-order iteration
- Region attachment quad rendering to `ArrayMesh`
- Vertex color tint + texture sampling
- Manual slot stacking with per-slot Z offset (`depth_separation`)

### 3) Runtime registration/build wiring

- Registered classes in [`register_types.cpp`](spine-godot/spine_godot/register_types.cpp)
  - `GDREGISTER_CLASS(SpineSprite3D)`
  - `GDREGISTER_CLASS(SpineMesh3D)`
- Added source in GDExtension `SConstruct` list:
  - [`SpineSprite3D.cpp`](spine-godot/SConstruct)

### 4) Example usage added

- Example script: [`spine-sprite-3d.gd`](spine-godot/example-v4-extension/tests/spine-sprite-3d.gd)
- Example 3D scene: [`spine-sprite-3d.tscn`](spine-godot/example-v4-extension/tests/spine-sprite-3d.tscn)

---

## Current limitations (intentional / not done yet)

1. Region attachments only (mesh attachments not rendered)
2. Clipping attachments not rendered
3. No two-color tint
4. Advanced blend modes not implemented (normal-like path only)
5. No batching/perf optimization
6. No 3D-specific editor tooling/integration
7. No compile validation from this environment (SCons unavailable)

---

## Priority next steps

## P0 — Must-do next

1. **Build + run validation**
   - Build extension and resolve compile/runtime issues.
   - Open [`spine-sprite-3d.tscn`](spine-godot/example-v4-extension/tests/spine-sprite-3d.tscn) and verify visible animated character.

2. **Support `MeshAttachment`**
   - Extend [`SpineSprite3D::update_meshes()`](spine-godot/spine_godot/SpineSprite3D.cpp) to include mesh attachment geometry/indices.

3. **Clipping support**
   - Reintroduce `SkeletonClipping` branch in 3D path.

## P1 — Visual correctness improvements

4. **Blend modes**
   - Add additive/multiply/screen approximation in 3D material path.

5. **Material strategy cleanup**
   - Evaluate replacing custom shader path with `StandardMaterial3D` where suitable.

6. **Transparency/depth tuning**
   - Validate slot ordering under different camera angles and adjust depth policy.

## P2 — Feature parity / tooling

7. Add optional debug rendering in 3D (bones/regions overlays).
8. Add tests for runtime API compatibility vs `SpineSprite`.
9. Add docs page + migration notes (2D to 3D usage).
10. Add editor conveniences (inspector helpers, icons, sample scene linking).

---

## Notes for continued work

- Keep Spine core untouched; only adapt the Godot presentation layer.
- Keep API as close as possible to [`SpineSprite`](spine-godot/spine_godot/SpineSprite.h).
- Prefer incremental parity: Region -> Mesh -> Clipping -> blend modes -> perf.

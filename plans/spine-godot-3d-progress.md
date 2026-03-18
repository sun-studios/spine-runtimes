# Spine Godot 3D Port — Progress Tracker

Last updated: 2026-03-19

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
- Mesh attachment rendering to `ArrayMesh` (triangles + UVs)
- Defensive mesh data validation (vertex/UV parity + index bounds checks)
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

### 5) Mesh support + build validation update (2026-03-18)

- [`SpineSprite3D::update_meshes()`](spine-godot/spine_godot/SpineSprite3D.cpp) now uses RTTI inheritance checks (`instanceOf`) for `RegionAttachment` and `MeshAttachment`.
- Added geometry guard rails before `ArrayMesh` upload:
  - Vertex/UV pair count validation.
  - Triangle index bounds validation.
- Build validation succeeded in this environment:
  - `scons platform=windows target=template_debug`

### 6) 3D transparency/depth policy update for stable slot ordering (2026-03-19)

- Updated [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.cpp) depth behavior to keep slot ordering stable under camera angle changes:
  - Disabled per-slot depth writes in the slot shader (`depth_draw_never`) so transparent surfaces don't compete via z-buffer.
  - Switched per-vertex Z stacking to planar geometry (`z = 0`) and now apply ordering through `VisualInstance3D.sorting_offset`.
  - Forced `sorting_use_aabb_center = false` per slot mesh to avoid ordering drift caused by varying AABB centers.
  - Added helper methods to centralize/update the policy:
    - `apply_depth_policy(...)`
    - `apply_depth_policy_to_all_meshes()`
  - `set_depth_separation(...)` now reapplies sorting offsets immediately and clamps to non-negative values.
- Build validation succeeded after these changes:
  - `scons platform=windows target=template_debug`

---

## Current limitations (intentional / not done yet)

1. Clipping attachments not rendered
2. No two-color tint
3. Advanced blend modes not implemented (normal-like path only)
4. No batching/perf optimization
5. No 3D-specific editor tooling/integration
6. Runtime visual validation in Godot editor (scene-level confirmation) still pending

---

## Priority next steps

## P0 — Must-do next

1. **Run-time scene validation**
   - Build validation complete via `scons platform=windows target=template_debug`.
   - Open [`spine-sprite-3d.tscn`](spine-godot/example-v4-extension/tests/spine-sprite-3d.tscn) and verify visible animated character, including mesh-heavy skins.

2. **Clipping support**
   - Reintroduce `SkeletonClipping` branch in 3D path.

## P1 — Visual correctness improvements

3. **Blend modes**
   - Add additive/multiply/screen approximation in 3D material path.

4. **Material strategy cleanup**
   - Evaluate replacing custom shader path with `StandardMaterial3D` where suitable.

5. **Transparency/depth tuning**
   - Implemented runtime depth policy update to prioritize stable draw-order sorting.
   - Remaining: scene-level visual confirmation in Godot editor with multiple camera angles.

## P2 — Feature parity / tooling

6. Add optional debug rendering in 3D (bones/regions overlays).
7. Add tests for runtime API compatibility vs `SpineSprite`.
8. Add docs page + migration notes (2D to 3D usage).
9. Add editor conveniences (inspector helpers, icons, sample scene linking).

---

## Notes for continued work

- Keep Spine core untouched; only adapt the Godot presentation layer.
- Keep API as close as possible to [`SpineSprite`](spine-godot/spine_godot/SpineSprite.h).
- Prefer incremental parity: Region -> Mesh (done) -> Clipping -> blend modes -> perf.

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

### 7) 3D orientation + skeleton scale usability update (2026-03-19)

- Updated [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.cpp) to avoid upside-down output in 3D scenes:
  - Apply skeleton Y inversion internally via `skeleton->set_scale_y(-skeleton_scale)` during skeleton initialization and when scale changes.
  - Result: users no longer need to set negative `Node3D` Y scale to get correct orientation.
- Added `skeleton_scale` property to [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.h):
  - Exposed with getter/setter and inspector binding.
  - Default set to `0.01` (Unity-style smaller default world size).
- Updated example scene [`spine-sprite-3d.tscn`](spine-godot/example-v4-extension/tests/spine-sprite-3d.tscn):
  - Replaced node transform scaling with `skeleton_scale = 0.01`.
  - Adjusted camera distance to keep framing appropriate.
- Build validation succeeded after these changes:
  - `scons platform=windows target=template_debug`

### 8) 3D shadow-casting foundation (2026-03-19)

- Added shadow controls to [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.h):
  - `cast_shadow_mode` (inspector enum: `Off,On,DoubleSided,ShadowsOnly`), default `Off`.
  - `shadow_alpha_cutoff` (0..1), default `0.1`.
- Updated [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.cpp) shadow policy handling:
  - New helpers to propagate shadow settings to all slot meshes and materials:
    - `apply_shadow_policy(...)`
    - `apply_shadow_policy_to_all_meshes()`
    - `apply_shadow_parameters_to_material(...)`
    - `apply_shadow_parameters_to_all_materials()`
    - `get_effective_shadow_alpha_cutoff()`
  - New slot meshes now pick up the configured shadow mode on creation.
  - Runtime property changes immediately update existing meshes/materials.
- Updated slot shader/material path in [`SpineSprite3D.cpp`](spine-godot/spine_godot/SpineSprite3D.cpp) for alpha-aware shadow shaping:
  - Added shader uniform `shadow_alpha_cutoff`.
  - Writes `ALPHA_SCISSOR_THRESHOLD` from that uniform.
  - Keeps color rendering behavior unchanged (`ALBEDO`/`ALPHA` from textured vertex tint path).
  - Uses effective cutoff `0` when shadows are disabled to avoid unnecessary alpha scissor clipping.
- Updated example scene [`spine-sprite-3d.tscn`](spine-godot/example-v4-extension/tests/spine-sprite-3d.tscn):
  - Enabled directional light shadows (`shadow_enabled = true`).
  - Added a simple shadow receiver plane with `StandardMaterial3D`.
  - Enabled cast shadows on the demo `SpineSprite3D` (`cast_shadow_mode = On`, `shadow_alpha_cutoff = 0.1`) for immediate visual verification.
- Build validation succeeded after these changes:
  - `scons platform=windows target=template_debug`

### 9) Fix: attachment flicker with inherit-rotation disabled under small 3D scale (2026-03-19)

- Root cause identified in the 3D presentation path, not Spine core:
  - [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.cpp) previously pushed small display scale (eg `0.01`) into skeleton world transforms via `skeleton->set_scale_x/y(...)`.
  - In inherit modes such as `Inherit_NoRotationOrReflection`, Spine core transform code uses thresholded/normalized matrix math, which becomes numerically unstable when parent world matrix values are driven very small by runtime-wide scale.
  - This manifested as frame-to-frame attachment visibility flicker/disappearance on bones in chains affected by inherit-rotation-disabled settings, and was temporarily masked by animator-authored root scale keys above `1.0`.
- Runtime fix implemented in [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.cpp):
  - Keep skeleton transform scale at unit magnitude for stable core math:
    - `set_scale_x(1.0f)`
    - `set_scale_y(-1.0f)` (preserves Y inversion/orientation behavior)
  - Apply user-facing `skeleton_scale` only when writing mesh vertices to `ArrayMesh` (`x/y * skeleton_scale`).
- Result:
  - Removes dependency on obscure root/parent scale animation keys to stabilize rendering.
  - Child attachments under inherit-rotation-disabled bone chains now render consistently at intended 3D display scale.
- Build validation succeeded after the fix:
  - `scons platform=windows target=template_debug`

### Follow-up planned

- Full 3D material parity with [`SpineSprite`](spine-godot/spine_godot/SpineSprite.h) remains a follow-up task (blend-mode coverage and broader material strategy alignment).

### 10) Shadow regression fixes after initial shadow pass (2026-03-19)

- Addressed reported regressions in [`SpineSprite3D`](spine-godot/spine_godot/SpineSprite3D.cpp):
  1. No visible skeleton transparency when `cast_shadow_mode = Off`.
  2. Ambiguous `shadow_alpha_cutoff` default/behavior affecting perceived sprite edges.
  3. No visible shadows cast to receiver surfaces under the initial shadow shader configuration.
  4. Aggressive per-slot z-fighting/clipping when shadows were enabled.
- Implemented separate display and shadow shader paths:
  - `slot_shader`: baseline transparent display path (no alpha scissor write).
  - `slot_shadow_shader`: shadow-capable path with `depth_prepass_alpha` and optional `ALPHA_SCISSOR_THRESHOLD`.
    - Uses lit spatial render mode (`specular_disabled`) and drives visible color through `EMISSION` (`ALBEDO = 0`) so lighting/shadow passes remain active while preserving unlit-like look.
- Added helper methods in [`SpineSprite3D.h`](spine-godot/spine_godot/SpineSprite3D.h):
  - `is_shadow_casting_enabled()`
  - `get_active_slot_shader()`
- Material update flow now switches shaders based on `cast_shadow_mode` and only applies `shadow_alpha_cutoff` on shadow-enabled materials.
- Updated default `shadow_alpha_cutoff` to `0.0` so default appearance remains faithful to Spine visuals; users can increase cutoff specifically for shadow shaping.
- Updated [`SpineSprite3D::update_meshes()`](spine-godot/spine_godot/SpineSprite3D.cpp:394) to apply per-slot vertex Z separation only when shadow casting is enabled, preventing slot z-fighting while preserving non-shadow sort behavior.
- Build validation succeeded after regression fixes:
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
   - Track as part of full material parity follow-up after shadow foundation.

4. **Material strategy cleanup**
   - Evaluate replacing custom shader path with `StandardMaterial3D` where suitable.
   - Revisit with full parity work to avoid regressions in shadow behavior.

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

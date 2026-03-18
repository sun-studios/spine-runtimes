/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated April 5, 2025. Replaces all prior versions.
 *
 * Copyright (c) 2013-2025, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#pragma once

#include "SpineAnimationState.h"
#include "SpineCommon.h"
#include "SpineConstant.h"
#include "SpineSkeleton.h"
#include <unordered_map>
#include <vector>

#if VERSION_MAJOR > 3

#ifdef SPINE_GODOT_EXTENSION
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/texture.hpp>
#else
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/node_3d.h"
#include "scene/resources/array_mesh.h"
#include "scene/resources/material.h"
#include "scene/resources/mesh.h"
#include "scene/resources/shader.h"
#include "scene/resources/shader_material.h"
#include "scene/resources/texture.h"
#endif

class SpineSprite;
class SpineSkin;
struct SpineRendererObject;

class SpineMesh3D : public MeshInstance3D {
	GDCLASS(SpineMesh3D, MeshInstance3D)

protected:
	static void _bind_methods();

private:
	Ref<ArrayMesh> array_mesh;

public:
	SpineMesh3D();
	void clear_mesh();
	void update_mesh(const PackedVector3Array &vertices,
				 const PackedVector2Array &uvs,
				 const PackedColorArray &colors,
				 const PackedInt32Array &indices,
				 const Ref<Material> &material);
};

class SpineSprite3D : public Node3D,
					 public spine::AnimationStateListenerObject {
	GDCLASS(SpineSprite3D, Node3D)

protected:
	Ref<SpineSkeletonDataResource> skeleton_data_res;
	SpineSprite *spine_proxy;
	std::vector<SpineMesh3D *> mesh_instances;

	SpineConstant::UpdateMode update_mode;
	float time_scale;
	float depth_separation;

	spine::Vector<unsigned short> quad_indices;
	spine::Vector<float> scratch_vertices;

	Ref<Shader> slot_shader;
	Ref<Material> fallback_material;
	std::unordered_map<uint64_t, Ref<Material>> material_cache;

	static void _bind_methods();
	void _notification(int what);

	void generate_meshes_for_slots(Ref<SpineSkeleton> skeleton_ref);
	void remove_meshes();
	void update_meshes(Ref<SpineSkeleton> skeleton_ref);
	void apply_depth_policy(SpineMesh3D *mesh_instance, int draw_order_index);
	void apply_depth_policy_to_all_meshes();

	Ref<Material> create_slot_material(const Ref<Texture> &texture, bool has_texture);
	Ref<Material> get_or_create_slot_material(SpineRendererObject *renderer_object);

	void callback(spine::AnimationState *state, spine::EventType type, spine::TrackEntry *entry, spine::Event *event) override;

public:
	SpineSprite3D();
	~SpineSprite3D() override;

	void set_skeleton_data_res(const Ref<SpineSkeletonDataResource> &skeleton_data_resource);
	Ref<SpineSkeletonDataResource> get_skeleton_data_res();

	Ref<SpineSkeleton> get_skeleton();
	Ref<SpineAnimationState> get_animation_state();

	void on_skeleton_data_changed();
	void update_skeleton(float delta);

	void set_update_mode(SpineConstant::UpdateMode mode);
	SpineConstant::UpdateMode get_update_mode();

	void set_time_scale(float value);
	float get_time_scale();

	void set_depth_separation(float value);
	float get_depth_separation();

	Ref<SpineSkin> new_skin(const String &name);
};

#endif


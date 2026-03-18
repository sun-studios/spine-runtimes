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

#include "SpineSprite3D.h"

#if VERSION_MAJOR > 3

#include "SpineEvent.h"
#include "SpineRendererObject.h"
#include "SpineSprite.h"
#include "SpineTrackEntry.h"

void SpineMesh3D::_bind_methods() {
}

SpineMesh3D::SpineMesh3D() {
	array_mesh.instantiate();
	set_mesh(array_mesh);
	set_cast_shadows_setting(GeometryInstance3D::SHADOW_CASTING_SETTING_OFF);
}

void SpineMesh3D::clear_mesh() {
	if (array_mesh.is_valid()) {
		array_mesh->clear_surfaces();
	}
}

void SpineMesh3D::update_mesh(const PackedVector3Array &vertices,
					  const PackedVector2Array &uvs,
					  const PackedColorArray &colors,
					  const PackedInt32Array &indices,
					  const Ref<Material> &material) {
	if (!array_mesh.is_valid()) {
		array_mesh.instantiate();
		set_mesh(array_mesh);
	}

	array_mesh->clear_surfaces();
	if (indices.is_empty()) return;

	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	arrays[Mesh::ARRAY_VERTEX] = vertices;
	arrays[Mesh::ARRAY_TEX_UV] = uvs;
	arrays[Mesh::ARRAY_COLOR] = colors;
	arrays[Mesh::ARRAY_INDEX] = indices;

	array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
	if (material.is_valid()) {
		array_mesh->surface_set_material(0, material);
	}
}

void SpineSprite3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_skeleton_data_res", "skeleton_data_res"), &SpineSprite3D::set_skeleton_data_res);
	ClassDB::bind_method(D_METHOD("get_skeleton_data_res"), &SpineSprite3D::get_skeleton_data_res);
	ClassDB::bind_method(D_METHOD("get_skeleton"), &SpineSprite3D::get_skeleton);
	ClassDB::bind_method(D_METHOD("get_animation_state"), &SpineSprite3D::get_animation_state);
	ClassDB::bind_method(D_METHOD("on_skeleton_data_changed"), &SpineSprite3D::on_skeleton_data_changed);
	ClassDB::bind_method(D_METHOD("update_skeleton", "delta"), &SpineSprite3D::update_skeleton);
	ClassDB::bind_method(D_METHOD("new_skin", "name"), &SpineSprite3D::new_skin);

	ClassDB::bind_method(D_METHOD("set_update_mode", "v"), &SpineSprite3D::set_update_mode);
	ClassDB::bind_method(D_METHOD("get_update_mode"), &SpineSprite3D::get_update_mode);
	ClassDB::bind_method(D_METHOD("set_time_scale", "v"), &SpineSprite3D::set_time_scale);
	ClassDB::bind_method(D_METHOD("get_time_scale"), &SpineSprite3D::get_time_scale);
	ClassDB::bind_method(D_METHOD("set_depth_separation", "v"), &SpineSprite3D::set_depth_separation);
	ClassDB::bind_method(D_METHOD("get_depth_separation"), &SpineSprite3D::get_depth_separation);

	ADD_SIGNAL(MethodInfo("animation_started", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D"), PropertyInfo(Variant::OBJECT, "animation_state", PROPERTY_HINT_TYPE_STRING, "SpineAnimationState"), PropertyInfo(Variant::OBJECT, "track_entry", PROPERTY_HINT_TYPE_STRING, "SpineTrackEntry")));
	ADD_SIGNAL(MethodInfo("animation_interrupted", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D"), PropertyInfo(Variant::OBJECT, "animation_state", PROPERTY_HINT_TYPE_STRING, "SpineAnimationState"), PropertyInfo(Variant::OBJECT, "track_entry", PROPERTY_HINT_TYPE_STRING, "SpineTrackEntry")));
	ADD_SIGNAL(MethodInfo("animation_ended", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D"), PropertyInfo(Variant::OBJECT, "animation_state", PROPERTY_HINT_TYPE_STRING, "SpineAnimationState"), PropertyInfo(Variant::OBJECT, "track_entry", PROPERTY_HINT_TYPE_STRING, "SpineTrackEntry")));
	ADD_SIGNAL(MethodInfo("animation_completed", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D"), PropertyInfo(Variant::OBJECT, "animation_state", PROPERTY_HINT_TYPE_STRING, "SpineAnimationState"), PropertyInfo(Variant::OBJECT, "track_entry", PROPERTY_HINT_TYPE_STRING, "SpineTrackEntry")));
	ADD_SIGNAL(MethodInfo("animation_disposed", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D"), PropertyInfo(Variant::OBJECT, "animation_state", PROPERTY_HINT_TYPE_STRING, "SpineAnimationState"), PropertyInfo(Variant::OBJECT, "track_entry", PROPERTY_HINT_TYPE_STRING, "SpineTrackEntry")));
	ADD_SIGNAL(MethodInfo("animation_event", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D"), PropertyInfo(Variant::OBJECT, "animation_state", PROPERTY_HINT_TYPE_STRING, "SpineAnimationState"), PropertyInfo(Variant::OBJECT, "track_entry", PROPERTY_HINT_TYPE_STRING, "SpineTrackEntry"), PropertyInfo(Variant::OBJECT, "event", PROPERTY_HINT_TYPE_STRING, "SpineEvent")));
	ADD_SIGNAL(MethodInfo("before_animation_state_update", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D")));
	ADD_SIGNAL(MethodInfo("before_animation_state_apply", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D")));
	ADD_SIGNAL(MethodInfo("before_world_transforms_change", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D")));
	ADD_SIGNAL(MethodInfo("world_transforms_changed", PropertyInfo(Variant::OBJECT, "spine_sprite", PROPERTY_HINT_TYPE_STRING, "SpineSprite3D")));

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "skeleton_data_res", PROPERTY_HINT_RESOURCE_TYPE, "SpineSkeletonDataResource"), "set_skeleton_data_res", "get_skeleton_data_res");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "update_mode", PROPERTY_HINT_ENUM, "Process,Physics,Manual"), "set_update_mode", "get_update_mode");
	ADD_PROPERTY(PropertyInfo(VARIANT_FLOAT, "time_scale"), "set_time_scale", "get_time_scale");
	ADD_PROPERTY(PropertyInfo(VARIANT_FLOAT, "depth_separation", PROPERTY_HINT_RANGE, "0.0001,0.1,0.0001"), "set_depth_separation", "get_depth_separation");
}

SpineSprite3D::SpineSprite3D() : spine_proxy(memnew(SpineSprite)), update_mode(SpineConstant::UpdateMode_Process), time_scale(1.0f), depth_separation(0.001f) {
	quad_indices.setSize(6, 0);
	quad_indices[0] = 0;
	quad_indices[1] = 1;
	quad_indices[2] = 2;
	quad_indices[3] = 2;
	quad_indices[4] = 3;
	quad_indices[5] = 0;
	scratch_vertices.ensureCapacity(1200);

	spine_proxy->set_update_mode(SpineConstant::UpdateMode_Manual);

	slot_shader.instantiate();
	slot_shader->set_code(
			"shader_type spatial;\n"
			"render_mode unshaded, cull_disabled;\n"
			"uniform sampler2D spine_texture : source_color;\n"
			"uniform bool use_texture = true;\n"
			"void fragment() {\n"
			"\tvec4 c = COLOR;\n"
			"\tif (use_texture) c *= texture(spine_texture, UV);\n"
			"\tALBEDO = c.rgb;\n"
			"\tALPHA = c.a;\n"
			"}\n");

	fallback_material = create_slot_material(Ref<Texture>(), false);
}

SpineSprite3D::~SpineSprite3D() {
	remove_meshes();
	material_cache.clear();
	fallback_material.unref();
	slot_shader.unref();

	if (spine_proxy) {
		spine_proxy->emit_signal(SNAME("_internal_spine_objects_invalidated"));
		memdelete(spine_proxy);
		spine_proxy = nullptr;
	}
}

void SpineSprite3D::set_skeleton_data_res(const Ref<SpineSkeletonDataResource> &skeleton_data_resource) {
	skeleton_data_res = skeleton_data_resource;
	on_skeleton_data_changed();
}

Ref<SpineSkeletonDataResource> SpineSprite3D::get_skeleton_data_res() {
	return skeleton_data_res;
}

Ref<SpineSkeleton> SpineSprite3D::get_skeleton() {
	if (!spine_proxy) return nullptr;
	return spine_proxy->get_skeleton();
}

Ref<SpineAnimationState> SpineSprite3D::get_animation_state() {
	if (!spine_proxy) return nullptr;
	return spine_proxy->get_animation_state();
}

void SpineSprite3D::on_skeleton_data_changed() {
	remove_meshes();
	material_cache.clear();

	if (!spine_proxy) return;

	if (skeleton_data_res.is_valid()) {
		if (!skeleton_data_res->is_connected(SNAME("skeleton_data_changed"), callable_mp(this, &SpineSprite3D::on_skeleton_data_changed))) {
			skeleton_data_res->connect(SNAME("skeleton_data_changed"), callable_mp(this, &SpineSprite3D::on_skeleton_data_changed));
		}
	}

	spine_proxy->set_skeleton_data_res(skeleton_data_res);

	Ref<SpineAnimationState> animation_state = get_animation_state();
	if (animation_state.is_valid() && animation_state->get_spine_object()) {
		animation_state->get_spine_object()->setListener(this);
	}

	Ref<SpineSkeleton> skeleton = get_skeleton();
	if (skeleton.is_valid() && skeleton->get_spine_object()) {
		generate_meshes_for_slots(skeleton);
		update_skeleton(0);
	}
}

void SpineSprite3D::generate_meshes_for_slots(Ref<SpineSkeleton> skeleton_ref) {
	auto spine_skeleton = skeleton_ref->get_spine_object();
	if (!spine_skeleton) return;

	for (int i = 0, n = (int) spine_skeleton->getSlots().size(); i < n; i++) {
		auto mesh_instance = memnew(SpineMesh3D);
		add_child(mesh_instance);
		mesh_instances.push_back(mesh_instance);
	}
}

void SpineSprite3D::remove_meshes() {
	for (auto *mesh_instance: mesh_instances) {
		if (!mesh_instance) continue;
		remove_child(mesh_instance);
		memdelete(mesh_instance);
	}
	mesh_instances.clear();
}

void SpineSprite3D::_notification(int what) {
		switch (what) {
			case NOTIFICATION_READY: {
				set_process_internal(update_mode == SpineConstant::UpdateMode_Process);
				set_physics_process_internal(update_mode == SpineConstant::UpdateMode_Physics);
				update_skeleton(0);
				break;
			}
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (update_mode == SpineConstant::UpdateMode_Process) update_skeleton(get_process_delta_time());
			break;
		}
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (update_mode == SpineConstant::UpdateMode_Physics) update_skeleton(get_physics_process_delta_time());
			break;
		}
		default:
			break;
	}
}

void SpineSprite3D::update_skeleton(float delta) {
	if (!skeleton_data_res.is_valid() ||
		!skeleton_data_res->is_skeleton_data_loaded() ||
		!get_skeleton().is_valid() ||
		!get_skeleton()->get_spine_object() ||
		!get_animation_state().is_valid() ||
		!get_animation_state()->get_spine_object())
		return;

	emit_signal(SNAME("before_animation_state_update"), this);
	get_animation_state()->update(delta * time_scale);
	if (!is_visible_in_tree()) return;
	emit_signal(SNAME("before_animation_state_apply"), this);
	get_animation_state()->apply(get_skeleton());
	emit_signal(SNAME("before_world_transforms_change"), this);
	get_skeleton()->update(delta * time_scale);
	get_skeleton()->update_world_transform(SpineConstant::Physics_Update);
	emit_signal(SNAME("world_transforms_changed"), this);

	update_meshes(get_skeleton());
}

Ref<Material> SpineSprite3D::create_slot_material(const Ref<Texture> &texture, bool has_texture) {
	Ref<ShaderMaterial> material(memnew(ShaderMaterial));
	material->set_shader(slot_shader);
	material->set_shader_parameter(SNAME("use_texture"), has_texture);
	if (has_texture && texture.is_valid()) {
		material->set_shader_parameter(SNAME("spine_texture"), texture);
	}
	return material;
}

Ref<Material> SpineSprite3D::get_or_create_slot_material(SpineRendererObject *renderer_object) {
	if (!renderer_object || renderer_object->texture.is_null()) {
		return fallback_material;
	}

	RID texture_rid = renderer_object->texture->get_rid();
	uint64_t key = texture_rid.get_id();

	auto found = material_cache.find(key);
	if (found != material_cache.end()) {
		return found->second;
	}

	Ref<Material> material = create_slot_material(renderer_object->texture, true);
	material_cache[key] = material;
	return material;
}

void SpineSprite3D::update_meshes(Ref<SpineSkeleton> skeleton_ref) {
	auto skeleton = skeleton_ref->get_spine_object();
	if (!skeleton) return;

	for (int i = 0, n = (int) skeleton->getSlots().size(); i < n; ++i) {
		spine::Slot *slot = skeleton->getDrawOrder()[i];
		spine::Attachment *attachment = slot->getAttachment();
		SpineMesh3D *mesh_instance = mesh_instances[i];

		if (!attachment || !slot->getBone().isActive()) {
			mesh_instance->clear_mesh();
			continue;
		}

		SpineRendererObject *renderer_object = nullptr;
		spine::Vector<float> *vertices = nullptr;
		spine::Vector<float> *uvs = nullptr;
		spine::Vector<unsigned short> *indices = nullptr;
		spine::Color attachment_color(1, 1, 1, 1);

		if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) {
			auto *region = (spine::RegionAttachment *) attachment;
			scratch_vertices.setSize(8, 0);
			region->computeWorldVertices(*slot, scratch_vertices, 0);

			auto *atlas_region = (spine::AtlasRegion *) region->getRegion();
			if (!atlas_region || !atlas_region->page) {
				mesh_instance->clear_mesh();
				continue;
			}

			renderer_object = (SpineRendererObject *) atlas_region->page->texture;
			vertices = &scratch_vertices;
			uvs = &region->getUVs();
			indices = &quad_indices;
			attachment_color = region->getColor();
		} else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) {
			auto *mesh = (spine::MeshAttachment *) attachment;
			scratch_vertices.setSize(mesh->getWorldVerticesLength(), 0);
			mesh->computeWorldVertices(*slot, scratch_vertices);

			auto *atlas_region = (spine::AtlasRegion *) mesh->getRegion();
			if (!atlas_region || !atlas_region->page) {
				mesh_instance->clear_mesh();
				continue;
			}

			renderer_object = (SpineRendererObject *) atlas_region->page->texture;
			vertices = &scratch_vertices;
			uvs = &mesh->getUVs();
			indices = &mesh->getTriangles();
			attachment_color = mesh->getColor();
		} else {
			mesh_instance->clear_mesh();
			continue;
		}

		if (!vertices || !uvs || !indices || vertices->size() < 2 || uvs->size() < 2 || indices->size() == 0) {
			mesh_instance->clear_mesh();
			continue;
		}

		int num_vertices = (int) (vertices->size() / 2);
		if (num_vertices <= 0) {
			mesh_instance->clear_mesh();
			continue;
		}

		Ref<Material> material = get_or_create_slot_material(renderer_object);

		spine::Color skeleton_color = skeleton->getColor();
		spine::Color slot_color = slot->getColor();
		Color tint(
			skeleton_color.r * slot_color.r * attachment_color.r,
			skeleton_color.g * slot_color.g * attachment_color.g,
			skeleton_color.b * slot_color.b * attachment_color.b,
			skeleton_color.a * slot_color.a * attachment_color.a);

		PackedVector3Array vertices_array;
		vertices_array.resize(num_vertices);
		PackedVector2Array uvs_array;
		uvs_array.resize(num_vertices);
		PackedColorArray colors;
		colors.resize(num_vertices);
		PackedInt32Array indices_array;
		indices_array.resize((int) indices->size());

		float z = (float) i * depth_separation;
		for (int vertex_index = 0; vertex_index < num_vertices; vertex_index++) {
			int float_index = vertex_index * 2;
			float x = vertices->buffer()[float_index];
			float y = vertices->buffer()[float_index + 1];
			vertices_array.set(vertex_index, Vector3(x, y, z));
			uvs_array.set(vertex_index, Vector2(uvs->buffer()[float_index], uvs->buffer()[float_index + 1]));
			colors.set(vertex_index, tint);
		}

		for (int index = 0; index < (int) indices->size(); index++) {
			indices_array.set(index, (int) indices->buffer()[index]);
		}

		mesh_instance->update_mesh(vertices_array, uvs_array, colors, indices_array, material);
	}
}

void SpineSprite3D::callback(spine::AnimationState *state, spine::EventType type, spine::TrackEntry *entry, spine::Event *event) {
	Ref<SpineTrackEntry> entry_ref = Ref<SpineTrackEntry>(memnew(SpineTrackEntry));
	entry_ref->set_spine_object(spine_proxy, entry);

	Ref<SpineEvent> event_ref(nullptr);
	if (event) {
		event_ref = Ref<SpineEvent>(memnew(SpineEvent));
		event_ref->set_spine_object(spine_proxy, event);
	}

	switch (type) {
		case spine::EventType_Start:
			emit_signal(SNAME("animation_started"), this, get_animation_state(), entry_ref);
			break;
		case spine::EventType_Interrupt:
			emit_signal(SNAME("animation_interrupted"), this, get_animation_state(), entry_ref);
			break;
		case spine::EventType_End:
			emit_signal(SNAME("animation_ended"), this, get_animation_state(), entry_ref);
			break;
		case spine::EventType_Complete:
			emit_signal(SNAME("animation_completed"), this, get_animation_state(), entry_ref);
			break;
		case spine::EventType_Dispose:
			emit_signal(SNAME("animation_disposed"), this, get_animation_state(), entry_ref);
			break;
		case spine::EventType_Event:
			emit_signal(SNAME("animation_event"), this, get_animation_state(), entry_ref, event_ref);
			break;
	}
}

void SpineSprite3D::set_update_mode(SpineConstant::UpdateMode mode) {
	update_mode = mode;
	set_process_internal(update_mode == SpineConstant::UpdateMode_Process);
	set_physics_process_internal(update_mode == SpineConstant::UpdateMode_Physics);
}

SpineConstant::UpdateMode SpineSprite3D::get_update_mode() {
	return update_mode;
}

void SpineSprite3D::set_time_scale(float value) {
	time_scale = value;
}

float SpineSprite3D::get_time_scale() {
	return time_scale;
}

void SpineSprite3D::set_depth_separation(float value) {
	depth_separation = value;
}

float SpineSprite3D::get_depth_separation() {
	return depth_separation;
}

Ref<SpineSkin> SpineSprite3D::new_skin(const String &name) {
	if (!spine_proxy) return nullptr;
	Ref<SpineSkin> skin = memnew(SpineSkin);
	skin->init(name, spine_proxy);
	return skin;
}

#endif


extends Node3D

@onready var spine_sprite_3d: SpineSprite3D = $SpineSprite3D

func _ready():
	if spine_sprite_3d.skeleton_data_res:
		var animation_state: SpineAnimationState = spine_sprite_3d.get_animation_state()
		if animation_state:
			animation_state.set_animation("walk", true, 0)


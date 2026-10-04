extends KinematicBody

export(float) var move_speed = 6.0
export(float) var jump_speed = 6.0
export(float) var gravity = 18.0
export(float) var mouse_sensitivity = 0.0025

var velocity = Vector3.ZERO
var pitch = 0.0
var camera = null

func _ready():
    camera = $Camera
    Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _unhandled_input(event):
    if event is InputEventMouseMotion and Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
        rotate_y(-event.relative.x * mouse_sensitivity)
        pitch = clamp(pitch - event.relative.y * mouse_sensitivity, -1.45, 1.45)
        camera.rotation.x = pitch
    elif event is InputEventKey and event.pressed and not event.echo and event.scancode == KEY_ESCAPE:
        Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
    elif event is InputEventMouseButton and event.pressed and event.button_index == BUTTON_LEFT:
        Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _physics_process(delta):
    var input_vec = Vector2(
        float(Input.is_key_pressed(KEY_D)) - float(Input.is_key_pressed(KEY_A)),
        float(Input.is_key_pressed(KEY_S)) - float(Input.is_key_pressed(KEY_W))
    ).clamped(1.0)

    var basis = global_transform.basis
    var direction = (basis.x * input_vec.x) + (basis.z * input_vec.y)
    direction.y = 0.0
    if direction.length_squared() > 0.0:
        direction = direction.normalized()

    velocity.x = direction.x * move_speed
    velocity.z = direction.z * move_speed
    velocity.y -= gravity * delta

    if Input.is_key_pressed(KEY_SPACE) and is_on_floor():
        velocity.y = jump_speed

    velocity = move_and_slide(velocity, Vector3.UP, false, 4, deg2rad(45.0), false)

func deg2rad(value):
    return value * PI / 180.0

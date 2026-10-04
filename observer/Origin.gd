extends Node2D

# Godot 3.x only. No get_font(), no Godot 4 APIs.
const WORLD_WIDTH = 160
const WORLD_HEIGHT = 80
const FIXED_STEP = 0.05
const DEFAULT_SEED = 7

var rng = RandomNumberGenerator.new()
var seed_value = DEFAULT_SEED
var paused = false
var accumulator = 0.0
var sim_time = 0.0
var tick = 0
var day_length = 60.0
var terrain = []
var temperature = []
var entities = []
var pan = Vector2.ZERO
var zoom = 1.0
var dragging = false

var title_label = null
var stats_label = null
var help_label = null
var state_label = null

func _ready():
    rng.seed = DEFAULT_SEED
    _build_ui()
    reset_world(DEFAULT_SEED)
    set_process(true)
    update()

func _build_ui():
    var layer = CanvasLayer.new()
    layer.name = "UI"
    add_child(layer)

    var header = ColorRect.new()
    header.rect_position = Vector2(20, 20)
    header.rect_size = Vector2(1060, 95)
    header.color = Color("18212b")
    layer.add_child(header)

    title_label = Label.new()
    title_label.rect_position = Vector2(20, 8)
    title_label.rect_size = Vector2(600, 30)
    title_label.text = "ORIGIN / VEarth"
    title_label.add_color_override("font_color", Color("e8eef5"))
    title_label.add_font_size_override("font_size", 22)
    header.add_child(title_label)

    stats_label = Label.new()
    stats_label.rect_position = Vector2(20, 42)
    stats_label.rect_size = Vector2(1000, 22)
    stats_label.add_color_override("font_color", Color("aebdcc"))
    header.add_child(stats_label)

    help_label = Label.new()
    help_label.rect_position = Vector2(20, 68)
    help_label.rect_size = Vector2(700, 22)
    help_label.text = "SPACE: pause/resume    R: reset    Wheel: zoom    Left drag: pan"
    help_label.add_color_override("font_color", Color("8292a3"))
    header.add_child(help_label)

    state_label = Label.new()
    state_label.rect_position = Vector2(820, 12)
    state_label.rect_size = Vector2(210, 25)
    state_label.align = Label.ALIGN_RIGHT
    state_label.add_color_override("font_color", Color("cbd7e3"))
    header.add_child(state_label)

func reset_world(new_seed):
    seed_value = int(new_seed)
    rng.seed = seed_value
    paused = false
    accumulator = 0.0
    sim_time = 0.0
    tick = 0
    pan = Vector2.ZERO
    zoom = 1.0
    terrain.clear()
    temperature.clear()
    entities.clear()

    var previous = 28.0
    for x in range(WORLD_WIDTH):
        var wave = sin(float(x) * 0.075) * 7.0 + sin(float(x) * 0.19) * 2.5
        previous = clamp(previous * 0.82 + (28.0 + wave) * 0.18 + rng.randf_range(-2.0, 2.0), 7.0, 58.0)
        terrain.append(previous)
        temperature.append(285.0 + sin(float(x) * 0.035) * 5.0)

    entities.append({"name":"Stone-1", "position":Vector2(35, 55), "velocity":Vector2(3, 0), "radius":1.2})
    entities.append({"name":"Stone-2", "position":Vector2(105, 62), "velocity":Vector2(-2, 0), "radius":1.2})

func _process(delta):
    if not paused:
        accumulator += min(delta, 0.25)
        while accumulator >= FIXED_STEP:
            _simulate_step(FIXED_STEP)
            accumulator -= FIXED_STEP
    _update_ui()
    update()

func _simulate_step(dt):
    tick += 1
    sim_time += dt

    var phase = fmod(sim_time, day_length) / day_length
    var sunlight = max(0.0, sin(phase * PI * 2.0 - PI * 0.5))
    var target_global = 282.0 + sunlight * 12.0

    for x in range(WORLD_WIDTH):
        var target = target_global + sin(float(x) * 0.035) * 4.0
        temperature[x] = lerp(temperature[x], target, min(1.0, dt / 8.0))

    # Godot 2D Y grows downward, so positive Y is gravity.
    for entity in entities:
        var velocity = entity["velocity"]
        var position = entity["position"]
        velocity.y += 9.81 * dt
        position += velocity * dt
        position.x = clamp(position.x, 0.0, float(WORLD_WIDTH) - 0.001)
        var ix = int(clamp(floor(position.x), 0, WORLD_WIDTH - 1))
        var floor_y = terrain[ix] + entity["radius"]
        if position.y > floor_y:
            position.y = floor_y
            if abs(velocity.y) > 0.25:
                velocity.y = -velocity.y * 0.28
            else:
                velocity.y = 0.0
            velocity.x *= 0.985
        entity["velocity"] = velocity
        entity["position"] = position

func _input(event):
    if event is InputEventKey and event.pressed and not event.echo:
        if event.scancode == KEY_SPACE:
            paused = not paused
        elif event.scancode == KEY_R:
            reset_world(DEFAULT_SEED)

    if event is InputEventMouseButton:
        if event.button_index == BUTTON_WHEEL_UP and event.pressed:
            zoom = clamp(zoom * 1.1, 0.5, 3.0)
        elif event.button_index == BUTTON_WHEEL_DOWN and event.pressed:
            zoom = clamp(zoom / 1.1, 0.5, 3.0)
        elif event.button_index == BUTTON_LEFT:
            dragging = event.pressed

    if event is InputEventMouseMotion and dragging:
        pan += event.relative

func _update_ui():
    if stats_label == null:
        return
    var avg = 0.0
    for value in temperature:
        avg += value
    if temperature.size() > 0:
        avg /= temperature.size()
    var phase = fmod(sim_time, day_length) / day_length
    var sunlight = max(0.0, sin(phase * PI * 2.0 - PI * 0.5))
    stats_label.text = "tick %d    time %.2f s    seed %d    entities %d    avg %.2f K    sun %.3f" % [tick, sim_time, seed_value, entities.size(), avg, sunlight]
    state_label.text = "VEarth: PAUSED" if paused else "VEarth: RUNNING"

func _draw():
    var viewport = get_viewport_rect().size
    draw_rect(Rect2(Vector2.ZERO, viewport), Color("10151c"))

    var area = Rect2(30, 130, max(100.0, viewport.x - 60.0), max(200.0, viewport.y - 160.0))
    draw_rect(area, Color("0d1117"))

    var sx = area.size.x / float(WORLD_WIDTH) * zoom
    var base_y = area.position.y + area.size.y + pan.y
    var sea_y = base_y - 25.0 * 6.0

    for x in range(WORLD_WIDTH):
        var px = area.position.x + float(x) * sx + pan.x
        if px > area.end.x or px + sx < area.position.x:
            continue
        var ground_y = base_y - terrain[x] * 6.0
        draw_rect(Rect2(px, sea_y, sx + 1.0, base_y - sea_y), Color("17364a"))
        draw_rect(Rect2(px, ground_y, sx + 1.0, base_y - ground_y), Color("314536"))

    for entity in entities:
        var px = area.position.x + entity["position"].x * sx + pan.x
        var py = base_y - entity["position"].y * 6.0 + pan.y
        draw_circle(Vector2(px, py), max(3.0, entity["radius"] * sx), Color("d9a441"))

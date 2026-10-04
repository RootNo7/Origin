extends Spatial

const STATE_PATH = "res://observer/bridge/state.txt"
const PlayerScript = preload("res://observer/godot/scripts/Player.gd")

var state_time = 0.0
var state_tick = 0
var state_width = 1
var state_depth = 1
var state_sea = 8.0
var state_temp = 288.15
var state_sun = 1.0
var state_world_revision = -1
var terrain_heights = []
var entities = []
var terrain_mesh = null
var terrain_collision = null
var water_node = null
var player = null
var status_label = null
var banner_label = null
var entity_nodes = {}
var terrain_dirty = false

func _ready():
    _create_environment()
    _create_hud()
    _refresh_state()
    _rebuild_terrain()
    _create_player()
    _sync_entities()
    set_process(true)

func _process(_delta):
    if _refresh_state():
        if terrain_dirty:
            _rebuild_terrain()
            _reset_player_if_needed()
        _sync_entities()
    if player:
        status_label.text = "ORIGIN  |  t=%.1fs  tick=%d  temp=%.1fK  sun=%.2f\nFPS: %d  |  WASD move  Space jump  Esc mouse"
            % [state_time, state_tick, state_temp, state_sun, Engine.get_frames_per_second()]

func _create_environment():
    var world_env = WorldEnvironment.new()
    var environment = Environment.new()
    environment.background_mode = Environment.BG_COLOR
    environment.background_color = Color(0.03, 0.05, 0.08)
    environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
    environment.ambient_light_color = Color(0.6, 0.65, 0.72)
    environment.ambient_light_energy = 0.65
    world_env.environment = environment
    add_child(world_env)

    var sun = DirectionalLight.new()
    sun.name = "Sun"
    sun.light_energy = 1.1
    sun.rotation_degrees = Vector3(-55, -30, 0)
    add_child(sun)

func _create_hud():
    var canvas = CanvasLayer.new()
    canvas.name = "HUD"
    add_child(canvas)

    status_label = Label.new()
    status_label.rect_position = Vector2(18, 16)
    status_label.add_color_override("font_color", Color(0.86, 0.92, 1.0))
    status_label.text = "ORIGIN"
    canvas.add_child(status_label)

    banner_label = Label.new()
    banner_label.rect_position = Vector2(18, 96)
    banner_label.add_color_override("font_color", Color(0.65, 0.78, 0.92))
    banner_label.text = "3D human test client • C++ simulation is authoritative"
    canvas.add_child(banner_label)

func _refresh_state():
    var file = File.new()
    if not file.file_exists(STATE_PATH):
        banner_label.text = "Waiting for C++ bridge: observer/bridge/state.txt"
        return false
    if file.open(STATE_PATH, File.READ) != OK:
        return false

    var lines = file.get_as_text().split("\n")
    file.close()

    var new_heights = []
    var new_entities = []
    var in_terrain = false
    for line in lines:
        var trimmed = line.strip_edges()
        if trimmed == "terrain":
            in_terrain = true
            continue
        if trimmed.begins_with("entities "):
            in_terrain = false
            continue
        if trimmed == "END":
            break
        var parts = trimmed.split(" ")
        if parts.size() < 2:
            continue

        if parts[0] == "ORIGIN_STATE":
            continue
        elif parts[0] == "time":
            state_time = float(parts[1])
        elif parts[0] == "tick":
            state_tick = int(parts[1])
        elif parts[0] == "width":
            state_width = max(1, int(parts[1]))
        elif parts[0] == "depth":
            state_depth = max(1, int(parts[1]))
        elif parts[0] == "sea_level":
            state_sea = float(parts[1])
        elif parts[0] == "world_revision":
            var incoming_revision = int(parts[1])
            terrain_dirty = incoming_revision != state_world_revision
            state_world_revision = incoming_revision
        elif parts[0] == "temperature":
            state_temp = float(parts[1])
        elif parts[0] == "sunlight":
            state_sun = float(parts[1])
        elif in_terrain and parts.size() >= 6:
            new_heights.append(float(parts[2]))
        elif parts[0].is_valid_integer() and parts.size() >= 5:
            new_entities.append({"id": int(parts[0]), "x": float(parts[1]), "y": float(parts[2]), "z": float(parts[3]), "r": float(parts[4])})

    terrain_heights = new_heights
    entities = new_entities
    return terrain_heights.size() == state_width * state_depth

func _rebuild_terrain():
    if terrain_heights.size() != state_width * state_depth:
        return

    if terrain_mesh:
        terrain_mesh.queue_free()
        terrain_mesh = null
    if terrain_collision:
        terrain_collision.queue_free()
        terrain_collision = null
    if water_node:
        water_node.queue_free()
        water_node = null

    var mesh = ArrayMesh.new()
    var vertices = PoolVector3Array()
    var normals = PoolVector3Array()
    var indices = PoolIntArray()

    for z in range(state_depth):
        for x in range(state_width):
            vertices.append(Vector3(x, terrain_heights[z * state_width + x], z))
            normals.append(Vector3.UP)

    for z in range(state_depth - 1):
        for x in range(state_width - 1):
            var i = z * state_width + x
            indices.append(i)
            indices.append(i + state_width)
            indices.append(i + 1)
            indices.append(i + 1)
            indices.append(i + state_width)
            indices.append(i + state_width + 1)

    var arrays = []
    arrays.resize(Mesh.ARRAY_MAX)
    arrays[Mesh.ARRAY_VERTEX] = vertices
    arrays[Mesh.ARRAY_NORMAL] = normals
    arrays[Mesh.ARRAY_INDEX] = indices
    mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)

    var material = SpatialMaterial.new()
    material.albedo_color = Color(0.24, 0.42, 0.23)
    material.roughness = 1.0
    mesh.surface_set_material(0, material)

    terrain_mesh = MeshInstance.new()
    terrain_mesh.name = "VEarthTerrain"
    terrain_mesh.mesh = mesh
    add_child(terrain_mesh)

    var static_body = StaticBody.new()
    static_body.name = "VEarthCollision"
    var collision = CollisionShape.new()
    collision.shape = mesh.create_trimesh_shape()
    static_body.add_child(collision)
    add_child(static_body)
    terrain_collision = static_body

    var water_mesh = PlaneMesh.new()
    water_mesh.size = Vector2(state_width - 1, state_depth - 1)
    water_mesh.material = _water_material()
    water_node = MeshInstance.new()
    water_node.name = "SeaLevel"
    water_node.mesh = water_mesh
    water_node.translation = Vector3((state_width - 1) * 0.5, state_sea, (state_depth - 1) * 0.5)
    add_child(water_node)

    terrain_dirty = false

func _water_material():
    var mat = SpatialMaterial.new()
    mat.albedo_color = Color(0.08, 0.23, 0.40, 0.68)
    mat.flags_transparent = true
    mat.roughness = 0.2
    mat.metallic = 0.05
    return mat

func _create_player():
    player = KinematicBody.new()
    player.name = "HumanTester"
    player.set_script(PlayerScript)
    player.translation = Vector3((state_width - 1) * 0.5, _spawn_height() + 0.05, (state_depth - 1) * 0.5)

    var shape = CollisionShape.new()
    var capsule = CapsuleShape.new()
    capsule.radius = 0.35
    capsule.height = 1.75
    shape.shape = capsule
    shape.translation = Vector3(0, 0.875, 0)
    player.add_child(shape)

    var camera = Camera.new()
    camera.name = "Camera"
    camera.translation = Vector3(0, 1.58, 0)
    camera.current = true
    camera.near = 0.05
    camera.far = 500.0
    player.add_child(camera)
    add_child(player)

func _reset_player_if_needed():
    if player:
        player.translation = Vector3((state_width - 1) * 0.5, _spawn_height() + 0.05, (state_depth - 1) * 0.5)

func _spawn_height():
    var x = int((state_width - 1) * 0.5)
    var z = int((state_depth - 1) * 0.5)
    return terrain_heights[z * state_width + x]

func _sync_entities():
    var seen = {}
    for item in entities:
        var id = item.id
        seen[id] = true
        var visual = entity_nodes.get(id, null)
        if visual == null or not is_instance_valid(visual):
            var mesh = SphereMesh.new()
            mesh.radius = item.r
            mesh.height = item.r * 2.0
            var mat = SpatialMaterial.new()
            mat.albedo_color = Color(0.85, 0.68, 0.25)
            mesh.material = mat
            visual = MeshInstance.new()
            visual.name = "Entity-%d" % id
            visual.mesh = mesh
            add_child(visual)
            entity_nodes[id] = visual
        visual.translation = Vector3(item.x, item.y, item.z)

    for id in entity_nodes.keys():
        if not seen.has(id):
            entity_nodes[id].queue_free()
            entity_nodes.erase(id)

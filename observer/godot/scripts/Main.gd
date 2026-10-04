extends Spatial

const STATE_PATH = "res://observer/bridge/state.txt"
const TERRAIN_PATH = "res://observer/bridge/terrain.txt"
const PlayerScript = preload("res://observer/godot/scripts/Player.gd")

var state_time = 0.0
var state_tick = 0
var state_width = 1
var state_depth = 1
var state_sea = 8.0
var state_temp = 288.15
var state_sun = 1.0
var state_world_revision = -1
var state_terrain_revision = -1
var calendar = [0, 0, 0, 0, 0, 0, 0]
var terrain_heights = []
var entities = []
var resources = []
var terrain_mesh = null
var terrain_collision = null
var water_node = null
var player = null
var status_label = null
var banner_label = null
var crosshair_label = null
var resource_nodes = {}
var entity_nodes = {}
var terrain_dirty = false

func _ready():
	_create_environment()
	_create_hud()
	_refresh_state()
	_load_terrain_cache()
	_rebuild_terrain()
	_create_player()
	_sync_entities()
	_sync_resources()
	set_process(true)

func _process(_delta):
	if _refresh_state():
		if terrain_dirty:
			_load_terrain_cache()
			_rebuild_terrain()
			_reset_player_if_needed()
		_sync_entities()
		_sync_resources()
	if player:
		status_label.text = "ORIGIN  |  t=%0.2fs  tick=%d  Y=%0.2f\nCalendar  Y%d M%d D%d  %02d:%02d:%02d  |  FPS: %d\nWASD move  Space jump  Esc mouse"
			% [state_time, state_tick, player.global_transform.origin.y,
			   calendar[0], calendar[1] + 1, calendar[2] + 1, calendar[4], calendar[5], calendar[6],
			   Engine.get_frames_per_second()]

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
	canvas.add_child(status_label)

	banner_label = Label.new()
	banner_label.rect_position = Vector2(18, 112)
	banner_label.add_color_override("font_color", Color(0.65, 0.78, 0.92))
	banner_label.text = "3D human test client  |  C++ simulation is authoritative"
	canvas.add_child(banner_label)

	crosshair_label = Label.new()
	crosshair_label.text = "+"
	crosshair_label.rect_position = Vector2(547, 338)
	crosshair_label.add_color_override("font_color", Color(1.0, 1.0, 1.0, 0.85))
	canvas.add_child(crosshair_label)

func _refresh_state():
	var file = File.new()
	if not file.file_exists(STATE_PATH):
		banner_label.text = "Waiting for C++ bridge: observer/bridge/state.txt"
		return false
	if file.open(STATE_PATH, File.READ) != OK:
		return false

	var lines = file.get_as_text().split("\n")
	file.close()

	var new_entities = []
	var new_resources = []
	var i = 0
	while i < lines.size():
		var trimmed = lines[i].strip_edges()
		var parts = trimmed.split(" ", false)
		if parts.size() == 0:
			i += 1
			continue

		match parts[0]:
			"ORIGIN_STATE":
				pass
			"time":
				if parts.size() > 1: state_time = float(parts[1])
			"tick":
				if parts.size() > 1: state_tick = int(parts[1])
			"width":
				if parts.size() > 1: state_width = max(2, int(parts[1]))
			"depth":
				if parts.size() > 1: state_depth = max(2, int(parts[1]))
			"sea_level":
				if parts.size() > 1: state_sea = float(parts[1])
			"world_revision":
				if parts.size() > 1: state_world_revision = int(parts[1])
			"terrain_revision":
				if parts.size() > 1:
					var incoming_revision = int(parts[1])
					terrain_dirty = incoming_revision != state_terrain_revision
					state_terrain_revision = incoming_revision
			"temperature":
				if parts.size() > 1: state_temp = float(parts[1])
			"sunlight":
				if parts.size() > 1: state_sun = float(parts[1])
			"calendar":
				calendar.clear()
				for j in range(1, min(parts.size(), 8)):
					calendar.append(int(parts[j]))
				while calendar.size() < 7: calendar.append(0)
			"resources":
				var count = parts.size() > 1 ? max(0, int(parts[1])) : 0
				for j in range(1, count + 1):
					if i + j >= lines.size(): break
					var rp = lines[i + j].strip_edges().split(" ", false)
					if rp.size() >= 7:
						new_resources.append({
							"id": int(rp[0]), "kind": int(rp[1]),
							"x": float(rp[2]), "y": float(rp[3]), "z": float(rp[4]),
							"remaining": float(rp[5]), "max": float(rp[6])
						})
				i += count
			"entities":
				var entity_count = parts.size() > 1 ? max(0, int(parts[1])) : 0
				for j in range(1, entity_count + 1):
					if i + j >= lines.size(): break
					var ep = lines[i + j].strip_edges().split(" ", false)
					if ep.size() >= 5:
						new_entities.append({"id": int(ep[0]), "x": float(ep[1]), "y": float(ep[2]), "z": float(ep[3]), "r": float(ep[4])})
				i += entity_count
			"END":
				break
		i += 1

	entities = new_entities
	resources = new_resources
	return true

func _load_terrain_cache():
	var file = File.new()
	if not file.file_exists(TERRAIN_PATH):
		banner_label.text = "Waiting for C++ terrain cache: observer/bridge/terrain.txt"
		terrain_heights = []
		return false
	if file.open(TERRAIN_PATH, File.READ) != OK:
		return false
	var lines = file.get_as_text().split("\n")
	file.close()

	var heights = []
	var local_width = 0
	var local_depth = 0
	for line in lines:
		var trimmed = line.strip_edges()
		var parts = trimmed.split(" ", false)
		if parts.size() == 0: continue
		if parts[0] == "width" and parts.size() > 1:
			local_width = int(parts[1])
		elif parts[0] == "depth" and parts.size() > 1:
			local_depth = int(parts[1])
		elif parts[0].is_valid_integer() and parts.size() >= 4:
			heights.append(float(parts[2]))

	if local_width != state_width or local_depth != state_depth or heights.size() != state_width * state_depth:
		return false
	terrain_heights = heights
	return true

func _rebuild_terrain():
	if terrain_heights.size() != state_width * state_depth:
		return

	if terrain_mesh: terrain_mesh.queue_free()
	if terrain_collision: terrain_collision.queue_free()
	if water_node: water_node.queue_free()

	var mesh = ArrayMesh.new()
	var vertices = PoolVector3Array()
	var normals = PoolVector3Array()
	var indices = PoolIntArray()

	for z in range(state_depth):
		for x in range(state_width):
			var h = terrain_heights[z * state_width + x]
			vertices.append(Vector3(x, h, z))
			var left = terrain_heights[z * state_width + max(0, x - 1)]
			var right = terrain_heights[z * state_width + min(state_width - 1, x + 1)]
			var back = terrain_heights[max(0, z - 1) * state_width + x]
			var front = terrain_heights[min(state_depth - 1, z + 1) * state_width + x]
			normals.append(Vector3(-(right - left) * 0.5, 1.0, -(front - back) * 0.5).normalized())

	for z in range(state_depth - 1):
		for x in range(state_width - 1):
			var a = z * state_width + x
			var b = a + state_width
			var c = a + 1
			var d = b + 1
			indices.append(a); indices.append(b); indices.append(c)
			indices.append(c); indices.append(b); indices.append(d)

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
		player.velocity = Vector3.ZERO

func _spawn_height():
	if terrain_heights.size() != state_width * state_depth: return 8.0
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

func _sync_resources():
	var seen = {}
	for item in resources:
		var id = item.id
		if item.remaining <= 0.0:
			continue
		seen[id] = true
		var visual = resource_nodes.get(id, null)
		if visual == null or not is_instance_valid(visual):
			var mesh = CubeMesh.new()
			mesh.size = Vector3(0.45, 0.55, 0.45)
			var mat = SpatialMaterial.new()
			mat.albedo_color = _resource_color(item.kind)
			mat.roughness = 0.9
			mesh.material = mat
			visual = MeshInstance.new()
			visual.name = "Resource-%d" % id
			visual.mesh = mesh
			add_child(visual)
			resource_nodes[id] = visual
		visual.translation = Vector3(item.x, item.y + 0.27, item.z)
		var ratio = item.max > 0.0 ? clamp(item.remaining / item.max, 0.2, 1.0) : 1.0
		visual.scale = Vector3(ratio, ratio, ratio)

	for id in resource_nodes.keys():
		if not seen.has(id):
			resource_nodes[id].queue_free()
			resource_nodes.erase(id)

func _resource_color(kind):
	match kind:
		0: return Color(0.46, 0.46, 0.48)
		1: return Color(0.50, 0.30, 0.12)
		2: return Color(0.18, 0.42, 0.80)
		3: return Color(0.55, 0.38, 0.20)
	return Color(0.8, 0.8, 0.8)

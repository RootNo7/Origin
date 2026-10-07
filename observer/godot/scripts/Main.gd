extends Spatial


const RUNTIME_PATH = "res://observer/godot/scripts/OriginRuntime.gd"
const PLAYER_PATH = "res://observer/godot/scripts/Player.gd"

var runtime = null
var player_script = null
var terrain_mesh = null
var terrain_collision = null
var water_node = null
var player = null
var status_label = null
var banner_label = null
var crosshair_label = null
var dev_panel = null
var dev_output = null
var resource_nodes = {}
var entity_nodes = {}
var last_terrain_revision = -1
var state_timer = 0.0
var result_flash_timer = 0.0
var last_action_text = ""
var last_saved_persistent_revision = -1
var autosave_timer = 0.0
var screenshot_path = "user://origin/captures/latest.png"
var pending_gather = false
var world_environment_node = null
var sun_node = null
var startup_failed = false
var decoration_root = null
var texture_cache = {}
var water_update_timer = 0.0
var water_material_node = null
var sky_node = null
var sky_update_timer = 0.0
var terrain_material_node = null
var sand_material_node = null
var cliff_material_node = null
var shared_rock_material = null
var shared_wood_material = null
var shared_leaf_material = null
var shared_soil_material = null

func _ready():
	var runtime_script = load(RUNTIME_PATH)
	if runtime_script == null:
		_show_startup_error("OriginRuntime.gd could not be loaded", "The simulation script has a parse/compile error. Check the first debugger error for OriginRuntime.gd.")
		return
	player_script = load(PLAYER_PATH)
	if player_script == null:
		_show_startup_error("Player.gd could not be loaded", "The first-person controller script has a parse/compile error. Check the first debugger error for Player.gd.")
		return
	runtime = runtime_script.new()
	if not runtime.initialize():
		_show_startup_error("Simulation initialization failed", "OriginRuntime.initialize() did not complete.")
		return
	var resumed = runtime.load_world()
	_create_environment()
	_sync_environment_visuals()
	_create_hud()
	if resumed:
		banner_label.text = "Loaded persistent Origin world."
	else:
		banner_label.text = "New Origin world generated."
	if not _rebuild_terrain():
		_show_startup_error("Terrain build failed", "The generated world data could not be converted into a renderable terrain mesh.")
		return
	_create_player()
	_sync_world_visuals()
	last_saved_persistent_revision = runtime.persistent_revision
	_update_hud()
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _physics_process(_delta):
	if startup_failed or runtime == null:
		return
	if pending_gather:
		pending_gather = false
		_perform_gather_ray()

func _process(delta):
	if startup_failed or runtime == null:
		return
	runtime.advance_frame(delta)
	_sync_environment_visuals()
	water_update_timer -= delta
	if water_update_timer <= 0.0:
		water_update_timer = 0.12
		_sync_water_visual()
	autosave_timer -= delta
	if autosave_timer <= 0.0:
		autosave_timer = 10.0
		if runtime.persistent_revision != last_saved_persistent_revision:
			if runtime.save_world():
				last_saved_persistent_revision = runtime.persistent_revision
	state_timer -= delta
	result_flash_timer = max(0.0, result_flash_timer - delta)
	if state_timer <= 0.0:
		state_timer = 0.10
		_sync_world_visuals()
	_send_player_pose()
	_update_hud()

func _input(event):
	if startup_failed:
		return
	if event is InputEventMouseButton and event.pressed and event.button_index == BUTTON_LEFT:
		if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
			pending_gather = true
	elif event is InputEventKey and event.pressed and not event.echo:
		if event.scancode == KEY_F1:
			_toggle_dev_panel()
		elif event.scancode == KEY_ESCAPE:
			if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
			else:
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _create_environment():
	var world_env = WorldEnvironment.new()
	world_environment_node = world_env
	var environment = Environment.new()
	environment.background_mode = Environment.BG_SKY
	environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	environment.ambient_light_color = Color(0.42, 0.49, 0.56)
	environment.ambient_light_energy = 0.18
	environment.background_energy = 0.58
	environment.fog_enabled = false
	environment.adjustment_enabled = false

	var sky = ProceduralSky.new()
	sky.texture_size = ProceduralSky.TEXTURE_SIZE_256
	sky.sky_top_color = Color(0.055, 0.12, 0.24)
	sky.sky_horizon_color = Color(0.43, 0.62, 0.78)
	sky.ground_horizon_color = Color(0.22, 0.20, 0.16)
	sky.ground_bottom_color = Color(0.045, 0.050, 0.060)
	sky.sun_color = Color(1.0, 0.74, 0.40)
	sky.sun_angle_min = 0.75
	sky.sun_angle_max = 7.5
	sky.sun_energy = 0.75
	sky.sun_latitude = 42.0
	sky.sun_longitude = -90.0
	sky.sky_energy = 0.75
	sky.ground_energy = 0.40
	environment.background_sky = sky
	sky_node = sky

	world_env.environment = environment
	add_child(world_env)

	sun_node = DirectionalLight.new()
	sun_node.name = "Sun"
	sun_node.shadow_enabled = true
	sun_node.directional_shadow_max_distance = 120.0
	sun_node.shadow_bias = 0.10
	sun_node.light_energy = 0.55
	sun_node.light_color = Color(1.0, 0.93, 0.82)
	add_child(sun_node)

func _sync_environment_visuals():
	if runtime == null:
		return
	var fraction = runtime.solar_day_fraction()
	var angle = fraction * PI * 2.0
	var sun_height = sin(angle - PI * 0.5)
	var daylight = clamp(max(0.0, sun_height), 0.0, 1.0)
	var twilight = clamp(1.0 - abs(sun_height) / 0.28, 0.0, 1.0)
	if sun_node != null:
		var altitude = -6.0 + sun_height * 82.0
		var azimuth = fraction * 360.0 - 90.0
		sun_node.rotation_degrees = Vector3(-altitude, azimuth, 0.0)
		sun_node.light_energy = 0.08 + daylight * 0.52 + twilight * 0.06
		if sun_height < 0.18:
			sun_node.light_color = Color(1.0, 0.46 + daylight * 0.45, 0.28 + daylight * 0.50)
		else:
			sun_node.light_color = Color(1.0, 0.94, 0.84)
	if sky_node != null:
		sky_update_timer -= get_process_delta_time()
		if sky_update_timer <= 0.0:
			sky_update_timer = 0.25
			sky_node.sun_latitude = -18.0 + daylight * 72.0
			sky_node.sun_longitude = fraction * 360.0 - 90.0
			sky_node.sun_energy = 0.02 + daylight * 0.80 + twilight * 0.12
			sky_node.sky_energy = 0.18 + daylight * 0.70 + twilight * 0.10
			sky_node.ground_energy = 0.22 + daylight * 0.34
			if sun_height < -0.18:
				sky_node.sky_top_color = Color(0.005, 0.008, 0.025)
				sky_node.sky_horizon_color = Color(0.018, 0.028, 0.070)
				sky_node.ground_horizon_color = Color(0.018, 0.020, 0.028)
			elif sun_height < 0.18:
				var t = clamp((sun_height + 0.18) / 0.36, 0.0, 1.0)
				sky_node.sky_top_color = Color(0.018 + 0.055 * t, 0.030 + 0.115 * t, 0.060 + 0.20 * t)
				sky_node.sky_horizon_color = Color(0.08 + 0.28 * t, 0.06 + 0.28 * t, 0.09 + 0.38 * t)
				sky_node.ground_horizon_color = Color(0.055 + 0.18 * t, 0.050 + 0.16 * t, 0.045 + 0.13 * t)
			else:
				sky_node.sky_top_color = Color(0.07, 0.16 + daylight * 0.10, 0.34 + daylight * 0.12)
				sky_node.sky_horizon_color = Color(0.43 + daylight * 0.08, 0.62 + daylight * 0.12, 0.77 + daylight * 0.10)
				sky_node.ground_horizon_color = Color(0.24 + daylight * 0.06, 0.22 + daylight * 0.05, 0.18 + daylight * 0.04)

	if world_environment_node != null and world_environment_node.environment != null:
		world_environment_node.environment.ambient_light_energy = 0.065 + daylight * 0.19 + twilight * 0.05
		world_environment_node.environment.background_energy = 0.26 + daylight * 0.45 + twilight * 0.08
		world_environment_node.environment.ambient_light_color = Color(0.30 + daylight * 0.14, 0.36 + daylight * 0.15, 0.44 + daylight * 0.17)

func _create_hud():
	var canvas = CanvasLayer.new()
	canvas.name = "HUD"
	add_child(canvas)

	status_label = Label.new()
	status_label.rect_position = Vector2(18, 16)
	status_label.add_color_override("font_color", Color(0.86, 0.92, 1.0))
	canvas.add_child(status_label)

	banner_label = Label.new()
	banner_label.rect_position = Vector2(18, 150)
	banner_label.add_color_override("font_color", Color(0.65, 0.78, 0.92))
	banner_label.text = "Origin Godot Runtime | F1 Developer Console"
	canvas.add_child(banner_label)

	crosshair_label = Label.new()
	crosshair_label.text = "+"
	crosshair_label.add_color_override("font_color", Color(1.0, 1.0, 1.0, 0.85))
	canvas.add_child(crosshair_label)

	_create_dev_panel(canvas)

func _create_dev_panel(canvas):
	dev_panel = Panel.new()
	dev_panel.rect_position = Vector2(18, 205)
	dev_panel.rect_size = Vector2(360, 440)
	dev_panel.visible = false
	canvas.add_child(dev_panel)

	var title = Label.new()
	title.text = "ORIGIN DEVELOPER / TEST CONSOLE"
	title.rect_position = Vector2(14, 12)
	dev_panel.add_child(title)

	var buttons = [
		["New World", "new_world"],
		["Save World", "save_world"],
		["Load World", "load_world"],
		["Run Runtime Tests", "tests"],
		["Run Full Smoke Test", "smoke"],
		["Benchmark 10,000 ticks", "benchmark"],
		["Capture Screenshot", "screenshot"],
		["Reset HumanTester", "reset_tester"],
		["Pause / Resume", "pause"],
		["Speed x1", "speed_1"],
		["Speed x4", "speed_4"],
		["Speed x16", "speed_16"]
	]
	var y = 44
	for entry in buttons:
		var button = Button.new()
		button.text = entry[0]
		button.rect_position = Vector2(14, y)
		button.rect_size = Vector2(160, 28)
		button.connect("pressed", self, "_dev_command", [entry[1]])
		dev_panel.add_child(button)
		y += 32

	dev_output = Label.new()
	dev_output.rect_position = Vector2(188, 44)
	dev_output.rect_size = Vector2(156, 365)
	dev_output.autowrap = true
	dev_output.text = "Console ready.\n\nEverything runs inside Godot.\nNo C++ build is required."
	dev_panel.add_child(dev_output)

func _dev_command(command):
	match command:
		"new_world":
			runtime.initialize(runtime.world_seed)
			_rebuild_terrain()
			_reset_player_if_needed()
			_sync_world_visuals()
			_console("New world generated.")
		"save_world":
			var saved = runtime.save_world()
			if saved:
				last_saved_persistent_revision = runtime.persistent_revision
			_console("Save: %s" % saved)
		"load_world":
			var ok = runtime.load_world()
			if ok:
				_rebuild_terrain()
				_reset_player_if_needed()
				_sync_world_visuals()
				last_saved_persistent_revision = runtime.persistent_revision
			_console("Load: %s" % ok)
		"tests":
			_console(runtime.format_self_test_report(runtime.run_self_tests()))
		"smoke":
			_console(run_full_smoke_test())
		"benchmark":
			_console(str(runtime.benchmark(10000)))
		"reset_tester":
			runtime.reset_human_test_actor()
			_reset_player_if_needed()
			_console("HumanTester reset to the saved/world start position.")
		"screenshot":
			_capture_screenshot()
		"pause":
			_console("Paused: %s" % runtime.toggle_pause())
		"speed_1":
			runtime.set_speed(1.0)
			_console("Speed x1")
		"speed_4":
			runtime.set_speed(4.0)
			_console("Speed x4")
		"speed_16":
			runtime.set_speed(16.0)
			_console("Speed x16")

func run_full_smoke_test():
	var runtime_result = runtime.run_self_tests()
	var checks = [
		["runtime self-tests", bool(runtime_result.get("passed", false))],
		["first-person player exists", player != null],
		["first-person camera exists", player != null and player.has_node("Camera")],
		["terrain mesh exists", terrain_mesh != null and terrain_mesh.mesh != null],
		["terrain collision exists", terrain_collision != null],
		["water surface exists", water_node != null],
		["resource visuals exist", resource_nodes.size() > 0]
	]
	var passed = 0
	var output = "FULL SMOKE TEST"
	for check in checks:
		var ok = bool(check[1])
		if ok:
			passed += 1
		output += "\n%s %s" % ["OK" if ok else "FAIL", str(check[0])]
	output += "\nRuntime checks: %d/%d" % [int(runtime_result.get("passed_checks", 0)), int(runtime_result.get("checks", []).size())]
	output += "\nScene checks: %d/%d" % [passed, checks.size()]
	output += "\nRESULT: %s" % ["PASS" if bool(runtime_result.get("passed", false)) and passed == checks.size() else "FAIL"]
	return output

func is_developer_panel_visible():
	return dev_panel != null and dev_panel.visible

func _toggle_dev_panel():
	if dev_panel:
		dev_panel.visible = not dev_panel.visible
		if dev_panel.visible:
			Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		else:
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _console(message):
	if dev_output:
		dev_output.text = str(message)
	banner_label.text = str(message)

func _capture_screenshot():
	var dir = Directory.new()
	dir.make_dir_recursive("user://origin/captures")
	var image = get_viewport().get_texture().get_data()
	image.flip_y()
	var err = image.save_png(screenshot_path)
	_console("Screenshot: %s -> %s" % [screenshot_path, err == OK])

func _rebuild_terrain():
	if runtime == null or runtime.heights.size() != runtime.width * runtime.depth:
		return false
	if terrain_mesh != null and is_instance_valid(terrain_mesh):
		terrain_mesh.queue_free()
	if terrain_collision != null and is_instance_valid(terrain_collision):
		terrain_collision.queue_free()
	if water_node != null and is_instance_valid(water_node):
		water_node.queue_free()
	if decoration_root != null and is_instance_valid(decoration_root):
		decoration_root.queue_free()
	resource_nodes.clear()
	entity_nodes.clear()

	var mesh = ArrayMesh.new()
	var vertices = PoolVector3Array()
	var normals = PoolVector3Array()
	var uvs = PoolVector2Array()
	var terrain_indices = PoolIntArray()
	var sand_indices = PoolIntArray()
	var cliff_indices = PoolIntArray()
	for z in range(runtime.depth):
		for x in range(runtime.width):
			var h = float(runtime.heights[z * runtime.width + x])
			vertices.append(Vector3(float(x), h, float(z)))
			var left = float(runtime.heights[z * runtime.width + max(0, x - 1)])
			var right = float(runtime.heights[z * runtime.width + min(runtime.width - 1, x + 1)])
			var back = float(runtime.heights[max(0, z - 1) * runtime.width + x])
			var front = float(runtime.heights[min(runtime.depth - 1, z + 1) * runtime.width + x])
			var dx = (right - left) * 0.5
			var dz = (front - back) * 0.5
			# For UVs of (x,z), the correct world-surface normal is (-dz, 1, -dx).
			# The old renderer swapped dx/dz here, producing the striped/white lighting artifact.
			normals.append(Vector3(-dz, 1.0, -dx).normalized())
			uvs.append(Vector2(float(x) / max(1.0, float(runtime.width - 1)), float(z) / max(1.0, float(runtime.depth - 1))))
	for z in range(runtime.depth - 1):
		for x in range(runtime.width - 1):
			var a = z * runtime.width + x
			var b = a + runtime.width
			var c = a + 1
			var d = b + 1
			var h_avg = (float(runtime.heights[a]) + float(runtime.heights[b]) + float(runtime.heights[c]) + float(runtime.heights[d])) * 0.25
			var slope_avg = (runtime.surface_slope(float(x), float(z)) + runtime.surface_slope(float(x + 1), float(z + 1))) * 0.5
			var target = terrain_indices
			if h_avg < runtime.sea_level + 0.78 and h_avg > runtime.sea_level - 1.75:
				target = sand_indices
			elif slope_avg > 0.54 or h_avg > 18.0:
				target = cliff_indices
			target.append(a)
			target.append(b)
			target.append(c)
			target.append(c)
			target.append(b)
			target.append(d)

	_add_terrain_surface(mesh, vertices, normals, uvs, terrain_indices, _terrain_material())
	if sand_indices.size() > 0:
		_add_terrain_surface(mesh, vertices, normals, uvs, sand_indices, _sand_material())
	if cliff_indices.size() > 0:
		_add_terrain_surface(mesh, vertices, normals, uvs, cliff_indices, _cliff_material())

	terrain_mesh = MeshInstance.new()
	terrain_mesh.name = "Terrain"
	terrain_mesh.mesh = mesh
	add_child(terrain_mesh)

	var shape = mesh.create_trimesh_shape()
	if shape != null:
		var static_body = StaticBody.new()
		static_body.name = "TerrainCollision"
		var collision = CollisionShape.new()
		collision.shape = shape
		static_body.add_child(collision)
		add_child(static_body)
		terrain_collision = static_body

	_create_water_surface()
	_create_decorations()
	last_terrain_revision = runtime.terrain_revision
	return true

func _add_terrain_surface(mesh, vertices, normals, uvs, indices, material):
	if indices.size() == 0:
		return
	var arrays = []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = vertices
	arrays[Mesh.ARRAY_NORMAL] = normals
	arrays[Mesh.ARRAY_TEX_UV] = uvs
	arrays[Mesh.ARRAY_INDEX] = indices
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	mesh.surface_set_material(mesh.get_surface_count() - 1, material)

func _terrain_material():
	if terrain_material_node != null:
		return terrain_material_node
	terrain_material_node = SpatialMaterial.new()
	terrain_material_node.albedo_color = Color(0.86, 0.94, 0.82)
	terrain_material_node.albedo_texture = _texture("terrain_albedo.png")
	terrain_material_node.roughness = 0.94
	terrain_material_node.metallic = 0.0
	terrain_material_node.specular = 0.08
	terrain_material_node.uv1_scale = Vector3(20.0, 20.0, 1.0)
	return terrain_material_node

func _sand_material():
	if sand_material_node != null:
		return sand_material_node
	sand_material_node = SpatialMaterial.new()
	sand_material_node.albedo_color = Color(0.92, 0.88, 0.72)
	sand_material_node.albedo_texture = _texture("sand_albedo.png")
	sand_material_node.roughness = 0.97
	sand_material_node.specular = 0.05
	sand_material_node.uv1_scale = Vector3(16.0, 16.0, 1.0)
	return sand_material_node

func _cliff_material():
	if cliff_material_node != null:
		return cliff_material_node
	cliff_material_node = SpatialMaterial.new()
	cliff_material_node.albedo_color = Color(0.82, 0.82, 0.80)
	cliff_material_node.albedo_texture = _texture("rock_albedo.png")
	cliff_material_node.roughness = 0.96
	cliff_material_node.specular = 0.06
	cliff_material_node.uv1_scale = Vector3(11.0, 11.0, 1.0)
	return cliff_material_node

func _create_water_surface():
	var water_mesh = PlaneMesh.new()
	water_mesh.size = Vector2(runtime.width - 1, runtime.depth - 1)
	water_material_node = _water_material()
	water_mesh.material = water_material_node
	water_node = MeshInstance.new()
	water_node.name = "Water"
	water_node.mesh = water_mesh
	water_node.translation = Vector3((runtime.width - 1) * 0.5, runtime.sea_level, (runtime.depth - 1) * 0.5)
	add_child(water_node)

func _sync_water_visual():
	if water_node == null or not is_instance_valid(water_node):
		return
	var t = fmod(runtime.seconds * 0.75, PI * 2.0)
	water_node.translation.y = runtime.sea_level + sin(t) * 0.018
	if water_material_node != null:
		var fraction = runtime.solar_day_fraction()
		var sun_height = sin(fraction * PI * 2.0 - PI * 0.5)
		var daylight = clamp(max(0.0, sun_height), 0.0, 1.0)
		water_material_node.albedo_color = Color(0.34 + daylight * 0.22, 0.58 + daylight * 0.24, 0.64 + daylight * 0.26)
		var uv_phase = fmod(runtime.seconds * 0.010, 1.0)
		water_material_node.uv1_offset = Vector3(uv_phase, fmod(runtime.seconds * 0.006, 1.0), 0.0)

func _water_material():
	var mat = SpatialMaterial.new()
	mat.albedo_color = Color(0.48, 0.70, 0.76)
	mat.albedo_texture = _texture("water_albedo.png")
	mat.roughness = 0.25
	mat.metallic = 0.0
	mat.specular = 0.32
	mat.uv1_scale = Vector3(14.0, 14.0, 1.0)
	return mat

func _texture(file_name):
	if texture_cache.has(file_name):
		return texture_cache[file_name]
	var path = "res://observer/godot/assets/textures/" + str(file_name)
	var texture = load(path)
	if texture == null:
		push_error("Origin texture failed to load: " + path)
	texture_cache[file_name] = texture
	return texture

func _create_decorations():
	decoration_root = Spatial.new()
	decoration_root.name = "Decorations"
	add_child(decoration_root)
	var step = 6
	for z in range(3, runtime.depth - 3, step):
		for x in range(3, runtime.width - 3, step):
			var h = runtime.ground_height(float(x), float(z))
			if h <= runtime.sea_level + 0.9:
				continue
			if runtime.surface_slope(float(x), float(z)) > 0.48:
				continue
			var chance = _visual_hash(x, z)
			if chance > 0.62:
				_add_tree(Vector3(float(x), h, float(z)), 0.85 + _visual_hash(x + 11, z + 7) * 0.45)
			elif chance > 0.52:
				_add_small_rock(Vector3(float(x), h, float(z)), 0.6 + _visual_hash(x - 9, z + 3) * 0.7)

func _visual_hash(x, z):
	var value = sin(float(x) * 12.9898 + float(z) * 78.233 + float(runtime.world_seed) * 37.719) * 43758.5453
	return abs(value - floor(value))

func _add_tree(position, scale_value):
	var root = Spatial.new()
	root.translation = position
	root.scale = Vector3.ONE * scale_value
	var trunk_mesh = CylinderMesh.new()
	trunk_mesh.top_radius = 0.14
	trunk_mesh.bottom_radius = 0.22
	trunk_mesh.height = 2.6
	trunk_mesh.radial_segments = 8
	trunk_mesh.rings = 2
	trunk_mesh.material = _wood_material()
	var trunk = MeshInstance.new()
	trunk.mesh = trunk_mesh
	trunk.translation.y = 1.3
	root.add_child(trunk)
	var crown_mesh = SphereMesh.new()
	crown_mesh.radius = 1.0
	crown_mesh.height = 1.65
	crown_mesh.radial_segments = 12
	crown_mesh.rings = 6
	crown_mesh.material = _leaf_material()
	var crown = MeshInstance.new()
	crown.mesh = crown_mesh
	crown.scale = Vector3(1.45, 1.05, 1.35)
	crown.translation = Vector3(0, 2.8, 0)
	root.add_child(crown)
	var crown2 = MeshInstance.new()
	crown2.mesh = crown_mesh
	crown2.scale = Vector3(1.0, 0.9, 1.0)
	crown2.translation = Vector3(0.38, 2.15, 0.05)
	root.add_child(crown2)
	decoration_root.add_child(root)

func _add_small_rock(position, scale_value):
	var mesh = SphereMesh.new()
	mesh.radius = 0.55
	mesh.height = 0.70
	mesh.radial_segments = 10
	mesh.rings = 5
	mesh.material = _rock_material()
	var visual = MeshInstance.new()
	visual.mesh = mesh
	visual.translation = position + Vector3(0, 0.25, 0)
	visual.scale = Vector3(scale_value, scale_value * 0.72, scale_value * 0.85)
	visual.rotation_degrees = Vector3(0, _visual_hash(int(position.x) + 4, int(position.z) + 2) * 360.0, 0)
	decoration_root.add_child(visual)

func _rock_material():
	if shared_rock_material != null:
		return shared_rock_material
	shared_rock_material = SpatialMaterial.new()
	shared_rock_material.albedo_color = Color(1.0, 1.0, 1.0)
	shared_rock_material.albedo_texture = _texture("rock_albedo.png")
	shared_rock_material.roughness = 0.86
	shared_rock_material.specular = 0.24
	shared_rock_material.normal_enabled = true
	shared_rock_material.normal_texture = _texture("rock_normal.png")
	shared_rock_material.normal_scale = 0.20
	return shared_rock_material

func _wood_material():
	if shared_wood_material != null:
		return shared_wood_material
	shared_wood_material = SpatialMaterial.new()
	shared_wood_material.albedo_color = Color(1.0, 1.0, 1.0)
	shared_wood_material.albedo_texture = _texture("wood_albedo.png")
	shared_wood_material.roughness = 0.91
	shared_wood_material.specular = 0.16
	shared_wood_material.normal_enabled = true
	shared_wood_material.normal_texture = _texture("wood_normal.png")
	shared_wood_material.normal_scale = 0.18
	return shared_wood_material

func _leaf_material():
	if shared_leaf_material != null:
		return shared_leaf_material
	shared_leaf_material = SpatialMaterial.new()
	shared_leaf_material.albedo_color = Color(1.0, 1.0, 1.0)
	shared_leaf_material.albedo_texture = _texture("leaf_albedo.png")
	shared_leaf_material.roughness = 0.94
	shared_leaf_material.specular = 0.08
	return shared_leaf_material

func _create_player():
	player = KinematicBody.new()
	player.name = "HumanTester"
	player.set_script(player_script)
	var actor = runtime.get_entity(runtime.human_test_actor_id)
	if actor != null:
		player.translation = Vector3(float(actor.x), float(actor.y) - 0.35, float(actor.z))
	else:
		var x = (runtime.width - 1) * 0.5
		var z = (runtime.depth - 1) * 0.5
		player.translation = Vector3(x, runtime.ground_height(x, z) + 0.02, z)

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
	camera.far = max(500.0, float(max(runtime.width, runtime.depth)) * 1.5)
	player.add_child(camera)
	add_child(player)

func _reset_player_if_needed():
	if player == null:
		return
	var actor = runtime.get_entity(runtime.human_test_actor_id)
	if actor != null:
		player.translation = Vector3(float(actor.x), float(actor.y) - 0.35, float(actor.z))
	else:
		var x = (runtime.width - 1) * 0.5
		var z = (runtime.depth - 1) * 0.5
		player.translation = Vector3(x, runtime.ground_height(x, z) + 0.02, z)
	var camera = player.get_node("Camera")
	if camera != null:
		camera.far = max(500.0, float(max(runtime.width, runtime.depth)) * 1.5)
	player.velocity = Vector3.ZERO

func _send_player_pose():
	if player == null:
		return
	runtime.set_human_test_pose(player.global_transform.origin + Vector3(0, 0.35, 0))

func _perform_gather_ray():
	if player == null or runtime.human_test_actor_id <= 0:
		return
	var camera = player.get_node("Camera")
	var origin = camera.global_transform.origin
	var direction = -camera.global_transform.basis.z.normalized()
	var space = get_world().direct_space_state
	var hit = space.intersect_ray(origin, origin + direction * 200.0, [player], 0x7fffffff, true, true)
	if hit.empty():
		_console("Nothing targeted.")
		return
	var collider = hit.get("collider", null)
	if collider == null or not collider.has_meta("resource_id"):
		_console("That is not a gatherable resource.")
		return
	var resource_id = int(collider.get_meta("resource_id"))
	var result = runtime.apply_action({"type": "gather_resource", "actor_id": runtime.human_test_actor_id, "target_id": resource_id, "amount": 1.0}, "developer")
	last_action_text = str(result)
	result_flash_timer = 1.5
	_console("Gather: %s" % result.reason)

func is_player_in_water(position):
	if runtime == null:
		return false
	return runtime.get_water_state_for_player(position)

func _sync_world_visuals():
	if last_terrain_revision != runtime.terrain_revision:
		_rebuild_terrain()
	_sync_entities()
	_sync_resources()

func _sync_entities():
	var seen = {}
	for item in runtime.entities:
		var id = int(item.id)
		if id == runtime.human_test_actor_id or not item.alive:
			continue
		seen[id] = true
		var visual = entity_nodes.get(id, null)
		if visual == null or not is_instance_valid(visual):
			var mesh = SphereMesh.new()
			mesh.radius = max(0.08, float(item.radius))
			mesh.height = max(0.16, float(item.radius) * 2.0)
			mesh.material = _rock_material()
			visual = MeshInstance.new()
			visual.name = "Entity-%d" % id
			visual.mesh = mesh
			add_child(visual)
			entity_nodes[id] = visual
		visual.translation = Vector3(item.x, item.y, item.z)
	var stale = []
	for id in entity_nodes.keys():
		if not seen.has(id): stale.append(id)
	for id in stale:
		if is_instance_valid(entity_nodes[id]):
			entity_nodes[id].queue_free()
		entity_nodes.erase(id)

func _sync_resources():
	var seen = {}
	for item in runtime.resources:
		var id = int(item.id)
		if int(item.kind) == 2:
			continue
		if float(item.remaining) <= 0.0:
			continue
		seen[id] = true
		var visual = resource_nodes.get(id, null)
		if visual == null or not is_instance_valid(visual):
			visual = _create_resource_visual(item)
			resource_nodes[id] = visual
		visual.translation = Vector3(item.x, float(item.y), item.z)
		var ratio = clamp(float(item.remaining) / max(0.001, float(item.max)), 0.22, 1.0)
		visual.scale = Vector3.ONE * ratio
	var stale = []
	for id in resource_nodes.keys():
		if not seen.has(id):
			stale.append(id)
	for id in stale:
		if is_instance_valid(resource_nodes[id]):
			resource_nodes[id].queue_free()
		resource_nodes.erase(id)

func _create_resource_visual(item):
	var root = Spatial.new()
	root.name = "Resource-%d" % int(item.id)
	var kind = int(item.kind)
	var visual = MeshInstance.new()
	var collider = StaticBody.new()
	collider.name = "ResourceCollider-%d" % int(item.id)
	collider.set_meta("resource_id", int(item.id))
	var collision_shape = CollisionShape.new()

	if kind == 0:
		var mesh = SphereMesh.new()
		mesh.radius = 0.48
		mesh.height = 0.72
		mesh.radial_segments = 12
		mesh.rings = 6
		mesh.material = _rock_material()
		visual.mesh = mesh
		visual.translation.y = 0.22
		var sphere = SphereShape.new()
		sphere.radius = 0.40
		collision_shape.shape = sphere
		collision_shape.translation.y = 0.22
	elif kind == 1:
		var mesh = CylinderMesh.new()
		mesh.top_radius = 0.18
		mesh.bottom_radius = 0.23
		mesh.height = 0.95
		mesh.radial_segments = 10
		mesh.material = _wood_material()
		visual.mesh = mesh
		root.rotation_degrees = Vector3(90, 15, 0)
		visual.translation.y = 0.25
		var box = BoxShape.new()
		box.extents = Vector3(0.22, 0.48, 0.22)
		collision_shape.shape = box
		collision_shape.translation.y = 0.25
	else:
		var mesh = SphereMesh.new()
		mesh.radius = 0.42
		mesh.height = 0.55
		mesh.radial_segments = 10
		mesh.rings = 5
		if shared_soil_material == null:
			shared_soil_material = SpatialMaterial.new()
			shared_soil_material.albedo_color = Color(1.0, 1.0, 1.0)
			shared_soil_material.albedo_texture = _texture("soil_albedo.png")
			shared_soil_material.roughness = 0.96
			shared_soil_material.normal_enabled = true
			shared_soil_material.normal_texture = _texture("soil_normal.png")
			shared_soil_material.normal_scale = 0.20
		mesh.material = shared_soil_material
		visual.mesh = mesh
		visual.translation.y = 0.18
		var soil_shape = SphereShape.new()
		soil_shape.radius = 0.34
		collision_shape.shape = soil_shape
		collision_shape.translation.y = 0.18

	root.add_child(visual)
	collider.add_child(collision_shape)
	root.add_child(collider)
	add_child(root)
	return root

func _resource_color(kind):
	match int(kind):
		0: return Color(0.46, 0.46, 0.48)
		1: return Color(0.50, 0.30, 0.12)
		2: return Color(0.18, 0.42, 0.80)
		3: return Color(0.55, 0.38, 0.20)
	return Color(0.8, 0.8, 0.8)

func _update_hud():
	if status_label == null:
		return
	var cal = runtime.calendar()
	var inv = runtime.human_inventory()
	var paused_text = "PAUSED" if runtime.paused else "RUNNING"
	var phase = runtime.solar_phase_name()
	var focus = ""
	if result_flash_timer > 0.0:
		focus = "\nLast action: %s" % last_action_text
	var save_state = "CLEAN" if runtime.persistent_revision == last_saved_persistent_revision else "DIRTY/AUTOSAVE"
	status_label.text = "ORIGIN 0.8.0 | %s | t=%0.2fs tick=%d  Y=%0.2f\nCalendar Y%d M%d D%d  %02d:%02d:%02d\nTemp %0.1fK  Sun %0.2f  %s  Speed x%0.1f  Save %s\nStone %.1f  Wood %.1f  Water %.1f  Soil %.1f\nWASD move  Space jump  Shift down in water  Left-click gather  F1 console  Esc mouse | FPS %d%s" % [paused_text, runtime.seconds, runtime.tick, player.global_transform.origin.y, cal[0], cal[1] + 1, cal[2] + 1, cal[4], cal[5], cal[6], runtime.global_temperature, runtime.sunlight, phase, runtime.speed, save_state, inv[0], inv[1], inv[2], inv[3], Engine.get_frames_per_second(), focus]
	crosshair_label.rect_position = get_viewport().size * 0.5 - Vector2(4, 12)

func _show_startup_error(title, message):
	startup_failed = true
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	var canvas = CanvasLayer.new()
	canvas.name = "StartupError"
	add_child(canvas)

	var panel = ColorRect.new()
	panel.rect_position = Vector2(36, 36)
	panel.rect_size = Vector2(720, 250)
	panel.color = Color(0.035, 0.045, 0.065, 1.0)
	canvas.add_child(panel)

	var heading = Label.new()
	heading.rect_position = Vector2(24, 24)
	heading.rect_size = Vector2(672, 44)
	heading.add_color_override("font_color", Color(1.0, 0.45, 0.35))
	heading.text = "ORIGIN STARTUP ERROR\n" + str(title)
	panel.add_child(heading)

	var details = Label.new()
	details.rect_position = Vector2(24, 94)
	details.rect_size = Vector2(672, 100)
	details.autowrap = true
	details.text = str(message) + "\n\nOrigin stopped before creating the world so this error is visible instead of a grey/empty screen."
	panel.add_child(details)

	var hint = Label.new()
	hint.rect_position = Vector2(24, 204)
	hint.text = "F8 = stop   |   Fix the first parser error, then press F6/F5 again"
	panel.add_child(hint)

func _notification(what):
	if what == NOTIFICATION_WM_QUIT_REQUEST:
		if runtime != null:
			runtime.save_world()
		get_tree().quit()


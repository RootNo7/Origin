extends Reference

# Origin 0.8.1-dev stable Godot 3.6 runtime

# The runtime is authoritative for the playable Godot build.
# The previous native backend is historical only; the active project contains no native runtime dependency.

const SAVE_FORMAT = 6
const PREVIOUS_SAVE_FORMAT = 5
const LEGACY_SAVE_FORMAT = 4
const MAX_DIMENSION = 512
const MAX_SAVE_BYTES = 128 * 1024 * 1024
const INVENTORY_CAPACITY_PER_KIND = 1000.0
const MAX_EVENT_LOG = 512
const MAX_SAVE_CELLS = MAX_DIMENSION * MAX_DIMENSION
const MAX_RESOURCES = 20000
const MAX_ENTITIES = 4096
const DEFAULT_WIDTH = 96
const DEFAULT_DEPTH = 96
const DEFAULT_SEED = 7
const DEFAULT_DT = 1.0 / 30.0
const DAY_LENGTH_SECONDS = 600.0
const START_DAY_FRACTION = 0.30
const MIN_DT = 0.000001
const MAX_DT = 0.25
const MAX_SPEED = 16.0
const GRAVITY = 9.81
const GATHER_RANGE = 2.25
const MAX_FRAME_STEPS = 32
const MAX_ACCUMULATOR = 1.0

var width = DEFAULT_WIDTH
var depth = DEFAULT_DEPTH
var world_seed = DEFAULT_SEED
var sea_level = 8.0
var heights = []
var temperatures = []
var water_depths = []
var humidity = []
var resources = []
var entities = []
var next_resource_id = 1
var next_entity_id = 1
var world_revision = 0
var persistent_revision = 0
var terrain_revision = 0
var tick = 0
var seconds = 0.0
var fixed_dt = DEFAULT_DT
var speed = 1.0
var paused = false
var human_test_actor_id = 0
var sunlight = 1.0
var global_temperature = 288.15
var accumulator = 0.0
var environment_accumulator = 0.0
var last_action_result = {}
var event_log = []

func initialize(p_seed = DEFAULT_SEED, p_width = DEFAULT_WIDTH, p_depth = DEFAULT_DEPTH):
	width = clamp(int(p_width), 8, MAX_DIMENSION)
	depth = clamp(int(p_depth), 8, MAX_DIMENSION)
	world_seed = int(p_seed)
	sea_level = 8.0
	heights = []
	temperatures = []
	water_depths = []
	humidity = []
	resources = []
	entities = []
	next_resource_id = 1
	next_entity_id = 1
	world_revision = 0
	persistent_revision = 0
	terrain_revision = 0
	tick = 0
	seconds = DAY_LENGTH_SECONDS * START_DAY_FRACTION
	fixed_dt = DEFAULT_DT
	speed = 1.0
	paused = false
	human_test_actor_id = 0
	var initial_sun = sin((START_DAY_FRACTION - 0.25) * PI * 2.0)
	sunlight = clamp(max(0.0, initial_sun), 0.0, 1.0)
	global_temperature = 286.0
	accumulator = 0.0
	environment_accumulator = 0.0
	last_action_result = {}
	event_log = []
	_generate_world()
	_spawn_default_entities()
	_record_event("simulation_started", "VEarth 3D world initialized")
	return true

func _generate_world():
	var cell_count = width * depth
	heights.resize(cell_count)
	temperatures.resize(cell_count)
	water_depths.resize(cell_count)
	humidity.resize(cell_count)
	var cx = float(width - 1) * 0.5
	var cz = float(depth - 1) * 0.5
	var scale = max(16.0, float(max(width, depth)))
	for z in range(depth):
		for x in range(width):
			var fx = float(x)
			var fz = float(z)
			var nx = (fx - cx) / scale
			var nz = (fz - cz) / scale
			var continent = sin(nx * PI * 1.8) * 2.6 + cos(nz * PI * 1.55) * 2.15 + sin((nx - nz) * PI * 3.6) * 1.15
			var hills = sin(fx * 0.072 + fz * 0.041) * 1.35 + cos(fx * 0.043 - fz * 0.091) * 0.95
			var detail = (_hash01(x, z) - 0.5) * 0.65
			var h = clamp(8.1 + continent + hills + detail, 1.0, 28.0)
			var latitude = (fz / max(1.0, float(depth - 1))) * PI
			var t = 287.0 - 5.0 * cos(latitude) + 1.5 * sin(fx * 0.035 + fz * 0.02)
			var idx = z * width + x
			heights[idx] = h
			temperatures[idx] = t
			water_depths[idx] = max(0.0, sea_level - h)
			var coastal = clamp(water_depths[idx] / 4.0, 0.0, 1.0)
			humidity[idx] = clamp(0.45 + coastal * 0.36 + (_hash01(x + 73, z - 31) - 0.5) * 0.10, 0.05, 0.98)
	terrain_revision = 1
	world_revision = 1
	_generate_resources()

func _hash01(x, z):
	var n = int(world_seed) * 374761393 + int(x) * 668265263 + int(z) * 2147483647
	n = int((n ^ int(n >> 13)) * 1274126177)
	n = n ^ int(n >> 16)
	var positive = n & 2147483647
	return float(positive) / 2147483647.0

func _generate_resources():
	resources.clear()
	next_resource_id = 1
	for z in range(4, depth - 4, 9):
		for x in range(4, width - 4, 11):
			var h = ground_height(float(x), float(z))
			var kind = 0
			if h < sea_level:
				kind = 2
			elif int(x + z) % 3 == 0:
				kind = 1
			else:
				kind = 0
			var max_amount = 25.0 if kind == 0 else 12.0
			resources.append({
				"id": next_resource_id,
				"kind": kind,
				"x": float(x),
				"y": h,
				"z": float(z),
				"remaining": max_amount,
				"max": max_amount
			})
			next_resource_id += 1

func _spawn_default_entities():
	var spawn = find_human_spawn()
	human_test_actor_id = _create_entity("HumanTester", 3, spawn, Vector3.ZERO, 70.0, 0.35, 0.0, false)
	var stone_x = clamp(float(width) * 0.28, 1.0, float(width - 2))
	var stone_z = clamp(float(depth) * 0.32, 1.0, float(depth - 2))
	var stone_y = ground_height(stone_x, stone_z) + 1.5
	_create_entity("Stone-1", 0, Vector3(stone_x, stone_y, stone_z), Vector3.ZERO, 40.0, 0.45, 0.05, true)

func find_human_spawn():
	var cx = float(width - 1) * 0.5
	var cz = float(depth - 1) * 0.5
	var max_radius = max(width, depth)
	for radius in range(0, max_radius):
		var samples = max(8, radius * 8)
		for i in range(samples):
			var angle = float(i) / float(samples) * PI * 2.0
			var x = clamp(cx + cos(angle) * float(radius), 1.0, float(width - 2))
			var z = clamp(cz + sin(angle) * float(radius), 1.0, float(depth - 2))
			var h = ground_height(x, z)
			if h > sea_level + 0.8 and surface_slope(x, z) < 0.55:
				return Vector3(x, h + 0.35, z)
	return Vector3(cx, ground_height(cx, cz) + 0.35, cz)

func surface_slope(x, z):
	if heights.empty():
		return 0.0
	var qx = clamp(float(x), 0.0, float(width - 1))
	var qz = clamp(float(z), 0.0, float(depth - 1))
	var left = ground_height(qx - 1.0, qz)
	var right = ground_height(qx + 1.0, qz)
	var back = ground_height(qx, qz - 1.0)
	var front = ground_height(qx, qz + 1.0)
	var dx = (right - left) * 0.5
	var dz = (front - back) * 0.5
	return clamp(sqrt(dx * dx + dz * dz) / max(0.001, sqrt(1.0 + dx * dx + dz * dz)), 0.0, 1.0)

func humidity_at(x, z):
	if humidity.empty():
		return 0.45
	var ix = clamp(int(round(float(x))), 0, width - 1)
	var iz = clamp(int(round(float(z))), 0, depth - 1)
	return float(humidity[iz * width + ix])

func is_water_at(x, z):
	if heights.empty():
		return false
	return ground_height(x, z) < sea_level - 0.02

func get_water_state_for_player(position):
	if not is_water_at(position.x, position.z):
		return false
	return position.y < sea_level + 1.15

func _create_entity(name, material, position, velocity, mass, radius, restitution, dynamic):
	var entity = {
		"id": next_entity_id,
		"name": str(name),
		"material": int(material),
		"alive": true,
		"dynamic": bool(dynamic),
		"x": float(position.x),
		"y": float(position.y),
		"z": float(position.z),
		"vx": float(velocity.x),
		"vy": float(velocity.y),
		"vz": float(velocity.z),
		"mass": max(0.001, float(mass)),
		"radius": max(0.05, float(radius)),
		"restitution": clamp(float(restitution), 0.0, 1.0),
		"inventory": {"0": 0.0, "1": 0.0, "2": 0.0, "3": 0.0}
	}
	entities.append(entity)
	next_entity_id += 1
	return int(entity.id)

func ground_height(x, z):
	if heights.empty():
		return 0.0
	var qx = clamp(float(x), 0.0, float(width - 1))
	var qz = clamp(float(z), 0.0, float(depth - 1))
	var x0 = int(floor(qx))
	var z0 = int(floor(qz))
	var x1 = min(x0 + 1, width - 1)
	var z1 = min(z0 + 1, depth - 1)
	var tx = qx - float(x0)
	var tz = qz - float(z0)
	var h00 = float(heights[z0 * width + x0])
	var h10 = float(heights[z0 * width + x1])
	var h01 = float(heights[z1 * width + x0])
	var h11 = float(heights[z1 * width + x1])
	var hx0 = lerp(h00, h10, tx)
	var hx1 = lerp(h01, h11, tx)
	return lerp(hx0, hx1, tz)

func get_resource(resource_id):
	for resource in resources:
		if int(resource.id) == int(resource_id):
			return resource
	return null

func get_entity(entity_id):
	for entity in entities:
		if int(entity.id) == int(entity_id):
			return entity
	return null

func set_human_test_pose(position):
	var actor = get_entity(human_test_actor_id)
	if actor == null or not actor.alive:
		return false
	if not _finite_vec3(position):
		return false
	if position.x < 0.0 or position.z < 0.0 or position.x > float(width - 1) or position.z > float(depth - 1):
		return false
	var old_position = Vector3(float(actor.x), float(actor.y), float(actor.z))
	actor.x = float(position.x)
	actor.y = max(float(position.y), ground_height(position.x, position.z) + float(actor.radius))
	actor.z = float(position.z)
	actor.vx = 0.0
	actor.vy = 0.0
	actor.vz = 0.0
	var new_position = Vector3(float(actor.x), float(actor.y), float(actor.z))
	if old_position.distance_to(new_position) > 0.05:
		persistent_revision += 1
	return true

func reset_human_test_actor():
	return set_human_test_pose(find_human_spawn())

func apply_action(action, source = "agent"):
	var result = {"accepted": false, "amount": 0.0, "reason": "unsupported_action"}
	if typeof(action) != TYPE_DICTIONARY:
		return _store_action_result(result)
	var action_type = str(action.get("type", ""))
	if action_type != "gather_resource":
		return _store_action_result(result)
	var actor_id = int(action.get("actor_id", 0))
	var target_id = int(action.get("target_id", 0))
	var amount = float(action.get("amount", 1.0))
	if str(source) == "agent" and actor_id == human_test_actor_id:
		result.reason = "developer_actor_unavailable"
		return _store_action_result(result)
	var actor = get_entity(actor_id)
	if actor == null or not actor.alive:
		result.reason = "invalid_actor"
		return _store_action_result(result)
	if not _finite(amount) or amount <= 0.0:
		result.reason = "invalid_amount"
		return _store_action_result(result)
	var resource = get_resource(target_id)
	if resource == null or float(resource.remaining) <= 0.0:
		result.reason = "invalid_or_depleted_resource"
		return _store_action_result(result)
	if not _finite_resource(resource) or not _finite_entity(actor):
		result.reason = "invalid_state"
		return _store_action_result(result)
	var dx = float(actor.x) - float(resource.x)
	var dy = float(actor.y) - float(resource.y)
	var dz = float(actor.z) - float(resource.z)
	if dx * dx + dy * dy + dz * dz > GATHER_RANGE * GATHER_RANGE:
		result.reason = "out_of_range"
		return _store_action_result(result)
	var current = float(actor.inventory.get(str(int(resource.kind)), 0.0))
	var free_capacity = INVENTORY_CAPACITY_PER_KIND - current
	if not _finite(free_capacity) or free_capacity <= 0.0:
		result.reason = "inventory_full"
		return _store_action_result(result)
	var gather_amount = min(amount, min(float(resource.remaining), free_capacity))
	if not _finite(gather_amount) or gather_amount <= 0.0:
		result.reason = "inventory_full"
		return _store_action_result(result)
	resource.remaining = max(0.0, float(resource.remaining) - gather_amount)
	actor.inventory[str(int(resource.kind))] = current + gather_amount
	world_revision += 1
	persistent_revision += 1
	result.accepted = true
	result.amount = gather_amount
	result.reason = resource_kind_name(int(resource.kind))
	return _store_action_result(result)

func _store_action_result(result):
	last_action_result = result.duplicate(true)
	return result

func advance_frame(frame_delta):
	if not _finite(float(frame_delta)) or frame_delta <= 0.0:
		return 0
	if paused:
		return 0
	accumulator = min(accumulator + float(frame_delta) * speed, MAX_ACCUMULATOR)
	var steps = 0
	while accumulator >= fixed_dt and steps < MAX_FRAME_STEPS:
		step()
		accumulator -= fixed_dt
		steps += 1
	if steps == MAX_FRAME_STEPS and accumulator >= fixed_dt * 8.0:
		# Under a severe frame stall, discard only excessive backlog so one bad frame cannot spiral forever.
		accumulator = min(accumulator, fixed_dt * 8.0)
		_record_event("simulation_clamped", "frame backlog capped")
	return steps

func step():
	if paused:
		return false
	var dt = clamp(fixed_dt, MIN_DT, MAX_DT)
	_step_environment(dt)
	_step_physics(dt)
	seconds += dt
	if not _finite(seconds):
		seconds = 0.0
	if tick < 9223372036854775807:
		tick += 1
	_record_event("simulation_stepped", "step")
	return true

func _step_environment(dt):
	environment_accumulator += dt
	if environment_accumulator < 0.25:
		return
	var elapsed = environment_accumulator
	environment_accumulator = 0.0
	var day_fraction = solar_day_fraction()
	var sun_value = sin((day_fraction - 0.25) * PI * 2.0)
	sunlight = clamp(max(0.0, sun_value), 0.0, 1.0)
	global_temperature = 283.5 + 6.5 * sunlight
	var blend = clamp(elapsed / 60.0, 0.0, 1.0)
	for z in range(depth):
		for x in range(width):
			var idx = z * width + x
			var target = global_temperature + 4.0 * sin(float(x) * 0.04) + cos(float(z) * 0.025)
			temperatures[idx] = lerp(float(temperatures[idx]), target, blend)
	world_revision += 1

func solar_day_fraction():
	return fmod(max(0.0, seconds), DAY_LENGTH_SECONDS) / DAY_LENGTH_SECONDS

func solar_phase_name():
	var fraction = solar_day_fraction()
	if fraction < 0.20 or fraction >= 0.80:
		return "Night"
	if fraction < 0.28:
		return "Dawn"
	if fraction < 0.70:
		return "Day"
	return "Dusk"

func _step_physics(dt):
	var max_substep = 1.0 / 120.0
	var substeps = int(ceil(dt / max_substep))
	substeps = clamp(substeps, 1, 480)
	var sub_dt = dt / float(substeps)
	for _s in range(substeps):
		for entity in entities:
			if not entity.alive or not entity.dynamic:
				continue
			_sanitize_entity(entity)
			entity.vy -= GRAVITY * sub_dt
			entity.x += entity.vx * sub_dt
			entity.y += entity.vy * sub_dt
			entity.z += entity.vz * sub_dt
			entity.x = clamp(entity.x, 0.0, float(width) - 0.0001)
			entity.z = clamp(entity.z, 0.0, float(depth) - 0.0001)
			var floor_y = ground_height(entity.x, entity.z) + entity.radius
			if entity.y < floor_y:
				entity.y = floor_y
				if abs(entity.vy) > 0.05:
					entity.vy = -entity.vy * entity.restitution
				else:
					entity.vy = 0.0
				entity.vx *= 0.98
				entity.vz *= 0.98

func _sanitize_entity(entity):
	if not _finite_entity(entity) or float(entity.mass) <= 0.0:
		entity.mass = 1.0
	entity.radius = 0.35 if not _finite(float(entity.radius)) or float(entity.radius) <= 0.0 else float(entity.radius)
	entity.restitution = 0.2 if not _finite(float(entity.restitution)) else clamp(float(entity.restitution), 0.0, 1.0)
	if not _finite_entity(entity):
		var x = float(width - 1) * 0.5
		var z = float(depth - 1) * 0.5
		entity.x = x
		entity.y = ground_height(x, z) + entity.radius
		entity.z = z
		entity.vx = 0.0
		entity.vy = 0.0
		entity.vz = 0.0

func _finite_entity(entity):
	return _finite(float(entity.x)) and _finite(float(entity.y)) and _finite(float(entity.z)) and _finite(float(entity.vx)) and _finite(float(entity.vy)) and _finite(float(entity.vz)) and _finite(float(entity.mass)) and _finite(float(entity.radius)) and _finite(float(entity.restitution))

func _finite_resource(resource):
	return _finite(float(resource.x)) and _finite(float(resource.y)) and _finite(float(resource.z)) and _finite(float(resource.remaining)) and _finite(float(resource.max)) and float(resource.remaining) >= 0.0 and float(resource.max) >= 0.0 and float(resource.remaining) <= float(resource.max)

func resource_kind_name(kind):
	match int(kind):
		0: return "stone"
		1: return "wood"
		2: return "water"
		3: return "soil"
	return "unknown"

func calendar():
	var world_seconds = max(0.0, seconds) / DAY_LENGTH_SECONDS * 86400.0
	var total = int(clamp(floor(world_seconds), 0.0, 9223372036854775807.0))
	var second = total % 60
	var minute_total = int(total / 60)
	var minute = minute_total % 60
	var hour_total = int(minute_total / 60)
	var hour = hour_total % 24
	var day = int(hour_total / 24)
	var week = int(day / 7)
	var day_of_year = day % 360
	var month = int(day_of_year / 30)
	var day_of_month = day_of_year % 30
	var year = int(day / 360)
	var day_of_week = day % 7
	return [year, month, day_of_month, week, hour, minute, second, day_of_week]

func set_speed(value):
	var requested = float(value)
	if not _finite(requested) or requested < 0.0:
		requested = 1.0
	speed = clamp(requested, 0.0, MAX_SPEED)

func toggle_pause():
	paused = not paused
	return paused

func save_world(path = "user://origin/world.json"):
	var data = _serialize()
	if data.empty():
		return false
	var directory = Directory.new()
	var parent = path.get_base_dir()
	if parent != ".":
		var dir_error = directory.make_dir_recursive(parent)
		if dir_error != OK and not directory.dir_exists(parent):
			return false
	var temp_path = path + ".tmp"
	var file = File.new()
	if file.open(temp_path, File.WRITE) != OK:
		return false
	file.store_string(to_json(data))
	file.close()
	if directory.file_exists(path):
		var backup = path + ".bak"
		if directory.file_exists(backup):
			directory.remove(backup)
		if directory.rename(path, backup) != OK:
			directory.remove(temp_path)
			return false
	if directory.rename(temp_path, path) != OK:
		if directory.file_exists(path + ".bak"):
			directory.rename(path + ".bak", path)
		directory.remove(temp_path)
		return false
	if directory.file_exists(path + ".bak"):
		directory.remove(path + ".bak")
	return true

func load_world(path = "user://origin/world.json"):
	var file = File.new()
	if not file.file_exists(path):
		var legacy_paths = ["user://origin/world.save", "storage/saves/world.save"]
		for legacy_path in legacy_paths:
			if file.file_exists(legacy_path):
				return import_legacy_save(legacy_path)
		return false
	if file.open(path, File.READ) != OK:
		return false
	if file.get_len() <= 0 or file.get_len() > MAX_SAVE_BYTES:
		file.close()
		return false
	var text = file.get_as_text()
	file.close()
	var parsed = parse_json(text)
	if typeof(parsed) != TYPE_DICTIONARY:
		return false
	var format = int(parsed.get("format", 0))
	if format != SAVE_FORMAT and format != PREVIOUS_SAVE_FORMAT:
		return false
	return _apply_serialized(parsed)

func import_legacy_save(path):
	var file = File.new()
	if file.open(path, File.READ) != OK:
		return false
	var lines = file.get_as_text().split("\n")
	file.close()
	if lines.empty():
		return false
	var first = lines[0].strip_edges().split(" ", false)
	if first.size() < 2 or first[0] != "ORIGIN_SAVE":
		return false
	var legacy_version = int(first[1])
	if legacy_version != 3 and legacy_version != LEGACY_SAVE_FORMAT:
		return false
	# Import only the stable scalar/state subset. Terrain and entities are read below.
	var incoming = {"tick": "0", "seconds": "0.0", "dt": str(DEFAULT_DT), "speed": "1.0", "paused": "0", "seed": str(world_seed), "width": str(width), "depth": str(depth), "sea_level": str(sea_level), "world_revision": "0", "terrain_revision": "1"}
	var i = 1
	while i < lines.size():
		var line = lines[i].strip_edges()
		if line == "END":
			break
		var parts = line.split(" ", false)
		if parts.empty():
			i += 1
			continue
		var key = parts[0]
		if key == "cells" and parts.size() >= 2:
			incoming["cell_count"] = int(parts[1])
			if incoming["cell_count"] <= 0 or incoming["cell_count"] > MAX_SAVE_CELLS:
				return false
			incoming["cells"] = []
			i += 1
			for _c in range(incoming["cell_count"]):
				if i >= lines.size(): return false
				var cp = lines[i].strip_edges().split(" ", false)
				if cp.size() < 6: return false
				incoming["cells"].append([int(cp[0]), int(cp[1]), float(cp[2]), float(cp[3]), float(cp[4]), float(cp[5])])
				i += 1
			continue
		if key == "resources" and parts.size() >= 3:
			incoming["resources"] = []
			incoming["next_resource_id"] = int(parts[2])
			var count = int(parts[1])
			if count < 0 or count > MAX_RESOURCES:
				return false
			i += 1
			for _r in range(count):
				if i >= lines.size(): return false
				var rp = lines[i].strip_edges().split(" ", false)
				if rp.size() < 7: return false
				incoming["resources"].append({"id": int(rp[0]), "kind": int(rp[1]), "x": float(rp[2]), "y": float(rp[3]), "z": float(rp[4]), "remaining": float(rp[5]), "max": float(rp[6])})
				i += 1
			continue
		if key == "entities" and parts.size() >= 2:
			incoming["entities"] = []
			var count_e = int(parts[1])
			if count_e < 0 or count_e > MAX_ENTITIES:
				return false
			i += 1
			for _e in range(count_e):
				if i >= lines.size(): return false
				var ep = lines[i].strip_edges().split(" ", false)
				if ep.size() < 14: return false
				var entity_name = ep[3].replace("\"", "")
				var entity = {"id": int(ep[0]), "material": int(ep[1]), "alive": int(ep[2]) != 0, "name": entity_name, "x": float(ep[4]), "y": float(ep[5]), "z": float(ep[6]), "vx": float(ep[7]), "vy": float(ep[8]), "vz": float(ep[9]), "mass": float(ep[10]), "radius": float(ep[11]), "restitution": float(ep[12]), "dynamic": int(ep[13]) != 0, "inventory": {"0": 0.0, "1": 0.0, "2": 0.0, "3": 0.0}}
				i += 1
				if i >= lines.size(): return false
				var inv = lines[i].strip_edges().split(" ", false)
				var p = 0
				while p + 1 < inv.size() and p < 8:
					entity.inventory[str(int(inv[p]))] = float(inv[p + 1])
					p += 2
				incoming["entities"].append(entity)
				i += 1
			continue
		if key == "environment" and parts.size() >= 3:
			incoming["environment_sun"] = parts[1]
			incoming["environment_temp"] = parts[2]
		elif parts.size() >= 2:
			incoming[key] = parts[1]
		i += 1
	if not incoming.has("width") or not incoming.has("depth") or not incoming.has("cells"):
		return false
	incoming["format"] = SAVE_FORMAT
	incoming["schema"] = "origin_godot_runtime"
	var data = {
		"format": SAVE_FORMAT,
		"schema": "origin_godot_runtime",
		"clock": {"tick": int(str(incoming.get("tick", 0))), "seconds": float(str(incoming.get("seconds", 0.0))), "dt": float(str(incoming.get("dt", DEFAULT_DT))), "speed": float(str(incoming.get("speed", 1.0))), "paused": int(str(incoming.get("paused", "0"))) != 0},
		"world": {"seed": int(str(incoming.get("seed", world_seed))), "width": int(str(incoming["width"])), "depth": int(str(incoming["depth"])), "sea_level": float(str(incoming.get("sea_level", sea_level))), "world_revision": int(str(incoming.get("world_revision", 0))), "terrain_revision": int(str(incoming.get("terrain_revision", 1))), "next_resource_id": int(incoming.get("next_resource_id", 1)), "heights": [], "temperatures": [], "water_depths": [], "humidity": []},
		"resources": incoming.get("resources", []),
		"entities": incoming.get("entities", []),
		"human_test_actor_id": int(str(incoming.get("test_actor", 0))),
		"environment": {"sunlight": float(str(incoming.get("environment_sun", "1.0"))), "temperature": float(str(incoming.get("environment_temp", "288.15")))}
	}
	for cell in incoming["cells"]:
		data["world"]["heights"].append(cell[2])
		data["world"]["temperatures"].append(cell[3])
		data["world"]["water_depths"].append(cell[4])
		data["world"]["humidity"].append(cell[5])

	var imported_human_id = int(data["human_test_actor_id"])
	var highest_entity_id = 0
	var has_human = false
	for entity in data["entities"]:
		highest_entity_id = max(highest_entity_id, int(entity.get("id", 0)))
		if str(entity.get("name", "")) == "HumanTester":
			has_human = true
			imported_human_id = int(entity.get("id", 0))
	if not has_human and legacy_version == 3:
		imported_human_id = highest_entity_id + 1
		var center_x = int(int(data["world"]["width"]) / 2)
		var center_z = int(int(data["world"]["depth"]) / 2)
		var center_index = center_z * int(data["world"]["width"]) + center_x
		var center_height = float(data["world"]["heights"][center_index])
		data["entities"].append({
			"id": imported_human_id,
			"material": 3,
			"alive": true,
			"name": "HumanTester",
			"dynamic": false,
			"x": float(center_x),
			"y": center_height + 0.35,
			"z": float(center_z),
			"vx": 0.0,
			"vy": 0.0,
			"vz": 0.0,
			"mass": 70.0,
			"radius": 0.35,
			"restitution": 0.0,
			"inventory": {"0": 0.0, "1": 0.0, "2": 0.0, "3": 0.0}
		})
	data["human_test_actor_id"] = imported_human_id
	return _apply_serialized(data)

func _serialize():
	if heights.size() != width * depth:
		return {}
	return {
		"format": SAVE_FORMAT,
		"schema": "origin_godot_runtime",
		"clock": {"tick": tick, "seconds": seconds, "dt": fixed_dt, "speed": speed, "paused": paused},
		"world": {"seed": world_seed, "width": width, "depth": depth, "sea_level": sea_level, "world_revision": world_revision, "persistent_revision": persistent_revision, "terrain_revision": terrain_revision, "next_resource_id": next_resource_id, "heights": heights, "temperatures": temperatures, "water_depths": water_depths, "humidity": humidity},
		"resources": resources,
		"entities": entities,
		"human_test_actor_id": human_test_actor_id,
		"environment": {"sunlight": sunlight, "temperature": global_temperature}
	}

func _apply_serialized(data):
	if typeof(data) != TYPE_DICTIONARY:
		return false
	var incoming_world = data.get("world", null)
	var incoming_clock = data.get("clock", null)
	var incoming_environment = data.get("environment", null)
	if typeof(incoming_world) != TYPE_DICTIONARY or typeof(incoming_clock) != TYPE_DICTIONARY or typeof(incoming_environment) != TYPE_DICTIONARY:
		return false
	var data_format = int(data.get("format", 0))
	if data_format != SAVE_FORMAT and data_format != PREVIOUS_SAVE_FORMAT:
		return false

	var candidate_width = int(incoming_world.get("width", 0))
	var candidate_depth = int(incoming_world.get("depth", 0))
	if candidate_width < 8 or candidate_depth < 8 or candidate_width > MAX_DIMENSION or candidate_depth > MAX_DIMENSION:
		return false
	var candidate_cell_count = candidate_width * candidate_depth
	if candidate_cell_count <= 0 or candidate_cell_count > MAX_SAVE_CELLS:
		return false

	var hs = incoming_world.get("heights", [])
	var ts = incoming_world.get("temperatures", [])
	var ws = incoming_world.get("water_depths", [])
	var hum = incoming_world.get("humidity", [])
	if typeof(hs) != TYPE_ARRAY or typeof(ts) != TYPE_ARRAY or typeof(ws) != TYPE_ARRAY or typeof(hum) != TYPE_ARRAY:
		return false
	if hs.size() != candidate_cell_count or ts.size() != candidate_cell_count or ws.size() != candidate_cell_count or hum.size() != candidate_cell_count:
		return false

	var candidate_sea_level = float(incoming_world.get("sea_level", sea_level))
	var candidate_world_revision = int(incoming_world.get("world_revision", 0))
	var candidate_persistent_revision = int(incoming_world.get("persistent_revision", candidate_world_revision))
	var candidate_terrain_revision = int(incoming_world.get("terrain_revision", 1))
	var candidate_next_resource_id = int(incoming_world.get("next_resource_id", 0))
	if not _finite(candidate_sea_level) or abs(candidate_sea_level) > 100000.0:
		return false
	if candidate_world_revision < 0 or candidate_persistent_revision < 0 or candidate_terrain_revision < 0 or candidate_next_resource_id <= 0:
		return false

	for value in hs:
		if not _finite(float(value)):
			return false
	for value in ts:
		if not _finite(float(value)):
			return false
	for value in ws:
		if not _finite(float(value)) or float(value) < 0.0:
			return false
	for value in hum:
		if not _finite(float(value)) or float(value) < 0.0 or float(value) > 1.0:
			return false

	var new_resources = data.get("resources", [])
	if typeof(new_resources) != TYPE_ARRAY or new_resources.size() > MAX_RESOURCES:
		return false
	var resource_ids = {}
	var max_resource_id = 0
	for resource in new_resources:
		if typeof(resource) != TYPE_DICTIONARY:
			return false
		if not _has_keys(resource, ["id", "kind", "x", "y", "z", "remaining", "max"]):
			return false
		var resource_id = int(resource.get("id", 0))
		if resource_id <= 0 or resource_ids.has(resource_id) or not _finite_resource(resource):
			return false
		var resource_kind = int(resource.get("kind", -1))
		var resource_x = float(resource.get("x", 0.0))
		var resource_y = float(resource.get("y", 0.0))
		var resource_z = float(resource.get("z", 0.0))
		var resource_remaining = float(resource.get("remaining", 0.0))
		var resource_max = float(resource.get("max", 0.0))
		if resource_kind < 0 or resource_kind > 3:
			return false
		if resource_x < 0.0 or resource_z < 0.0 or resource_x > float(candidate_width - 1) or resource_z > float(candidate_depth - 1):
			return false
		if resource_max <= 0.0 or resource_remaining < 0.0 or resource_remaining > resource_max or not _finite(resource_y) or abs(resource_y) > 1000000.0:
			return false
		resource_ids[resource_id] = true
		max_resource_id = max(max_resource_id, resource_id)
	if candidate_next_resource_id <= max_resource_id:
		return false

	var new_entities = data.get("entities", [])
	if typeof(new_entities) != TYPE_ARRAY or new_entities.size() > MAX_ENTITIES:
		return false
	var entity_ids = {}
	var found_human = false
	var human_count = 0
	var max_entity_id = 0
	var candidate_human_id = int(data.get("human_test_actor_id", 0))
	if candidate_human_id <= 0:
		return false
	for entity in new_entities:
		if typeof(entity) != TYPE_DICTIONARY:
			return false
		if not _has_keys(entity, ["id", "name", "material", "alive", "dynamic", "x", "y", "z", "vx", "vy", "vz", "mass", "radius", "restitution", "inventory"]):
			return false
		var entity_id = int(entity.get("id", 0))
		if entity_id <= 0 or entity_ids.has(entity_id) or not _finite_entity(entity):
			return false
		var entity_material = int(entity.get("material", -1))
		if entity_material < 0 or entity_material > 3:
			return false
		var inventory = entity.get("inventory", {})
		if typeof(inventory) != TYPE_DICTIONARY:
			return false
		for kind in [0, 1, 2, 3]:
			var inventory_value = float(inventory.get(str(kind), 0.0))
			if not _finite(inventory_value) or inventory_value < 0.0 or inventory_value > INVENTORY_CAPACITY_PER_KIND:
				return false
		var entity_x = float(entity.get("x", 0.0))
		var entity_y = float(entity.get("y", 0.0))
		var entity_z = float(entity.get("z", 0.0))
		var entity_mass = float(entity.get("mass", 0.0))
		var entity_radius = float(entity.get("radius", 0.0))
		var entity_restitution = float(entity.get("restitution", 0.0))
		if entity_x < 0.0 or entity_z < 0.0 or entity_x > float(candidate_width - 1) or entity_z > float(candidate_depth - 1):
			return false
		if abs(entity_y) > 1000000.0 or entity_mass <= 0.0 or entity_radius <= 0.0 or entity_restitution < 0.0 or entity_restitution > 1.0:
			return false
		entity_ids[entity_id] = true
		max_entity_id = max(max_entity_id, entity_id)
		if str(entity.get("name", "")) == "HumanTester":
			human_count += 1
			if entity_id == candidate_human_id:
				found_human = true
	if new_entities.empty() or max_entity_id >= 9223372036854775806 or not found_human or human_count != 1:
		return false

	var candidate_tick = int(incoming_clock.get("tick", 0))
	var candidate_seconds = float(incoming_clock.get("seconds", 0.0))
	var candidate_dt = float(incoming_clock.get("dt", DEFAULT_DT))
	var candidate_speed = float(incoming_clock.get("speed", 1.0))
	if candidate_tick < 0 or not _finite(candidate_seconds) or candidate_seconds < 0.0 or not _finite(candidate_dt) or candidate_dt <= 0.0 or not _finite(candidate_speed) or candidate_speed < 0.0:
		return false
	var candidate_sunlight = float(incoming_environment.get("sunlight", 1.0))
	var candidate_temperature = float(incoming_environment.get("temperature", 288.15))
	if not _finite(candidate_sunlight) or candidate_sunlight < 0.0 or candidate_sunlight > 1.0 or not _finite(candidate_temperature):
		return false

	# Commit only after the complete candidate has passed validation.
	width = candidate_width
	depth = candidate_depth
	heights = hs.duplicate()
	temperatures = ts.duplicate()
	water_depths = ws.duplicate()
	humidity = hum.duplicate()
	resources = new_resources.duplicate(true)
	entities = new_entities.duplicate(true)
	world_seed = int(incoming_world.get("seed", world_seed))
	sea_level = candidate_sea_level
	world_revision = candidate_world_revision
	persistent_revision = candidate_persistent_revision
	terrain_revision = candidate_terrain_revision
	next_resource_id = candidate_next_resource_id
	next_entity_id = max_entity_id + 1
	human_test_actor_id = candidate_human_id
	tick = candidate_tick
	seconds = candidate_seconds
	fixed_dt = clamp(candidate_dt, MIN_DT, MAX_DT)
	speed = clamp(candidate_speed, 0.0, MAX_SPEED)
	paused = bool(incoming_clock.get("paused", false))
	sunlight = candidate_sunlight
	global_temperature = candidate_temperature
	accumulator = 0.0
	environment_accumulator = 0.0
	return true

func run_self_tests():
	var backup = _serialize()
	var event_backup = event_log.duplicate(true)
	var checks = []
	_test(checks, "world dimensions", heights.size() == width * depth)
	var min_height = 999999.0
	var max_height = -999999.0
	var has_water = false
	for value in heights:
		min_height = min(min_height, float(value))
		max_height = max(max_height, float(value))
	for value in water_depths:
		if float(value) > 0.02:
			has_water = true
			break
	_test(checks, "terrain has meaningful relief", max_height - min_height > 5.0)
	_test(checks, "water exists below sea level", has_water)
	var spawn_test = find_human_spawn()
	_test(checks, "HumanTester spawn is safe", spawn_test.y > ground_height(spawn_test.x, spawn_test.z) and not is_water_at(spawn_test.x, spawn_test.z))
	var saved_seconds_for_cycle = seconds
	seconds = DAY_LENGTH_SECONDS * 0.50
	environment_accumulator = 0.25
	_step_environment(0.0)
	var noon_light = sunlight
	seconds = 0.0
	environment_accumulator = 0.25
	_step_environment(0.0)
	var midnight_light = sunlight
	_test(checks, "solar cycle has real contrast", noon_light > 0.95 and midnight_light < 0.05)
	seconds = saved_seconds_for_cycle
	_step_environment(0.0)
	_test(checks, "resource IDs unique", _ids_unique(resources))
	_test(checks, "entity IDs unique", _ids_unique(entities))
	var mirror = get_script().new()
	mirror.initialize(world_seed, width, depth)
	_test(checks, "deterministic terrain", hash(heights) == hash(mirror.heights))
	var resource = resources[0] if not resources.empty() else null
	var actor = get_entity(human_test_actor_id)
	if resource != null and actor != null:
		set_human_test_pose(Vector3(resource.x, resource.y + actor.radius, resource.z))
		var before = float(resource.remaining)
		var gathered = apply_action({"type": "gather_resource", "actor_id": human_test_actor_id, "target_id": int(resource.id), "amount": 1.0}, "developer")
		_test(checks, "developer gather", gathered.accepted and float(resource.remaining) < before)
		var protected = apply_action({"type": "gather_resource", "actor_id": human_test_actor_id, "target_id": int(resource.id), "amount": 1.0}, "agent")
		_test(checks, "agent cannot control HumanTester", not protected.accepted and protected.reason == "developer_actor_unavailable")
	else:
		_test(checks, "developer gather", false)
		_test(checks, "agent cannot control HumanTester", false)
	var test_path = "user://origin/self_test_%d.json" % OS.get_ticks_usec()
	_test(checks, "save", save_world(test_path))
	var saved_revision = world_revision
	world_revision += 7
	_test(checks, "load", load_world(test_path) and world_revision == saved_revision)
	var previous_format = backup.duplicate(true)
	previous_format["format"] = PREVIOUS_SAVE_FORMAT
	_test(checks, "previous save format loads", _apply_serialized(previous_format))
	_apply_serialized(backup)
	var custom_runtime = get_script().new()
	custom_runtime.initialize(world_seed, 24, 20)
	var custom_save = custom_runtime._serialize()
	var custom_loaded = _apply_serialized(custom_save)
	_test(checks, "custom world size loads", custom_loaded and width == 24 and depth == 20)
	_apply_serialized(backup)
	var restored_actor = get_entity(human_test_actor_id)
	var inventory_survives = false
	var pose_survives = false
	if restored_actor != null and actor != null:
		pose_survives = Vector3(float(restored_actor.x), float(restored_actor.y), float(restored_actor.z)).distance_to(Vector3(float(actor.x), float(actor.y), float(actor.z))) < 0.001
	_test(checks, "human pose survives save/load", pose_survives)
	if restored_actor != null and resource != null:
		inventory_survives = float(restored_actor.inventory.get(str(int(resource.kind)), 0.0)) > 0.0
	_test(checks, "inventory survives save/load", inventory_survives)
	var speed_backup = _serialize()
	set_speed(16.0)
	accumulator = 0.0
	advance_frame(fixed_dt)
	_test(checks, "high speed preserves fixed-step rate", tick >= int(speed_backup["clock"]["tick"]) + 16)
	_apply_serialized(speed_backup)
	for _e in range(MAX_EVENT_LOG + 32):
		_record_event("self_test", "event-bound")
	_test(checks, "event log stays bounded", event_log.size() == MAX_EVENT_LOG)
	_apply_serialized(speed_backup)
	var capacity_resource = resources[0] if not resources.empty() else null
	var capacity_actor = get_entity(human_test_actor_id)
	var inventory_capacity_ok = false
	if capacity_resource != null and capacity_actor != null:
		capacity_actor.inventory[str(int(capacity_resource.kind))] = INVENTORY_CAPACITY_PER_KIND
		set_human_test_pose(Vector3(capacity_resource.x, capacity_resource.y + capacity_actor.radius, capacity_resource.z))
		var capacity_result = apply_action({"type": "gather_resource", "actor_id": human_test_actor_id, "target_id": int(capacity_resource.id), "amount": 1.0}, "developer")
		inventory_capacity_ok = not capacity_result.accepted and capacity_result.reason == "inventory_full"
	_test(checks, "inventory capacity is enforced", inventory_capacity_ok)
	_apply_serialized(speed_backup)
	var bad_path = "user://origin/self_test_bad_%d.json" % OS.get_ticks_usec()
	var bad_file = File.new()
	bad_file.open(bad_path, File.WRITE)
	bad_file.store_string("{\"format\":%d,\"world\":{}}" % SAVE_FORMAT)
	bad_file.close()
	var revision_before_bad_load = world_revision
	_test(checks, "invalid load is atomic", not load_world(bad_path) and world_revision == revision_before_bad_load)
	var cleanup = Directory.new()
	if cleanup.file_exists(bad_path):
		cleanup.remove(bad_path)
	if cleanup.file_exists(test_path):
		cleanup.remove(test_path)
	_apply_serialized(backup)
	event_log = event_backup
	var passed_checks = 0
	var failed_names = []
	for check in checks:
		if bool(check.ok):
			passed_checks += 1
		else:
			failed_names.append(check.name)
	return {"passed": passed_checks == checks.size(), "checks": checks, "passed_checks": passed_checks, "failed_checks": checks.size() - passed_checks, "failed_names": failed_names}

func format_self_test_report(result):
	var output = "SELF TEST: %s (%d/%d passed)" % ["PASS" if bool(result.get("passed", false)) else "FAIL", int(result.get("passed_checks", 0)), int(result.get("checks", []).size())]
	for check in result.get("checks", []):
		output += "\n%s %s" % ["OK" if bool(check.ok) else "FAIL", str(check.name)]
	return output

func _test(checks, name, condition):
	var ok = bool(condition)
	checks.append({"name": str(name), "ok": ok})
	return ok


func _has_keys(dictionary, keys):
	for key in keys:
		if not dictionary.has(key):
			return false
	return true

func _ids_unique(items):
	var seen = {}
	for item in items:
		var id = int(item.get("id", 0))
		if id <= 0 or seen.has(id): return false
		seen[id] = true
	return true

func benchmark(ticks):
	var backup = _serialize()
	var event_backup = event_log.duplicate(true)
	var count = clamp(int(ticks), 1, 100000)
	var start = OS.get_ticks_usec()
	var was_paused = paused
	paused = false
	for _i in range(count):
		step()
	var elapsed = float(OS.get_ticks_usec() - start) / 1000000.0
	_apply_serialized(backup)
	event_log = event_backup
	paused = was_paused
	return {"ticks": count, "seconds": elapsed, "ticks_per_second": float(count) / max(elapsed, 0.000001)}

func snapshot():
	return {
		"version": SAVE_FORMAT,
		"time": seconds,
		"tick": tick,
		"width": width,
		"depth": depth,
		"sea_level": sea_level,
		"world_revision": world_revision,
		"persistent_revision": persistent_revision,
		"terrain_revision": terrain_revision,
		"temperature": global_temperature,
		"sunlight": sunlight,
		"calendar": calendar(),
		"test_actor": human_test_actor_id,
		"inventory": human_inventory(),
		"resources": resources.duplicate(true),
		"entities": entities.duplicate(true)
	}

func human_inventory():
	var actor = get_entity(human_test_actor_id)
	if actor == null:
		return [0.0, 0.0, 0.0, 0.0]
	return [float(actor.inventory.get("0", 0.0)), float(actor.inventory.get("1", 0.0)), float(actor.inventory.get("2", 0.0)), float(actor.inventory.get("3", 0.0))]

func terrain():
	return heights

func _record_event(kind, message):
	event_log.append({"tick": tick, "seconds": seconds, "type": str(kind), "message": str(message)})
	while event_log.size() > MAX_EVENT_LOG:
		event_log.pop_front()

func _finite_vec3(value):
	return _finite(float(value.x)) and _finite(float(value.y)) and _finite(float(value.z))

func _finite(value):
	var f = float(value)
	return not is_nan(f) and not is_inf(f)

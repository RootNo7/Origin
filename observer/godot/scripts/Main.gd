extends Node2D

var path = "res://observer/bridge/state.txt"
var t = 0.0
var tick = 0
var w = 80
var h = 28
var temp = 0.0
var sun = 0.0
var ground = []
var entities = []

var font

func _ready():
	set_process(true)
	
	# SAFE LOADING: Fall back to a built-in bitmap font texture if font.tres fails
	font = load("res://font.tres")
	if not font:
		print("Custom font not found. Generating system fallback bitmap font.")
		font = BitmapFont.new()
		
		# Pulls Godot's internal default text styling from the running editor engine
		var default_font = Control.new().get_font("font")
		if default_font:
			font = default_font

	_update_state()
	update()

func _process(_d):
	_update_state()
	update()

func _update_state():
	var f = File.new()
	if f.open(path, File.READ) != OK:
		return
	var lines = f.get_as_text().split("\n")
	f.close()
	entities.clear()
	
	for line in lines:
		var p = line.strip_edges().split(" ")
		if p.size() < 2:
			continue
		
		if p[0] == "time":
			t = float(p[1])
		elif p[0] == "tick":
			tick = int(p[1])
		elif p[0] == "width":
			w = int(p[1])
		elif p[0] == "height":
			h = int(p[1])
		elif p[0] == "temperature":
			temp = float(p[1])
		elif p[0] == "sunlight":
			sun = float(p[1])
		elif p[0] == "ground":
			ground.clear()
			for i in range(1, p.size()):
				ground.append(float(p[i]))
		elif p[0].is_valid_integer() and p.size() >= 3:
			entities.append(Vector2(float(p[1]), float(p[2])))

func _draw():
	# Background
	draw_rect(Rect2(0, 0, 1100, 700), Color(0.025, 0.03, 0.045))
	
	# Status strings
	if font:
		draw_string(font, Vector2(32, 42), "ORIGIN — VEarth", Color(0.9, 0.93, 1))
		draw_string(font, Vector2(32, 70), "Time: %.2f s Tick: %d Temperature: %.2f K Sunlight: %.3f" % [t, tick, temp, sun], Color(0.72, 0.78, 0.86))
	
	# Viewport container
	var o = Vector2(32, 105)
	var size = Vector2(1036, 550)
	draw_rect(Rect2(o, size), Color(0.06, 0.07, 0.09))
	
	# Draw terrain line
	if ground.size() > 1:
		var pts = PoolVector2Array()
		for i in range(ground.size()):
			pts.append(Vector2(o.x + float(i) / float(ground.size() - 1) * size.x, o.y + size.y - ground[i] / float(max(1, h)) * size.y))
		draw_polyline(pts, Color(0.45, 0.70, 0.45), 2)
		
	# Draw agent entities
	for pos in entities:
		draw_circle(Vector2(o.x + pos.x / float(max(1, w)) * size.x, o.y + size.y - pos.y / float(max(1, h)) * size.y), 6, Color(0.95, 0.75, 0.30))

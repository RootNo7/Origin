extends SceneTree

const RuntimeScript = preload("res://observer/godot/scripts/OriginRuntime.gd")

func _init():
    var runtime = RuntimeScript.new()
    runtime.initialize()
    var result = runtime.run_self_tests()
    var benchmark = runtime.benchmark(10000)
    print("ORIGIN_GODOT_TESTS")
    print("passed=%s checks=%d passed_checks=%d failed_checks=%d" % [result.passed, result.checks.size(), result.passed_checks, result.failed_checks])
    print("benchmark_ticks=%d elapsed_seconds=%0.6f ticks_per_second=%0.2f" % [benchmark.ticks, benchmark.seconds, benchmark.ticks_per_second])
    if not result.passed:
        print("Origin Godot self-test FAILED")
        quit(1)
    print("Origin Godot self-test PASSED")
    quit(0)

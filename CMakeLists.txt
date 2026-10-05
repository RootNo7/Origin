cmake_minimum_required(VERSION 3.20)
project(Origin VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(origin_engine
    engine/time/simulation_clock.cpp
    engine/world/world.cpp
    engine/entities/entity_registry.cpp
    engine/physics/physics_world.cpp
    engine/chemistry/chemistry_world.cpp
    engine/environment/environment.cpp
    engine/events/event_bus.cpp
    engine/simulation/simulation.cpp
    observer/rendering/console_renderer.cpp
    observer/bridge/state_bridge.cpp
    observer/bridge/action_bridge.cpp
    observer/bridge/developer_bridge.cpp
    storage/serialization/world_serializer.cpp
)
target_include_directories(origin_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(origin_engine PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

add_executable(origin observer/application/main.cpp)
target_link_libraries(origin PRIVATE origin_engine)

enable_testing()
add_executable(origin_tests tests/test_runner.cpp)
target_link_libraries(origin_tests PRIVATE origin_engine)
add_test(NAME origin_tests COMMAND origin_tests)

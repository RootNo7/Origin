# Origin Engine - Core Simulation Library

# Engine version
set(ENGINE_VERSION ${PROJECT_VERSION})

# Engine sources
set(ENGINE_SOURCES
    core/universe.cpp
    core/universe_state.cpp
    core/entity.cpp
    core/entity_id.cpp
    core/component.cpp
    core/system.cpp
    
    time/time_system.cpp
    time/simulation_clock.cpp
    time/time_conversion.cpp
    
    physics/physics_system.cpp
    physics/rigid_body.cpp
    physics/collision.cpp
    physics/forces.cpp
    physics/integration.cpp
    physics/material_properties.cpp
    
    matter/matter_system.cpp
    matter/element.cpp
    matter/atom.cpp
    matter/molecule.cpp
    matter/material.cpp
    matter/state_of_matter.cpp
    
    chemistry/chemistry_system.cpp
    chemistry/chemical_bond.cpp
    chemistry/reaction.cpp
    chemistry/reaction_conditions.cpp
    chemistry/chemical_energy.cpp
    
    environment/environment_system.cpp
    environment/terrain.cpp
    environment/water.cpp
    environment/atmosphere.cpp
    environment/temperature.cpp
    environment/weather.cpp
    environment/climate.cpp
    environment/light.cpp
    environment/day_night_cycle.cpp
    environment/seasons.cpp
    environment/resources.cpp
    
    world/world_system.cpp
    world/v_earth.cpp
    world/region.cpp
    world/biome.cpp
    
    events/event_system.cpp
    events/event.cpp
    events/event_types.cpp
    
    life/life_system.cpp
    life/organism.cpp
    life/biological_processes.cpp
    
    simulation/simulation.cpp
    simulation/simulation_step.cpp
    simulation/determinism.cpp
    
    persistence/persistence.cpp
    persistence/save_format.cpp
    persistence/checkpoint.cpp
)

# Engine headers
set(ENGINE_HEADERS
    core/universe.h
    core/universe_state.h
    core/entity.h
    core/entity_id.h
    core/component.h
    core/system.h
    
    time/time_system.h
    time/simulation_clock.h
    time/time_conversion.h
    
    physics/physics_system.h
    physics/rigid_body.h
    physics/collision.h
    physics/forces.h
    physics/integration.h
    physics/material_properties.h
    
    matter/matter_system.h
    matter/element.h
    matter/atom.h
    matter/molecule.h
    matter/material.h
    matter/state_of_matter.h
    
    chemistry/chemistry_system.h
    chemistry/chemical_bond.h
    chemistry/reaction.h
    chemistry/reaction_conditions.h
    chemistry/chemical_energy.h
    
    environment/environment_system.h
    environment/terrain.h
    environment/water.h
    environment/atmosphere.h
    environment/temperature.h
    environment/weather.h
    environment/climate.h
    environment/light.h
    environment/day_night_cycle.h
    environment/seasons.h
    environment/resources.h
    
    world/world_system.h
    world/v_earth.h
    world/region.h
    world/biome.h
    
    events/event_system.h
    events/event.h
    events/event_types.h
    
    life/life_system.h
    life/organism.h
    life/biological_processes.h
    
    simulation/simulation.h
    simulation/simulation_step.h
    simulation/determinism.h
    
    persistence/persistence.h
    persistence/save_format.h
    persistence/checkpoint.h
)

# Create engine library
add_library(origin_engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_include_directories(origin_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/time
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/matter
    ${CMAKE_CURRENT_SOURCE_DIR}/chemistry
    ${CMAKE_CURRENT_SOURCE_DIR}/environment
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/events
    ${CMAKE_CURRENT_SOURCE_DIR}/life
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/persistence
)

# Link Python for AI bindings
target_link_libraries(origin_engine PUBLIC Python3::Python)

# Compiler options for engine
target_compile_options(origin_engine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fPIC>
)

# Install headers
install(FILES ${ENGINE_HEADERS}
    DESTINATION include/origin
    COMPONENT headers
)
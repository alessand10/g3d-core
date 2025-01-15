cmake_minimum_required(VERSION 3.30)

project(g3d-core)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../extern/glm ${CMAKE_BINARY_DIR}/glm)
add_subdirectory(../../g3d-api-layer/source api-layer-bin)
add_subdirectory(../../g3d-utils/source utils-bin)

add_library(g3d-core
    cube-map.cpp
    entity-factory.cpp
    components/transform-component.cpp
    scene-commands.cpp
    geometric-partition.cpp
    g3d-engine.cpp
    frame-graph/frame-graph.cpp
    subsystems/rendering/rendering-subsystem.cpp
    subsystems/pipeline/pipeline-subsystem.cpp
    subsystems/geometry/geometry-subsystem.cpp
    subsystems/material/material-subsystem.cpp
    frame-graph/fg-node.cpp
    scene-capture.cpp
    pbr-material.cpp
    mipmap-generator.cpp
    image-loader.cpp
    g3d-renderer.cpp
    g3d-input.cpp
)

target_link_libraries(g3d-core
    PUBLIC glm
    PUBLIC g3d-utils
    PUBLIC g3d-api-layer
)

target_include_directories(g3d-core
    PUBLIC .
    PUBLIC subsystems/engine
    PUBLIC subsystems/rendering
    PUBLIC ../g3d-api-layer/include
    PUBLIC ../g3d-utils/include
)

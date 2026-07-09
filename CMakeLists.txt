# Copyright (c) 2026 Yuancheng Li
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp32c6_boids_sim LANGUAGES C CXX)

target_sources(app PRIVATE
    src/main.cpp
    src/core/rendering/rendering.cpp
    src/features/boids/classes/flock.cpp
    src/features/boids/services/boids_renderer.cpp
    src/features/boids/store/render_snapshot_store.cpp
    src/features/scene/scene.cpp
    src/features/scene/bounds_outline.cpp
    src/features/rendering/rendering.cpp
    src/features/simulation/simulation.cpp
)

target_include_directories(app PRIVATE
    src
    src/config
    libs/Jet/src
)

file(GLOB JET_SOURCES CONFIGURE_DEPENDS "libs/Jet/src/*.cpp")
list(FILTER JET_SOURCES EXCLUDE REGEX "/(Example|Sample)\\.cpp$")
target_sources(app PRIVATE ${JET_SOURCES})

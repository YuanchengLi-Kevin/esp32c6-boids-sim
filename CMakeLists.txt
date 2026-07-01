# Copyright (c) 2026 Yuancheng Li
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp32c6_jet_game LANGUAGES C CXX)

target_sources(app PRIVATE
    src/main.cpp
    src/features/boids/boids.cpp
    src/features/game/game.cpp
    src/features/rendering/rendering.cpp
)

target_include_directories(app PRIVATE
    src
    src/config
    libs/Jet/src
)

file(GLOB JET_SOURCES CONFIGURE_DEPENDS "libs/Jet/src/*.cpp")
list(FILTER JET_SOURCES EXCLUDE REGEX "/(Example|Sample)\\.cpp$")
target_sources(app PRIVATE ${JET_SOURCES})

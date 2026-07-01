/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_JET_GAME_FEATURES_RENDERING_RENDERING_HPP_
#define ESP32C6_JET_GAME_FEATURES_RENDERING_RENDERING_HPP_

#include "features/boids/boids.hpp"

#include <cstddef>

namespace rendering {

inline constexpr std::size_t kMaxRenderedBoids = 24;

bool init(const boids::Flock& flock);
void renderFrame(const boids::Flock& flock);

}  // namespace rendering

#endif  // ESP32C6_JET_GAME_FEATURES_RENDERING_RENDERING_HPP_

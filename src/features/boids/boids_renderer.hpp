/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_RENDERER_HPP_
#define ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_RENDERER_HPP_

#include "constants/boids_sim_constants.hpp"
#include "features/boids/boids_snapshot.hpp"
#include "Scene.hpp"

#include <cstddef>

namespace boids::renderer
{

    inline constexpr std::size_t kMaxRenderedBoids = boids::kMaxRenderSnapshotBoids;

    void init(Renderer::Scene &scene);
    void update(const RenderSnapshot &snapshot);

} // namespace boids::renderer

#endif // ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_RENDERER_HPP_

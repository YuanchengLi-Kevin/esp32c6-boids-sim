/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "constants/constants.hpp"
#include "features/boids/types/render_snapshot.hpp"
#include "Scene.hpp"

#include <cstddef>

namespace boids::renderer
{

    inline constexpr std::size_t kMaxRenderedBoids = boids::kMaxRenderSnapshotBoids;

    void init(Renderer::Scene &scene);
    void update(const RenderSnapshot &snapshot);

} // namespace boids::renderer

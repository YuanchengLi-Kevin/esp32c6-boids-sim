/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "constants/constants.hpp"
#include "core/vec3/vec3.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace boids
{

	inline constexpr std::size_t kMaxRenderSnapshotBoids = constants::boids::kMaxBoids;

	struct RenderBoid
	{
		Vec3 position;
		Vec3 velocity;
	};

	struct RenderSnapshot
	{
		std::size_t count = 0;
		std::array<RenderBoid, kMaxRenderSnapshotBoids> boids{};
		uint32_t sequence = 0;
	};

} // namespace boids

/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "constants/constants.hpp"
#include "core/vec3/vec3.hpp"
#include "features/boids/types/boid.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace boids
{
	namespace boid_constants = constants::boids;

	struct FlockConfig
	{
		static constexpr std::size_t kMaxBoids = boid_constants::kMaxBoids;

		std::size_t boid_count = kMaxBoids;
		Vec3 bounds_min = {
			boid_constants::kBoundsMinX,
			boid_constants::kBoundsMinY,
			boid_constants::kBoundsMinZ,
		};
		Vec3 bounds_max = {
			boid_constants::kBoundsMaxX,
			boid_constants::kBoundsMaxY,
			boid_constants::kBoundsMaxZ,
		};
		float bounds_margin = boid_constants::kBoundsMargin;
		float bounds_strength = boid_constants::kBoundsStrength;
		float neighbor_radius = boid_constants::kNeighborRadius;
		float separation_radius = boid_constants::kSeparationRadius;
		float separation_weight = boid_constants::kSeparationWeight;
		float alignment_weight = boid_constants::kAlignmentWeight;
		float cohesion_weight = boid_constants::kCohesionWeight;
		float min_speed = boid_constants::kMinSpeed;
		float max_speed = boid_constants::kMaxSpeed;
	};

	class Flock
	{
	public:
		void init(uint32_t seed, const FlockConfig &config = FlockConfig{});
		void update(uint32_t dt_ms);

		std::size_t count() const { return active_count_; }
		const Boid &boid(std::size_t index) const { return boids_[index]; }

	private:
		FlockConfig config_{};
		std::size_t active_count_ = 0;
		std::array<Boid, FlockConfig::kMaxBoids> boids_{};
		std::array<Vec3, FlockConfig::kMaxBoids> next_velocities_{};
	};

} // namespace boids

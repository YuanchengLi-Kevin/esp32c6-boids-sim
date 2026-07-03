/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_HPP_
#define ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_HPP_

#include "constants/boids_sim_constants.hpp"
#include "core/vec3/vec3.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace boids
{

	struct Boid
	{
		Vec3 position;
		Vec3 velocity;
	};

	struct FlockConfig
	{
		static constexpr std::size_t kMaxBoids = constants::boids::kMaxBoids;

		std::size_t boid_count = kMaxBoids;
		Vec3 bounds_min = {
			constants::boids::kBoundsMinX,
			constants::boids::kBoundsMinY,
			constants::boids::kBoundsMinZ,
		};
		Vec3 bounds_max = {
			constants::boids::kBoundsMaxX,
			constants::boids::kBoundsMaxY,
			constants::boids::kBoundsMaxZ,
		};
		float bounds_margin = constants::boids::kBoundsMargin;
		float bounds_strength = constants::boids::kBoundsStrength;
		float neighbor_radius = constants::boids::kNeighborRadius;
		float separation_radius = constants::boids::kSeparationRadius;
		float separation_weight = constants::boids::kSeparationWeight;
		float alignment_weight = constants::boids::kAlignmentWeight;
		float cohesion_weight = constants::boids::kCohesionWeight;
		float min_speed = constants::boids::kMinSpeed;
		float max_speed = constants::boids::kMaxSpeed;
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

#endif // ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_HPP_

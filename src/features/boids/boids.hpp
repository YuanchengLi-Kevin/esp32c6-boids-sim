/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_JET_GAME_FEATURES_BOIDS_BOIDS_HPP_
#define ESP32C6_JET_GAME_FEATURES_BOIDS_BOIDS_HPP_

#include <array>
#include <cstddef>
#include <cstdint>

namespace boids {

struct Vec3 {
	float x;
	float y;
	float z;
};

struct Boid {
	Vec3 position;
	Vec3 velocity;
};

struct FlockConfig {
	static constexpr std::size_t kMaxBoids = 32;

	std::size_t boid_count = kMaxBoids;
	Vec3 bounds_min = {-120.0f, -80.0f, -120.0f};
	Vec3 bounds_max = {120.0f, 80.0f, 120.0f};
	float bounds_margin = 24.0f;
	float bounds_strength = 90.0f;
	float neighbor_radius = 48.0f;
	float separation_radius = 18.0f;
	float separation_weight = 110.0f;
	float alignment_weight = 2.8f;
	float cohesion_weight = 0.8f;
	float min_speed = 18.0f;
	float max_speed = 70.0f;
};

class Flock {
public:
	void init(uint32_t seed, const FlockConfig& config = FlockConfig{});
	void update(uint32_t dt_ms);

	std::size_t count() const { return active_count_; }
	const Boid& boid(std::size_t index) const { return boids_[index]; }

private:
	FlockConfig config_{};
	std::size_t active_count_ = 0;
	std::array<Boid, FlockConfig::kMaxBoids> boids_{};
	std::array<Vec3, FlockConfig::kMaxBoids> next_velocities_{};
};

}  // namespace boids

#endif  // ESP32C6_JET_GAME_FEATURES_BOIDS_BOIDS_HPP_

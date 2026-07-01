/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "boids.hpp"

#include <algorithm>
#include <cmath>

namespace boids {
namespace {

constexpr float kEpsilon = 0.0001f;

Vec3 add(const Vec3& a, const Vec3& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 subtract(const Vec3& a, const Vec3& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 multiply(const Vec3& value, float scale)
{
	return {value.x * scale, value.y * scale, value.z * scale};
}

float lengthSquared(const Vec3& value)
{
	return (value.x * value.x) + (value.y * value.y) + (value.z * value.z);
}

Vec3 clampSpeed(const Vec3& velocity, float min_speed, float max_speed)
{
	const float speed_sq = lengthSquared(velocity);
	if (speed_sq < kEpsilon) {
		return {min_speed, 0.0f, 0.0f};
	}

	const float speed = std::sqrt(speed_sq);
	if (speed > max_speed) {
		return multiply(velocity, max_speed / speed);
	}

	if (speed < min_speed) {
		return multiply(velocity, min_speed / speed);
	}

	return velocity;
}

uint32_t nextRandom(uint32_t& state)
{
	state = (state * 1664525u) + 1013904223u;
	return state;
}

float randomUnit(uint32_t& state)
{
	return static_cast<float>(nextRandom(state) >> 8) * (1.0f / 16777215.0f);
}

float randomRange(uint32_t& state, float min_value, float max_value)
{
	return min_value + ((max_value - min_value) * randomUnit(state));
}

Vec3 randomVector(uint32_t& state, const Vec3& min_value, const Vec3& max_value)
{
	return {
		randomRange(state, min_value.x, max_value.x),
		randomRange(state, min_value.y, max_value.y),
		randomRange(state, min_value.z, max_value.z),
	};
}

Vec3 boundsSteering(const Boid& boid, const FlockConfig& config)
{
	Vec3 steering = {0.0f, 0.0f, 0.0f};

	if (boid.position.x < config.bounds_min.x + config.bounds_margin) {
		steering.x += config.bounds_strength;
	} else if (boid.position.x > config.bounds_max.x - config.bounds_margin) {
		steering.x -= config.bounds_strength;
	}

	if (boid.position.y < config.bounds_min.y + config.bounds_margin) {
		steering.y += config.bounds_strength;
	} else if (boid.position.y > config.bounds_max.y - config.bounds_margin) {
		steering.y -= config.bounds_strength;
	}

	if (boid.position.z < config.bounds_min.z + config.bounds_margin) {
		steering.z += config.bounds_strength;
	} else if (boid.position.z > config.bounds_max.z - config.bounds_margin) {
		steering.z -= config.bounds_strength;
	}

	return steering;
}

Vec3 clampPosition(const Vec3& position, const FlockConfig& config)
{
	return {
		std::max(config.bounds_min.x, std::min(config.bounds_max.x, position.x)),
		std::max(config.bounds_min.y, std::min(config.bounds_max.y, position.y)),
		std::max(config.bounds_min.z, std::min(config.bounds_max.z, position.z)),
	};
}

}  // namespace

void Flock::init(uint32_t seed, const FlockConfig& config)
{
	config_ = config;
	active_count_ = std::min(config_.boid_count, FlockConfig::kMaxBoids);

	uint32_t random_state = seed == 0u ? 1u : seed;
	for (std::size_t i = 0; i < active_count_; ++i) {
		const Vec3 direction = randomVector(random_state, {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
		const float speed = randomRange(random_state, config_.min_speed, config_.max_speed);

		boids_[i].position = randomVector(random_state, config_.bounds_min, config_.bounds_max);
		boids_[i].velocity = clampSpeed(multiply(direction, speed), config_.min_speed, config_.max_speed);
		next_velocities_[i] = boids_[i].velocity;
	}
}

void Flock::update(uint32_t dt_ms)
{
	if (active_count_ == 0u || dt_ms == 0u) {
		return;
	}

	const float dt_seconds = static_cast<float>(dt_ms) * 0.001f;
	const float neighbor_radius_sq = config_.neighbor_radius * config_.neighbor_radius;
	const float separation_radius_sq = config_.separation_radius * config_.separation_radius;

	for (std::size_t i = 0; i < active_count_; ++i) {
		Vec3 separation = {0.0f, 0.0f, 0.0f};
		Vec3 alignment = {0.0f, 0.0f, 0.0f};
		Vec3 cohesion = {0.0f, 0.0f, 0.0f};
		std::size_t neighbor_count = 0;

		for (std::size_t j = 0; j < active_count_; ++j) {
			if (i == j) {
				continue;
			}

			const Vec3 offset = subtract(boids_[j].position, boids_[i].position);
			const float distance_sq = lengthSquared(offset);
			if (distance_sq > neighbor_radius_sq) {
				continue;
			}

			alignment = add(alignment, boids_[j].velocity);
			cohesion = add(cohesion, boids_[j].position);
			++neighbor_count;

			if (distance_sq < separation_radius_sq && distance_sq > kEpsilon) {
				separation = add(separation, multiply(offset, -1.0f / distance_sq));
			}
		}

		Vec3 acceleration = boundsSteering(boids_[i], config_);
		if (neighbor_count > 0u) {
			const float inv_neighbors = 1.0f / static_cast<float>(neighbor_count);
			alignment = subtract(multiply(alignment, inv_neighbors), boids_[i].velocity);
			cohesion = subtract(multiply(cohesion, inv_neighbors), boids_[i].position);

			acceleration = add(acceleration, multiply(separation, config_.separation_weight));
			acceleration = add(acceleration, multiply(alignment, config_.alignment_weight));
			acceleration = add(acceleration, multiply(cohesion, config_.cohesion_weight));
		}

		next_velocities_[i] = clampSpeed(
			add(boids_[i].velocity, multiply(acceleration, dt_seconds)),
			config_.min_speed,
			config_.max_speed);
	}

	for (std::size_t i = 0; i < active_count_; ++i) {
		boids_[i].velocity = next_velocities_[i];
		boids_[i].position = clampPosition(
			add(boids_[i].position, multiply(boids_[i].velocity, dt_seconds)),
			config_);
	}
}

}  // namespace boids

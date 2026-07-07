/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/boids/model/flock.hpp"

#include <algorithm>
#include <cmath>

namespace boids
{
	namespace
	{

		constexpr float kEpsilon = 0.0001f;

		float lengthSquared(const Vec3 &value)
		{
			return (value.x * value.x) + (value.y * value.y) + (value.z * value.z);
		}

		uint32_t nextRandom(uint32_t &state)
		{
			state = (state * 1664525u) + 1013904223u;
			return state;
		}

		float randomUnit(uint32_t &state)
		{
			return static_cast<float>(nextRandom(state) >> 8) * (1.0f / 16777215.0f);
		}

		float randomRange(uint32_t &state, float min_value, float max_value)
		{
			return min_value + ((max_value - min_value) * randomUnit(state));
		}

		Vec3 randomVector(uint32_t &state, const Vec3 &min_value, const Vec3 &max_value)
		{
			return {
				randomRange(state, min_value.x, max_value.x),
				randomRange(state, min_value.y, max_value.y),
				randomRange(state, min_value.z, max_value.z),
			};
		}

		Vec3 boundsSteering(const Boid &boid, const FlockConfig &config)
		{
			Vec3 steering = {0.0f, 0.0f, 0.0f};

			if (boid.position.x < config.bounds_min.x + config.bounds_margin)
			{
				steering.x += config.bounds_strength;
			}
			else if (boid.position.x > config.bounds_max.x - config.bounds_margin)
			{
				steering.x -= config.bounds_strength;
			}

			if (boid.position.y < config.bounds_min.y + config.bounds_margin)
			{
				steering.y += config.bounds_strength;
			}
			else if (boid.position.y > config.bounds_max.y - config.bounds_margin)
			{
				steering.y -= config.bounds_strength;
			}

			if (boid.position.z < config.bounds_min.z + config.bounds_margin)
			{
				steering.z += config.bounds_strength;
			}
			else if (boid.position.z > config.bounds_max.z - config.bounds_margin)
			{
				steering.z -= config.bounds_strength;
			}

			return steering;
		}

		Vec3 clampPosition(const Vec3 &position, const FlockConfig &config)
		{
			return {
				std::max(config.bounds_min.x, std::min(config.bounds_max.x, position.x)),
				std::max(config.bounds_min.y, std::min(config.bounds_max.y, position.y)),
				std::max(config.bounds_min.z, std::min(config.bounds_max.z, position.z)),
			};
		}

	} // namespace

	void Flock::init(uint32_t seed, const FlockConfig &config)
	{
		config_ = config;
		active_count_ = std::min(config_.boid_count, FlockConfig::kMaxBoids);

		uint32_t random_state = seed == 0u ? 1u : seed;
		for (std::size_t i = 0; i < active_count_; ++i)
		{
			const Vec3 direction = randomVector(random_state, {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
			const float speed = randomRange(random_state, config_.min_speed, config_.max_speed);

			boids_[i].position = randomVector(random_state, config_.bounds_min, config_.bounds_max);
			boids_[i].velocity = (direction * speed).clamp_magnitude(config_.min_speed, config_.max_speed);
			next_velocities_[i] = boids_[i].velocity;
		}
	}

	void Flock::update(uint32_t dt_ms)
	{
		if (active_count_ == 0u || dt_ms == 0u)
		{
			return;
		}

		const float dt_seconds = static_cast<float>(dt_ms) * 0.001f;
		const float neighbor_radius_sq = config_.neighbor_radius * config_.neighbor_radius;
		const float separation_radius_sq = config_.separation_radius * config_.separation_radius;

		for (std::size_t i = 0; i < active_count_; ++i)
		{
			Vec3 separation = {0.0f, 0.0f, 0.0f};
			Vec3 alignment = {0.0f, 0.0f, 0.0f};
			Vec3 cohesion = {0.0f, 0.0f, 0.0f};
			std::size_t neighbor_count = 0;

			for (std::size_t j = 0; j < active_count_; ++j)
			{
				if (i == j)
				{
					continue;
				}

				const Vec3 offset = boids_[j].position - boids_[i].position;
				const float distance_sq = lengthSquared(offset);
				if (distance_sq > neighbor_radius_sq)
				{
					continue;
				}

				alignment = alignment + boids_[j].velocity;
				cohesion = cohesion + boids_[j].position;
				++neighbor_count;

				if (distance_sq < separation_radius_sq && distance_sq > kEpsilon)
				{
					separation = separation + (offset * (-1.0f / distance_sq));
				}
			}

			Vec3 acceleration = boundsSteering(boids_[i], config_);
			if (neighbor_count > 0u)
			{
				const float inv_neighbors = 1.0f / static_cast<float>(neighbor_count);
				alignment = (alignment * inv_neighbors) - boids_[i].velocity;
				cohesion = (cohesion * inv_neighbors) - boids_[i].position;

				acceleration += separation * config_.separation_weight;
				acceleration += alignment * config_.alignment_weight;
				acceleration += cohesion * config_.cohesion_weight;
			}

			next_velocities_[i] =
				(boids_[i].velocity + (acceleration * dt_seconds)).clamp_magnitude(config_.min_speed, config_.max_speed);
		}

		for (std::size_t i = 0; i < active_count_; ++i)
		{
			boids_[i].velocity = next_velocities_[i];
			boids_[i].position = clampPosition(
				(boids_[i].position + (boids_[i].velocity * dt_seconds)),
				config_);
		}
	}

} // namespace boids

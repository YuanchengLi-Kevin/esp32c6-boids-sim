/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/boids/classes/flock.hpp"

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

	Flock::GridCell Flock::gridCell(const Vec3 &position) const
	{
		const float inverse_cell_size = 1.0f / config_.neighbor_radius;
		return {
			static_cast<int16_t>((position.x - config_.bounds_min.x) * inverse_cell_size),
			static_cast<int16_t>((position.y - config_.bounds_min.y) * inverse_cell_size),
			static_cast<int16_t>((position.z - config_.bounds_min.z) * inverse_cell_size),
		};
	}

	std::size_t Flock::gridBucket(const GridCell &cell) const
	{
		const uint32_t hash =
			(static_cast<uint32_t>(cell.x) * 73856093u) ^
			(static_cast<uint32_t>(cell.y) * 19349663u) ^
			(static_cast<uint32_t>(cell.z) * 83492791u);
		return hash % grid_bucket_heads_.size();
	}

	void Flock::rebuildSpatialGrid()
	{
		grid_bucket_heads_.fill(kNoBoid);

		for (std::size_t i = 0; i < active_count_; ++i)
		{
			grid_cells_[i] = gridCell(boids_[i].position);
			const std::size_t bucket = gridBucket(grid_cells_[i]);
			grid_next_boids_[i] = grid_bucket_heads_[bucket];
			grid_bucket_heads_[bucket] = static_cast<int16_t>(i);
		}
	}

	void Flock::update(float dt_seconds)
	{
		if (active_count_ == 0u || dt_seconds <= 0.0f)
		{
			return;
		}

		const float neighbor_radius_sq = config_.neighbor_radius * config_.neighbor_radius;
		const float separation_radius_sq = config_.separation_radius * config_.separation_radius;
		rebuildSpatialGrid();

		for (std::size_t i = 0; i < active_count_; ++i)
		{
			Vec3 separation = {0.0f, 0.0f, 0.0f};
			Vec3 alignment = {0.0f, 0.0f, 0.0f};
			Vec3 cohesion = {0.0f, 0.0f, 0.0f};
			std::size_t neighbor_count = 0;

			const GridCell origin = grid_cells_[i];
			for (int16_t z = origin.z - 1; z <= origin.z + 1; ++z)
			{
				for (int16_t y = origin.y - 1; y <= origin.y + 1; ++y)
				{
					for (int16_t x = origin.x - 1; x <= origin.x + 1; ++x)
					{
						const GridCell cell = {x, y, z};
						int16_t candidate = grid_bucket_heads_[gridBucket(cell)];
						while (candidate != kNoBoid)
						{
							const std::size_t j = static_cast<std::size_t>(candidate);
							candidate = grid_next_boids_[j];

							const GridCell candidate_cell = grid_cells_[j];
							if (j == i ||
								candidate_cell.x != cell.x ||
								candidate_cell.y != cell.y ||
								candidate_cell.z != cell.z)
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
					}
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

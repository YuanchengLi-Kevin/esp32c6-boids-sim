/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_SNAPSHOT_HPP_
#define ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_SNAPSHOT_HPP_

#include "constants/boids_sim_constants.hpp"
#include "core/vec3/vec3.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace boids
{

	class Flock;

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

	void initRenderSnapshots();
	void publishRenderSnapshot(const Flock &flock);
	bool copyLatestRenderSnapshot(RenderSnapshot &snapshot, uint32_t last_sequence);

} // namespace boids

#endif // ESP32C6_BOIDS_SIM_FEATURES_BOIDS_BOIDS_SNAPSHOT_HPP_

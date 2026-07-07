/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/boids/boids_snapshot.hpp"

#include "features/boids/model/flock.hpp"

#include <algorithm>

#include <zephyr/kernel.h>

namespace boids
{
	namespace
	{

		constexpr std::size_t kSnapshotBufferCount = 2;

		k_mutex snapshot_mutex;
		std::array<RenderSnapshot, kSnapshotBufferCount> snapshot_buffers;
		std::size_t published_snapshot_index = 0;
		uint32_t next_snapshot_sequence = 1;
		bool initialized = false;

		void captureSnapshot(const Flock &flock, RenderSnapshot &snapshot)
		{
			const std::size_t count = std::min(flock.count(), kMaxRenderSnapshotBoids);
			snapshot.count = count;
			for (std::size_t i = 0; i < count; ++i)
			{
				const Boid &boid = flock.boid(i);
				snapshot.boids[i] = {
					boid.position,
					boid.velocity,
				};
			}
		}

	} // namespace

	void initRenderSnapshots()
	{
		if (initialized)
		{
			return;
		}

		k_mutex_init(&snapshot_mutex);
		initialized = true;
	}

	void publishRenderSnapshot(const Flock &flock)
	{
		k_mutex_lock(&snapshot_mutex, K_FOREVER);

		const std::size_t write_index = 1u - published_snapshot_index;
		captureSnapshot(flock, snapshot_buffers[write_index]);
		snapshot_buffers[write_index].sequence = next_snapshot_sequence++;
		published_snapshot_index = write_index;

		k_mutex_unlock(&snapshot_mutex);
	}

	bool copyLatestRenderSnapshot(RenderSnapshot &snapshot, uint32_t last_sequence)
	{
		k_mutex_lock(&snapshot_mutex, K_FOREVER);

		const RenderSnapshot &published_snapshot = snapshot_buffers[published_snapshot_index];
		const bool has_new_snapshot =
			published_snapshot.sequence != 0u && published_snapshot.sequence != last_sequence;
		if (has_new_snapshot)
		{
			snapshot = published_snapshot;
		}

		k_mutex_unlock(&snapshot_mutex);
		return has_new_snapshot;
	}

} // namespace boids

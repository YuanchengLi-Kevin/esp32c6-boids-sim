/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "constants/constants.hpp"
#include "features/boids/classes/flock.hpp"
#include "features/boids/services/boids_renderer.hpp"
#include "features/boids/store/render_snapshot_store.hpp"
#include "features/rendering/rendering.hpp"
#include "features/scene/bounds_outline.hpp"
#include "features/scene/scene.hpp"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(boids_sim, LOG_LEVEL_INF);

namespace
{

	K_THREAD_STACK_DEFINE(
		simulation_thread_stack,
		constants::simulation::kThreadStackSize);

	k_thread simulation_thread;

	boids::FlockConfig makeFlockConfig()
	{
		boids::FlockConfig flock_config;
		flock_config.boid_count = constants::boids::kSpawnedBoids;
		return flock_config;
	}

	void simulation_thread_start(void *, void *, void *)
	{
		boids::FlockConfig flock_config = makeFlockConfig();
		boids::Flock flock;
		flock.init(constants::boids::kSeed, flock_config);
		boids::publishRenderSnapshot(flock);

		while (true)
		{
			flock.update(constants::simulation::kStepMs);
			boids::publishRenderSnapshot(flock);
			k_sleep(K_MSEC(constants::simulation::kStepMs));
		}
	}

} // namespace

int main()
{
	LOG_INF("ESP32-C6 Boids Sim initialized on %s", CONFIG_BOARD_TARGET);

	if (!scene::init())
	{
		LOG_ERR("Rendering initialization failed");
		return 1;
	}

	const boids::FlockConfig flock_config = makeFlockConfig();
	boids::renderer::init(scene::jetScene());
	scene::bounds_outline::init({
		flock_config.bounds_min.x,
		flock_config.bounds_min.y,
		flock_config.bounds_min.z,
		flock_config.bounds_max.x,
		flock_config.bounds_max.y,
		flock_config.bounds_max.z,
	});

	boids::initRenderSnapshots();

	k_thread_create(
		&simulation_thread,
		simulation_thread_stack,
		K_THREAD_STACK_SIZEOF(simulation_thread_stack),
		simulation_thread_start,
		nullptr,
		nullptr,
		nullptr,
		constants::simulation::kThreadPriority,
		0,
		K_NO_WAIT);
	k_thread_name_set(&simulation_thread, "boids_sim");

	rendering::init();

	return 0;
}

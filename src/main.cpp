/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "constants/constants.hpp"
#include "features/boids/classes/flock.hpp"
#include "features/boids/services/boids_renderer.hpp"
#include "features/boids/store/render_snapshot_store.hpp"
#include "features/rendering/rendering.hpp"
#include "features/simulation/simulation.hpp"
#include "features/scene/bounds_outline.hpp"
#include "features/scene/scene.hpp"

#include <zephyr/kernel.h>

int main()
{
	printk("ESP32-C6 Boids Sim initialized on %s\n", CONFIG_BOARD_TARGET);

	if (!scene::init())
	{
		printk("Rendering initialization failed\n");
		return 1;
	}

	boids::FlockConfig flock_config;
	flock_config.boid_count = constants::boids::kSpawnedBoids;

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

	simulation::init();
	rendering::init();

	k_sleep(K_FOREVER);
	return 0;
}

/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "constants/boids_sim_constants.hpp"
#include "features/boids/boids.hpp"
#include "features/boids/boids_renderer.hpp"
#include "features/scene/bounds_outline.hpp"
#include "features/scene/scene.hpp"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(boids_sim, LOG_LEVEL_INF);

int main()
{
	LOG_INF("ESP32-C6 Boids Sim initialized on %s", CONFIG_BOARD_TARGET);

	boids::FlockConfig flock_config;
	flock_config.boid_count = constants::boids::kSpawnedBoids;

	boids::Flock flock;
	flock.init(constants::boids::kSeed, flock_config);

	if (!scene::init())
	{
		LOG_ERR("Rendering initialization failed");
		return 1;
	}

	boids::renderer::init(scene::jetScene());
	scene::bounds_outline::init({
		flock_config.bounds_min.x,
		flock_config.bounds_min.y,
		flock_config.bounds_min.z,
		flock_config.bounds_max.x,
		flock_config.bounds_max.y,
		flock_config.bounds_max.z,
	});

	uint32_t previous_ms = k_uptime_get_32();
	while (true)
	{
		const uint32_t now_ms = k_uptime_get_32();
		uint32_t dt_ms = now_ms - previous_ms;
		previous_ms = now_ms;
		if (dt_ms > constants::frame::kMaxDeltaMs)
		{
			dt_ms = constants::frame::kMaxDeltaMs;
		}

		flock.update(dt_ms);
		boids::renderer::update(flock);
		scene::renderScene();
		scene::bounds_outline::draw(scene::camera(), scene::framebufferView());
		scene::flush();
		k_sleep(K_MSEC(constants::frame::kDelayMs));
	}
	return 0;
}

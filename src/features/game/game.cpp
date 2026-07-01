/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "game.hpp"

#include "features/boids/boids.hpp"
#include "features/rendering/rendering.hpp"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(game, LOG_LEVEL_INF);

namespace game {

void start()
{
	LOG_INF("ESP32-C6 Jet Game initialized on %s", CONFIG_BOARD_TARGET);

	boids::FlockConfig flock_config;
	flock_config.boid_count = rendering::kMaxRenderedBoids;

	boids::Flock flock;
	flock.init(0xC0FFEEu, flock_config);

	if (!rendering::init(flock)) {
		LOG_ERR("Rendering initialization failed");
		return;
	}

	uint32_t previous_ms = k_uptime_get_32();
	while (true) {
		const uint32_t now_ms = k_uptime_get_32();
		uint32_t dt_ms = now_ms - previous_ms;
		previous_ms = now_ms;
		if (dt_ms > 50u) {
			dt_ms = 50u;
		}

		flock.update(dt_ms);
		rendering::renderFrame(flock);
		k_sleep(K_MSEC(16));
	}
}

}  // namespace game

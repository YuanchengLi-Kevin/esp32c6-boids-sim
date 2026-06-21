/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "game.hpp"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(game, LOG_LEVEL_INF);

namespace game {

void start()
{
	LOG_INF("ESP32-C6 Jet Game initialized on %s", CONFIG_BOARD_TARGET);
}

}  // namespace game

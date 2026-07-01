/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_CONFIG_JET_CONFIG_HPP_
#define ESP32C6_BOIDS_SIM_CONFIG_JET_CONFIG_HPP_

/*
 * Jet keeps configuration in the consuming frontend. Start with the
 * upstream defaults; project-specific tuning can override them here as the
 * renderer and display features are integrated.
 */
#include "JetConfig.example.hpp"

/*
 * Keep the first Zephyr display path simple: one half-width framebuffer for
 * the full screen. The upload code expands each stored pixel horizontally.
 */
#undef FIELD_BUFFERS
#define FIELD_BUFFERS 0

#undef SSR_FIELD_REFLECT
#define SSR_FIELD_REFLECT 0

#endif  // ESP32C6_BOIDS_SIM_CONFIG_JET_CONFIG_HPP_

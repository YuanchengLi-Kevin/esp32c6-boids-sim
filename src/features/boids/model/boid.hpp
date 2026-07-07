/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_FEATURES_BOIDS_MODEL_BOID_HPP_
#define ESP32C6_BOIDS_SIM_FEATURES_BOIDS_MODEL_BOID_HPP_

#include "core/vec3/vec3.hpp"

namespace boids
{

	struct Boid
	{
		Vec3 position;
		Vec3 velocity;
	};

} // namespace boids

#endif // ESP32C6_BOIDS_SIM_FEATURES_BOIDS_MODEL_BOID_HPP_

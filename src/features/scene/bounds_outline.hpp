/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_FEATURES_SCENE_BOUNDS_OUTLINE_HPP_
#define ESP32C6_BOIDS_SIM_FEATURES_SCENE_BOUNDS_OUTLINE_HPP_

#include "Camera.hpp"
#include "core/rendering/rendering.hpp"

namespace scene::bounds_outline
{

	struct Bounds
	{
		float min_x;
		float min_y;
		float min_z;
		float max_x;
		float max_y;
		float max_z;
	};

	void init(const Bounds &bounds);
	void draw(const Renderer::Camera &camera, const core::rendering::FramebufferView &framebuffer);

} // namespace scene::bounds_outline

#endif // ESP32C6_BOIDS_SIM_FEATURES_SCENE_BOUNDS_OUTLINE_HPP_

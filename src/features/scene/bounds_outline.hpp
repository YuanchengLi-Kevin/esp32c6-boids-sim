/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

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

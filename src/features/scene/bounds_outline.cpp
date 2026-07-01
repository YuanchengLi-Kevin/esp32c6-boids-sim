/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/scene/bounds_outline.hpp"

#include "constants/boids_sim_constants.hpp"

#include <array>
#include <cstdint>

namespace scene::bounds_outline
{
	namespace
	{

		Bounds active_bounds{};

	} // namespace

	void init(const Bounds &bounds)
	{
		active_bounds = bounds;
	}

	void draw(const Renderer::Camera &camera, const core::rendering::FramebufferView &framebuffer)
	{
		const std::array<Vector3, 8> corners = {{
			{
				core::rendering::toWorld(active_bounds.min_x),
				core::rendering::toWorld(active_bounds.min_y),
				core::rendering::toWorld(active_bounds.min_z),
			},
			{
				core::rendering::toWorld(active_bounds.max_x),
				core::rendering::toWorld(active_bounds.min_y),
				core::rendering::toWorld(active_bounds.min_z),
			},
			{
				core::rendering::toWorld(active_bounds.max_x),
				core::rendering::toWorld(active_bounds.max_y),
				core::rendering::toWorld(active_bounds.min_z),
			},
			{
				core::rendering::toWorld(active_bounds.min_x),
				core::rendering::toWorld(active_bounds.max_y),
				core::rendering::toWorld(active_bounds.min_z),
			},
			{
				core::rendering::toWorld(active_bounds.min_x),
				core::rendering::toWorld(active_bounds.min_y),
				core::rendering::toWorld(active_bounds.max_z),
			},
			{
				core::rendering::toWorld(active_bounds.max_x),
				core::rendering::toWorld(active_bounds.min_y),
				core::rendering::toWorld(active_bounds.max_z),
			},
			{
				core::rendering::toWorld(active_bounds.max_x),
				core::rendering::toWorld(active_bounds.max_y),
				core::rendering::toWorld(active_bounds.max_z),
			},
			{
				core::rendering::toWorld(active_bounds.min_x),
				core::rendering::toWorld(active_bounds.max_y),
				core::rendering::toWorld(active_bounds.max_z),
			},
		}};
		const std::array<core::rendering::ScreenPoint, 8> projected = {{
			core::rendering::projectWorldPoint(camera, corners[0]),
			core::rendering::projectWorldPoint(camera, corners[1]),
			core::rendering::projectWorldPoint(camera, corners[2]),
			core::rendering::projectWorldPoint(camera, corners[3]),
			core::rendering::projectWorldPoint(camera, corners[4]),
			core::rendering::projectWorldPoint(camera, corners[5]),
			core::rendering::projectWorldPoint(camera, corners[6]),
			core::rendering::projectWorldPoint(camera, corners[7]),
		}};
		constexpr std::array<std::array<int, 2>, 12> edges = {{
			{{0, 1}},
			{{1, 2}},
			{{2, 3}},
			{{3, 0}},
			{{4, 5}},
			{{5, 6}},
			{{6, 7}},
			{{7, 4}},
			{{0, 4}},
			{{1, 5}},
			{{2, 6}},
			{{3, 7}},
		}};

		for (const auto &edge : edges)
		{
			core::rendering::drawLine(
				framebuffer,
				projected[edge[0]],
				projected[edge[1]],
				constants::colors::kBoundsOutline);
		}
	}

} // namespace scene::bounds_outline

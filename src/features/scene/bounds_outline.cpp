/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/scene/bounds_outline.hpp"

#include "constants/boids_sim_constants.hpp"

#include <array>
#include <cstdint>

using std::array;
using std::size_t;

namespace scene::bounds_outline
{

	namespace rendering = core::rendering;

	namespace
	{

		Bounds active_bounds{};

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

	} // namespace

	void init(const Bounds &bounds)
	{
		active_bounds = bounds;
	}

	void draw(const Renderer::Camera &camera, const rendering::FramebufferView &framebuffer)
	{
		const array<Vector3, 8> corners = {{
			rendering::toWorld(active_bounds.min_x, active_bounds.min_y, active_bounds.min_z),
			rendering::toWorld(active_bounds.max_x, active_bounds.min_y, active_bounds.min_z),
			rendering::toWorld(active_bounds.max_x, active_bounds.max_y, active_bounds.min_z),
			rendering::toWorld(active_bounds.min_x, active_bounds.max_y, active_bounds.min_z),
			rendering::toWorld(active_bounds.min_x, active_bounds.min_y, active_bounds.max_z),
			rendering::toWorld(active_bounds.max_x, active_bounds.min_y, active_bounds.max_z),
			rendering::toWorld(active_bounds.max_x, active_bounds.max_y, active_bounds.max_z),
			rendering::toWorld(active_bounds.min_x, active_bounds.max_y, active_bounds.max_z),
		}};

		array<rendering::ScreenPoint, corners.size()> projected{};
		for (size_t i = 0; i < corners.size(); ++i)
		{
			projected[i] = rendering::projectWorldPoint(camera, corners[i]);
		}

		for (const auto &edge : edges)
		{
			rendering::drawLine(
				framebuffer,
				projected[edge[0]],
				projected[edge[1]],
				constants::colors::kBoundsOutline);
		}
	}

} // namespace scene::bounds_outline

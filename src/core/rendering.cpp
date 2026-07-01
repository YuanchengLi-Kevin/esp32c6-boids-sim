/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "core/rendering.hpp"

#include <cstdlib>

namespace core::rendering
{

	int32_t toWorld(float value)
	{
		return static_cast<int32_t>(value * constants::world::kScale);
	}

	ScreenPoint projectWorldPoint(const Renderer::Camera &camera, const Vector3 &point)
	{
		int32_t cos_x;
		int32_t sin_x;
		int32_t cos_y;
		int32_t sin_y;
		int32_t cos_z;
		int32_t sin_z;
		camera.getRotationMatrix(cos_x, sin_x, cos_y, sin_y, cos_z, sin_z);

		Vector3 view = point - camera.position;
		view.assign(
			(static_cast<int64_t>(view.x) * cos_y + static_cast<int64_t>(view.z) * sin_y) /
				FIXED_POINT_SCALE,
			view.y,
			(-static_cast<int64_t>(view.x) * sin_y + static_cast<int64_t>(view.z) * cos_y) /
				FIXED_POINT_SCALE);
		view.assign(
			view.x,
			(static_cast<int64_t>(view.y) * cos_x - static_cast<int64_t>(view.z) * sin_x) /
				FIXED_POINT_SCALE,
			(static_cast<int64_t>(view.y) * sin_x + static_cast<int64_t>(view.z) * cos_x) /
				FIXED_POINT_SCALE);
		view.assign(
			(static_cast<int64_t>(view.x) * cos_z - static_cast<int64_t>(view.y) * sin_z) /
				FIXED_POINT_SCALE,
			(static_cast<int64_t>(view.x) * sin_z + static_cast<int64_t>(view.y) * cos_z) /
				FIXED_POINT_SCALE,
			view.z);

		if (view.z < camera.nearPlane || view.z > camera.farPlane)
		{
			return {0, 0, false};
		}

		const float inv_z = camera.fovFactor / static_cast<float>(view.z);
		return {
			static_cast<int>(static_cast<float>(view.x) * inv_z) + kScreenWidth / 2,
			kScreenHeight / 2 - static_cast<int>(static_cast<float>(view.y) * inv_z),
			true,
		};
	}

	void plotPixel(const FramebufferView &framebuffer, int x, int y, uint16_t color)
	{
		if (x < 0 || x >= framebuffer.screen_width || y < 0 || y >= framebuffer.screen_height)
		{
			return;
		}

		framebuffer.pixels[y * framebuffer.framebuffer_width + (x >> 1)] = color;
	}

	void drawLine(const FramebufferView &framebuffer, ScreenPoint a, ScreenPoint b, uint16_t color)
	{
		if (!a.visible || !b.visible)
		{
			return;
		}

		int x0 = a.x;
		int y0 = a.y;
		const int x1 = b.x;
		const int y1 = b.y;
		const int dx = std::abs(x1 - x0);
		const int sx = x0 < x1 ? 1 : -1;
		const int dy = -std::abs(y1 - y0);
		const int sy = y0 < y1 ? 1 : -1;
		int err = dx + dy;

		while (true)
		{
			plotPixel(framebuffer, x0, y0, color);
			if (x0 == x1 && y0 == y1)
			{
				break;
			}

			const int e2 = 2 * err;
			if (e2 >= dy)
			{
				err += dy;
				x0 += sx;
			}
			if (e2 <= dx)
			{
				err += dx;
				y0 += sy;
			}
		}
	}

} // namespace core::rendering

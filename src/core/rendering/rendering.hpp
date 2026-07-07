/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_CORE_RENDERING_HPP_
#define ESP32C6_BOIDS_SIM_CORE_RENDERING_HPP_

#include "Camera.hpp"
#include "constants/constants.hpp"

#include <cstdint>

namespace core::rendering
{

	inline constexpr int kScreenWidth = constants::display::kScreenWidth;
	inline constexpr int kScreenHeight = constants::display::kScreenHeight;
	inline constexpr int kFramebufferWidth = constants::display::kFramebufferWidth;

	struct FramebufferView
	{
		uint16_t *pixels;
		int screen_width;
		int screen_height;
		int framebuffer_width;
	};

	struct ScreenPoint
	{
		int x;
		int y;
		bool visible;
	};

	Vector3 toWorld(float x, float y, float z);
	ScreenPoint projectWorldPoint(const Renderer::Camera &camera, const Vector3 &point);
	void plotPixel(const FramebufferView &framebuffer, int x, int y, uint16_t color);
	void drawLine(const FramebufferView &framebuffer, ScreenPoint a, ScreenPoint b, uint16_t color);

} // namespace core::rendering

#endif // ESP32C6_BOIDS_SIM_CORE_RENDERING_HPP_

/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "scene.hpp"

#include "constants/boids_sim_constants.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

using std::array;

LOG_MODULE_REGISTER(scene, LOG_LEVEL_INF);

namespace scene
{
	namespace rendering = core::rendering;
	namespace
	{

		static_assert(HALF_WIDTH_BUFFERS == 1, "Display upload expects Jet half-width buffers.");
		static_assert(FIELD_BUFFERS == 0, "Display upload expects a full-height framebuffer.");

#define DISPLAY_NODE DT_CHOSEN(zephyr_display)

#if !DT_NODE_HAS_STATUS(DISPLAY_NODE, okay)
#error "No ready zephyr,display chosen node"
#endif
		const device *display_device = DEVICE_DT_GET(DISPLAY_NODE);

		array<uint16_t, rendering::kFramebufferWidth * rendering::kScreenHeight> framebuffer;
		array<uint16_t, rendering::kFramebufferWidth * rendering::kScreenHeight> previous_framebuffer;
		array<uint16_t, rendering::kScreenWidth * constants::display::kUploadRows> upload_buffer;
		array<int, rendering::kScreenHeight> dirty_min_x;
		array<int, rendering::kScreenHeight> dirty_max_x;

		Renderer::Scene *active_scene = nullptr;
		Renderer::Camera active_camera;
		bool initialized = false;
		bool full_refresh_pending = true;

		bool writeDisplayRegion(int x, int y, int width, int height)
		{
			display_buffer_descriptor descriptor = {
				.buf_size = static_cast<size_t>(width * height) * sizeof(upload_buffer[0]),
				.width = static_cast<uint16_t>(width),
				.height = static_cast<uint16_t>(height),
				.pitch = static_cast<uint16_t>(width),
			};

			const int err = display_write(display_device, x, y, &descriptor, upload_buffer.data());
			if (err != 0)
			{
				LOG_ERR("display_write failed at (%d, %d) %dx%d: %d", x, y, width, height, err);
				return false;
			}

			return true;
		}

		bool flushFullDisplay()
		{
			for (int y = 0; y < rendering::kScreenHeight; y += constants::display::kUploadRows)
			{
				const int rows = std::min(constants::display::kUploadRows, rendering::kScreenHeight - y);
				for (int row = 0; row < rows; ++row)
				{
					const uint16_t *src =
						&framebuffer[(y + row) * rendering::kFramebufferWidth];
					uint16_t *dst = &upload_buffer[row * rendering::kScreenWidth];
					for (int x = 0; x < rendering::kFramebufferWidth; ++x)
					{
						const uint16_t color = src[x];
						dst[(x * 2)] = color;
						dst[(x * 2) + 1] = color;
					}
				}

				if (!writeDisplayRegion(0, y, rendering::kScreenWidth, rows))
				{
					return false;
				}
			}

			std::copy(framebuffer.begin(), framebuffer.end(), previous_framebuffer.begin());
			return true;
		}

		void findDirtyRows()
		{
			dirty_min_x.fill(rendering::kFramebufferWidth);
			dirty_max_x.fill(-1);

			for (int y = 0; y < rendering::kScreenHeight; ++y)
			{
				const int row_offset = y * rendering::kFramebufferWidth;
				int first = 0;
				while (first < rendering::kFramebufferWidth &&
					   framebuffer[row_offset + first] == previous_framebuffer[row_offset + first])
				{
					++first;
				}

				if (first == rendering::kFramebufferWidth)
				{
					continue;
				}

				int last = rendering::kFramebufferWidth - 1;
				while (last > first &&
					   framebuffer[row_offset + last] == previous_framebuffer[row_offset + last])
				{
					--last;
				}

				dirty_min_x[y] = first;
				dirty_max_x[y] = last;
			}
		}

		bool flushDirtyDisplay()
		{
			findDirtyRows();

			for (int y = 0; y < rendering::kScreenHeight;)
			{
				if (dirty_max_x[y] < 0)
				{
					++y;
					continue;
				}

				const int start_y = y;
				int rows = 1;
				int min_x = dirty_min_x[y];
				int max_x = dirty_max_x[y];
				++y;

				while (y < rendering::kScreenHeight &&
					   rows < constants::display::kUploadRows &&
					   dirty_max_x[y] >= 0)
				{
					min_x = std::min(min_x, dirty_min_x[y]);
					max_x = std::max(max_x, dirty_max_x[y]);
					++rows;
					++y;
				}

				const int span_half_width = max_x - min_x + 1;
				const int span_display_width = span_half_width * 2;
				for (int row = 0; row < rows; ++row)
				{
					const uint16_t *src =
						&framebuffer[(start_y + row) * rendering::kFramebufferWidth + min_x];
					uint16_t *dst = &upload_buffer[row * span_display_width];
					for (int x = 0; x < span_half_width; ++x)
					{
						const uint16_t color = src[x];
						dst[x * 2] = color;
						dst[(x * 2) + 1] = color;
					}
				}

				if (!writeDisplayRegion(min_x * 2, start_y, span_display_width, rows))
				{
					return false;
				}

				for (int row = 0; row < rows; ++row)
				{
					const int row_offset = (start_y + row) * rendering::kFramebufferWidth;
					std::copy_n(
						&framebuffer[row_offset + min_x],
						span_half_width,
						&previous_framebuffer[row_offset + min_x]);
				}
			}

			return true;
		}

		bool flushDisplay()
		{
			if (full_refresh_pending)
			{
				if (!flushFullDisplay())
				{
					return false;
				}
				full_refresh_pending = false;
				return true;
			}

			return flushDirtyDisplay();
		}
	} // namespace

	bool init()
	{
		if (initialized)
		{
			return true;
		}

		if (!device_is_ready(display_device))
		{
			LOG_ERR("Display device is not ready");
			return false;
		}

		const int pixel_format_err = display_set_pixel_format(display_device, PIXEL_FORMAT_RGB_565);
		if (pixel_format_err != 0)
		{
			LOG_WRN("display_set_pixel_format failed: %d", pixel_format_err);
		}

		const int blanking_err = display_blanking_off(display_device);
		if (blanking_err != 0)
		{
			LOG_WRN("display_blanking_off failed: %d", blanking_err);
		}

		static Renderer::Scene scene_storage(
			framebuffer.data(),
			nullptr,
			rendering::kScreenWidth,
			rendering::kScreenHeight);

		active_scene = &scene_storage;
		active_scene->setBackcolor(constants::colors::kBackground);
		active_scene->setClearBuffer(true);
		active_scene->getObjects().reserve(constants::rendering::kInitialSceneObjects);

		active_camera.setPosition(
			constants::camera::kPositionX,
			constants::camera::kPositionY,
			constants::camera::kPositionZ);
		active_camera.lookAt({
			constants::camera::kTargetX,
			constants::camera::kTargetY,
			constants::camera::kTargetZ,
		});
		active_camera.setFOV(constants::camera::kFovDegrees, core::rendering::kScreenWidth);
		active_camera.nearPlane = constants::camera::kNearPlane;
		active_camera.farPlane = constants::camera::kFarPlane;
		active_scene->setCamera(&active_camera);

		initialized = true;
		LOG_INF(
			"Rendering initialized: %dx%d half-width framebuffer",
			rendering::kScreenWidth,
			rendering::kScreenHeight);
		return true;
	}

	Renderer::Scene &jetScene()
	{
		return *active_scene;
	}

	Renderer::Camera &camera()
	{
		return active_camera;
	}

	rendering::FramebufferView framebufferView()
	{
		return {
			framebuffer.data(),
			rendering::kScreenWidth,
			rendering::kScreenHeight,
			rendering::kFramebufferWidth,
		};
	}

	void renderScene()
	{
		if (initialized)
		{
			active_scene->render();
		}
	}

	void flush()
	{
		if (initialized)
		{
			(void)flushDisplay();
		}
	}

} // namespace scene

/*
 * Copyright (c) 2026 Yuancheng Li
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rendering.hpp"

#include "Camera.hpp"
#include "Material.hpp"
#include "Object.hpp"
#include "Scene.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(rendering, LOG_LEVEL_INF);

namespace rendering {
namespace {

constexpr int kScreenWidth = 240;
constexpr int kScreenHeight = 320;
constexpr int kFramebufferWidth = kScreenWidth / 2;
constexpr int kUploadRows = 8;
constexpr uint16_t kBackColor = 0x0008;

static_assert(HALF_WIDTH_BUFFERS == 1, "Display upload expects Jet half-width buffers.");
static_assert(FIELD_BUFFERS == 0, "Display upload expects a full-height framebuffer.");

#define DISPLAY_NODE DT_CHOSEN(zephyr_display)

#if !DT_NODE_HAS_STATUS(DISPLAY_NODE, okay)
#error "No ready zephyr,display chosen node"
#endif

const device* display_device = DEVICE_DT_GET(DISPLAY_NODE);

std::array<uint16_t, kFramebufferWidth * kScreenHeight> framebuffer;
std::array<uint16_t, kFramebufferWidth * kScreenHeight> previous_framebuffer;
std::array<uint16_t, kScreenWidth * kUploadRows> upload_buffer;
std::array<int, kScreenHeight> dirty_min_x;
std::array<int, kScreenHeight> dirty_max_x;
std::array<Renderer::Object, kMaxRenderedBoids> boid_objects;

Renderer::Scene* scene = nullptr;
Renderer::Camera camera;
Renderer::Material boid_material(0x07FF);
bool initialized = false;
bool full_refresh_pending = true;

int32_t toWorld(float value)
{
	return static_cast<int32_t>(value * 4.0f);
}

void buildBoidMesh(Renderer::Object& object)
{
	using Renderer::CullingMode;
	using Renderer::Object;

	object.vertices.reserve(4);
	object.triangles.reserve(4);

	const Vector2 uv = {0, 0};
	const Vector3 nose = {0, 0, 28};
	const Vector3 left = {-8, -5, -12};
	const Vector3 right = {8, -5, -12};
	const Vector3 top = {0, 9, -8};

	object.addVertex({nose, uv, {0, 0, FIXED_POINT_SCALE}});
	object.addVertex({left, uv, {0, 0, FIXED_POINT_SCALE}});
	object.addVertex({right, uv, {0, 0, FIXED_POINT_SCALE}});
	object.addVertex({top, uv, {0, 0, FIXED_POINT_SCALE}});

	object.addTriangle(0, 1, 3, &boid_material);
	object.addTriangle(0, 3, 2, &boid_material);
	object.addTriangle(0, 2, 1, &boid_material);
	object.addTriangle(1, 2, 3, &boid_material);
	object.computeFlatNormals();
	object.calculateBoundingBox();
	object.cullingMode = CullingMode::NO_CULLING;
}

void updateBoidObjects(const boids::Flock& flock)
{
	const std::size_t count = std::min(flock.count(), kMaxRenderedBoids);
	for (std::size_t i = 0; i < kMaxRenderedBoids; ++i) {
		boid_objects[i].enabled = i < count;
		if (i >= count) {
			continue;
		}

		const boids::Boid& boid = flock.boid(i);
		const Vector3 position = {
			toWorld(boid.position.x),
			toWorld(boid.position.y),
			toWorld(boid.position.z),
		};
		const Vector3 target = {
			toWorld(boid.position.x + boid.velocity.x),
			toWorld(boid.position.y + boid.velocity.y),
			toWorld(boid.position.z + boid.velocity.z),
		};

		boid_objects[i].setPosition(position);
		boid_objects[i].lookAt(target);
	}
}

bool writeDisplayRegion(int x, int y, int width, int height)
{
	display_buffer_descriptor descriptor = {
		.buf_size = static_cast<size_t>(width * height) * sizeof(upload_buffer[0]),
		.width = static_cast<uint16_t>(width),
		.height = static_cast<uint16_t>(height),
		.pitch = static_cast<uint16_t>(width),
	};

	const int err = display_write(display_device, x, y, &descriptor, upload_buffer.data());
	if (err != 0) {
		LOG_ERR("display_write failed at (%d, %d) %dx%d: %d", x, y, width, height, err);
		return false;
	}

	return true;
}

bool flushFullDisplay()
{
	for (int y = 0; y < kScreenHeight; y += kUploadRows) {
		const int rows = std::min(kUploadRows, kScreenHeight - y);
		for (int row = 0; row < rows; ++row) {
			const uint16_t* src = &framebuffer[(y + row) * kFramebufferWidth];
			uint16_t* dst = &upload_buffer[row * kScreenWidth];
			for (int x = 0; x < kFramebufferWidth; ++x) {
				const uint16_t color = src[x];
				dst[(x * 2)] = color;
				dst[(x * 2) + 1] = color;
			}
		}

		if (!writeDisplayRegion(0, y, kScreenWidth, rows)) {
			return false;
		}
	}

	std::copy(framebuffer.begin(), framebuffer.end(), previous_framebuffer.begin());
	return true;
}

void findDirtyRows()
{
	dirty_min_x.fill(kFramebufferWidth);
	dirty_max_x.fill(-1);

	for (int y = 0; y < kScreenHeight; ++y) {
		const int row_offset = y * kFramebufferWidth;
		int first = 0;
		while (first < kFramebufferWidth &&
		       framebuffer[row_offset + first] == previous_framebuffer[row_offset + first]) {
			++first;
		}

		if (first == kFramebufferWidth) {
			continue;
		}

		int last = kFramebufferWidth - 1;
		while (last > first &&
		       framebuffer[row_offset + last] == previous_framebuffer[row_offset + last]) {
			--last;
		}

		dirty_min_x[y] = first;
		dirty_max_x[y] = last;
	}
}

bool flushDirtyDisplay()
{
	findDirtyRows();

	for (int y = 0; y < kScreenHeight;) {
		if (dirty_max_x[y] < 0) {
			++y;
			continue;
		}

		const int start_y = y;
		int rows = 1;
		int min_x = dirty_min_x[y];
		int max_x = dirty_max_x[y];
		++y;

		while (y < kScreenHeight && rows < kUploadRows && dirty_max_x[y] >= 0) {
			min_x = std::min(min_x, dirty_min_x[y]);
			max_x = std::max(max_x, dirty_max_x[y]);
			++rows;
			++y;
		}

		const int span_half_width = max_x - min_x + 1;
		const int span_display_width = span_half_width * 2;
		for (int row = 0; row < rows; ++row) {
			const uint16_t* src = &framebuffer[(start_y + row) * kFramebufferWidth + min_x];
			uint16_t* dst = &upload_buffer[row * span_display_width];
			for (int x = 0; x < span_half_width; ++x) {
				const uint16_t color = src[x];
				dst[x * 2] = color;
				dst[(x * 2) + 1] = color;
			}
		}

		if (!writeDisplayRegion(min_x * 2, start_y, span_display_width, rows)) {
			return false;
		}

		for (int row = 0; row < rows; ++row) {
			const int row_offset = (start_y + row) * kFramebufferWidth;
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
	if (full_refresh_pending) {
		if (!flushFullDisplay()) {
			return false;
		}
		full_refresh_pending = false;
		return true;
	}

	return flushDirtyDisplay();
}

}  // namespace

bool init(const boids::Flock& flock)
{
	if (initialized) {
		return true;
	}

	if (!device_is_ready(display_device)) {
		LOG_ERR("Display device is not ready");
		return false;
	}

	const int pixel_format_err = display_set_pixel_format(display_device, PIXEL_FORMAT_RGB_565);
	if (pixel_format_err != 0) {
		LOG_WRN("display_set_pixel_format failed: %d", pixel_format_err);
	}

	const int blanking_err = display_blanking_off(display_device);
	if (blanking_err != 0) {
		LOG_WRN("display_blanking_off failed: %d", blanking_err);
	}

	static Renderer::Scene scene_storage(framebuffer.data(), nullptr, kScreenWidth, kScreenHeight);
	scene = &scene_storage;
	scene->setBackcolor(kBackColor);
	scene->setClearBuffer(true);
	scene->getObjects().reserve(kMaxRenderedBoids);

	camera.setPosition(0, 70, -760);
	camera.lookAt({0, 0, 0});
	camera.setFOV(70, kScreenWidth);
	camera.nearPlane = 16;
	camera.farPlane = 2000;
	scene->setCamera(&camera);

	boid_material.emissive = true;
	boid_material.shadingMode = Renderer::ShadingMode::UNLIT;

	for (Renderer::Object& object : boid_objects) {
		buildBoidMesh(object);
		scene->addObject(&object);
	}

	updateBoidObjects(flock);
	initialized = true;
	LOG_INF("Rendering initialized: %dx%d half-width framebuffer", kScreenWidth, kScreenHeight);
	return true;
}

void renderFrame(const boids::Flock& flock)
{
	if (!initialized && !init(flock)) {
		return;
	}

	updateBoidObjects(flock);
	scene->render();
	(void)flushDisplay();
}

}  // namespace rendering

/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/rendering/rendering.hpp"

#include "constants/constants.hpp"
#include "features/boids/services/boids_renderer.hpp"
#include "features/boids/store/render_snapshot_store.hpp"
#include "features/scene/bounds_outline.hpp"
#include "features/scene/scene.hpp"
#include "features/profiling/profiling.hpp"

#include <cstdint>

#include <zephyr/kernel.h>

namespace rendering
{
	namespace
	{

		K_THREAD_STACK_DEFINE(
			render_thread_stack,
			constants::render_thread::kThreadStackSize);

		k_thread render_thread;
		bool initialized = false;

		void renderThreadStart(void *, void *, void *)
		{
			boids::RenderSnapshot render_snapshot;
			uint32_t last_sequence = 0;

			while (true)
			{
				boids::waitForRenderSnapshot();

				if (boids::copyLatestRenderSnapshot(render_snapshot, last_sequence))
				{
					const uint64_t frame_start = profiling::start();
					last_sequence = render_snapshot.sequence;

					const uint64_t renderer_start = profiling::start();
					boids::renderer::update(render_snapshot);
					profiling::record(profiling::Stage::RendererUpdate, renderer_start);

					scene::updateCameraOrbit();
					const uint64_t render_start = profiling::start();
					scene::renderScene();
					profiling::record(profiling::Stage::SceneRender, render_start);

					const uint64_t outline_start = profiling::start();
					scene::bounds_outline::draw(scene::camera(), scene::framebufferView());
					profiling::record(profiling::Stage::BoundsOutline, outline_start);

					const uint64_t flush_start = profiling::start();
					scene::flush();
					profiling::record(profiling::Stage::DisplayFlush, flush_start);
					profiling::record(profiling::Stage::RenderFrame, frame_start);
					profiling::reportIfDue();
				}
			}
		}

	} // namespace

	void init()
	{
		if (initialized)
		{
			return;
		}

		k_thread_create(
			&render_thread,
			render_thread_stack,
			K_THREAD_STACK_SIZEOF(render_thread_stack),
			renderThreadStart,
			nullptr,
			nullptr,
			nullptr,
			constants::render_thread::kThreadPriority,
			0,
			K_NO_WAIT);
		k_thread_name_set(&render_thread, "boids_render");
		initialized = true;
	}

} // namespace rendering

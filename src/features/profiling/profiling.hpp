/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>

namespace profiling
{
	enum class Stage : uint8_t
	{
		SimulationUpdate,
		SnapshotPublish,
		RendererUpdate,
		SceneRender,
		BoundsOutline,
		DisplayFlush,
		RenderFrame,
		Count,
	};

	void init();
	uint64_t start();
	void record(Stage stage, uint64_t start_cycles);
	void reportIfDue();
} // namespace profiling

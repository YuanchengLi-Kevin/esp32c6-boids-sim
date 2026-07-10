/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "features/boids/types/render_snapshot.hpp"

#include <cstdint>

namespace boids
{

	class Flock;

	void initRenderSnapshots();
	void publishRenderSnapshot(const Flock &flock);
	void waitForRenderSnapshot();
	bool copyLatestRenderSnapshot(RenderSnapshot &snapshot, uint32_t last_sequence);

} // namespace boids

/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "features/profiling/profiling.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

#include <zephyr/kernel.h>

namespace profiling
{
	namespace
	{
		constexpr int64_t kReportIntervalMs = 5000;
		constexpr std::array<const char *, static_cast<std::size_t>(Stage::Count)> kStageNames = {
			"simulation_update",
			"snapshot_publish",
			"renderer_update",
			"scene_render",
			"bounds_outline",
			"display_flush",
			"render_frame",
		};

		struct Measurement
		{
			uint64_t total_us = 0;
			uint64_t max_us = 0;
			uint32_t count = 0;
		};

		k_mutex metrics_mutex;
		std::array<Measurement, static_cast<std::size_t>(Stage::Count)> metrics;
		int64_t next_report_ms = 0;
	} // namespace

	void init()
	{
		k_mutex_init(&metrics_mutex);
		next_report_ms = k_uptime_get() + kReportIntervalMs;
	}

	uint64_t start()
	{
		return k_cycle_get_64();
	}

	void record(Stage stage, uint64_t start_cycles)
	{
		const uint64_t elapsed_us = k_cyc_to_us_floor64(k_cycle_get_64() - start_cycles);
		const std::size_t index = static_cast<std::size_t>(stage);

		k_mutex_lock(&metrics_mutex, K_FOREVER);
		Measurement &measurement = metrics[index];
		measurement.total_us += elapsed_us;
		measurement.max_us = elapsed_us > measurement.max_us ? elapsed_us : measurement.max_us;
		++measurement.count;
		k_mutex_unlock(&metrics_mutex);
	}

	void reportIfDue()
	{
		const int64_t now_ms = k_uptime_get();
		if (now_ms < next_report_ms)
		{
			return;
		}

		std::array<Measurement, static_cast<std::size_t>(Stage::Count)> report;
		k_mutex_lock(&metrics_mutex, K_FOREVER);
		if (now_ms < next_report_ms)
		{
			k_mutex_unlock(&metrics_mutex);
			return;
		}
		report = metrics;
		metrics = {};
		next_report_ms = now_ms + kReportIntervalMs;
		k_mutex_unlock(&metrics_mutex);

		printk("profile: stage count avg_us max_us\n");
		for (std::size_t i = 0; i < report.size(); ++i)
		{
			const Measurement &measurement = report[i];
			if (measurement.count == 0)
			{
				continue;
			}

			printk(
				"profile: %s %u %llu %llu\n",
				kStageNames[i],
				static_cast<unsigned int>(measurement.count),
				static_cast<unsigned long long>(measurement.total_us / measurement.count),
				static_cast<unsigned long long>(measurement.max_us));
		}
	}
} // namespace profiling

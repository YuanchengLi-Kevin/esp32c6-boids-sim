/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "constants/constants.hpp"
#include "features/boids/store/render_snapshot_store.hpp"
#include "features/boids/classes/flock.hpp"
#include "features/profiling/profiling.hpp"

#include <zephyr/kernel.h>

namespace simulation
{
    namespace
    {
        K_THREAD_STACK_DEFINE(
            simulation_thread_stack,
            constants::simulation::kThreadStackSize);

        k_thread simulation_thread;
        bool initialized = false;

        void simulationThreadStart(void *, void *, void *)
        {
            boids::FlockConfig flock_config;
            flock_config.boid_count = constants::boids::kSpawnedBoids;

            boids::Flock flock;

            flock.init(constants::boids::kSeed, flock_config);
            boids::publishRenderSnapshot(flock);
            printk(
                "simulation: published initial render snapshot: count=%u\n",
                static_cast<unsigned int>(flock.count()));

            const int64_t epoch_ticks = k_uptime_ticks();
            uint64_t completed_steps = 0u;

            while (true)
            {
				const uint64_t update_start = profiling::start();
                flock.update(constants::simulation::kStepSeconds);
				profiling::record(profiling::Stage::SimulationUpdate, update_start);

				const uint64_t publish_start = profiling::start();
                boids::publishRenderSnapshot(flock);
				profiling::record(profiling::Stage::SnapshotPublish, publish_start);

                ++completed_steps;
                const int64_t deadline_ticks =
                    epoch_ticks +
                    static_cast<int64_t>(
                        (completed_steps * CONFIG_SYS_CLOCK_TICKS_PER_SEC +
                         constants::simulation::kStepsPerSecond - 1u) /
                        constants::simulation::kStepsPerSecond);

                k_sleep(K_TIMEOUT_ABS_TICKS(deadline_ticks));
            }
        }
    }

    void init()
    {
        if (initialized)
        {
            return;
        }

        k_thread_create(
            &simulation_thread,
            simulation_thread_stack,
            K_THREAD_STACK_SIZEOF(simulation_thread_stack),
            simulationThreadStart,
            nullptr,
            nullptr,
            nullptr,
            constants::simulation::kThreadPriority,
            0,
            K_NO_WAIT);
        k_thread_name_set(&simulation_thread, "boids_sim");
        initialized = true;
    }
}

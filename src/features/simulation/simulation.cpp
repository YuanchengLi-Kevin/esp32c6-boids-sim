/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#include "constants/constants.hpp"
#include "features/boids/store/render_snapshot_store.hpp"
#include "features/boids/classes/flock.hpp"

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

            while (true)
            {
                flock.update(constants::simulation::kStepMs);
                boids::publishRenderSnapshot(flock);
                k_sleep(K_MSEC(constants::simulation::kStepMs));
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

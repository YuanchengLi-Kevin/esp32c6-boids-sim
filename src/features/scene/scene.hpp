/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP32C6_BOIDS_SIM_FEATURES_SCENE_SCENE_HPP_
#define ESP32C6_BOIDS_SIM_FEATURES_SCENE_SCENE_HPP_

#include <Camera.hpp>
#include <Scene.hpp>
#include "core/rendering.hpp"

namespace scene
{

    bool init();
    Renderer::Scene &jetScene();
    Renderer::Camera &camera();
    core::rendering::FramebufferView framebufferView();
    void renderScene();
    void flush();

} // namespace scene

#endif // ESP32C6_BOIDS_SIM_FEATURES_SCENE_SCENE_HPP_

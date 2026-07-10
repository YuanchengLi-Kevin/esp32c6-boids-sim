/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Camera.hpp>
#include <Scene.hpp>
#include "core/rendering/rendering.hpp"

namespace scene
{

    bool init();
    Renderer::Scene &jetScene();
    Renderer::Camera &camera();
    core::rendering::FramebufferView framebufferView();
    void updateCameraOrbit();
    void renderScene();
    void flush();

} // namespace scene

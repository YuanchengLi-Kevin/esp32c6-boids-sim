/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace constants
{

    namespace display
    {
        inline constexpr int kScreenWidth = 320;
        inline constexpr int kScreenHeight = 240;
        inline constexpr int kFramebufferWidth = kScreenWidth / 2;
        inline constexpr int kUploadRows = 8;
    } // namespace display

    namespace colors
    {
        inline constexpr uint16_t kBackground = 0x0008;
        inline constexpr uint16_t kBoid = 0x07FF;
        inline constexpr uint16_t kBoundsOutline = 0x8410;
    } // namespace colors

    namespace world
    {
        inline constexpr float kScale = 4.0f;
    } // namespace world

    namespace frame
    {
        inline constexpr uint32_t kMaxDeltaMs = 50u;
        inline constexpr uint32_t kDelayMs = 16u;
    } // namespace frame

    namespace simulation
    {
        inline constexpr uint32_t kStepsPerSecond = 30u;
        inline constexpr float kStepSeconds = 1.0f / static_cast<float>(kStepsPerSecond);
        inline constexpr int kThreadPriority = 4;
        inline constexpr int kThreadStackSize = 4096;
    } // namespace simulation

    namespace render_thread
    {
        inline constexpr int kThreadPriority = 5;
        inline constexpr int kThreadStackSize = 8192;
    } // namespace render_thread

    namespace camera
    {
        inline constexpr int kPositionX = 0;
        inline constexpr int kPositionY = 70;
        inline constexpr int kPositionZ = -1400;
        inline constexpr int kTargetX = 0;
        inline constexpr int kTargetY = 0;
        inline constexpr int kTargetZ = 0;
        inline constexpr int kOrbitRadius = 1400;
        inline constexpr int kOrbitHeight = 70;
        inline constexpr uint32_t kOrbitPeriodMs = 20000u;
        inline constexpr int kFovDegrees = 60;
        inline constexpr int kNearPlane = 16;
        inline constexpr int kFarPlane = 2000;
    } // namespace camera

    namespace boids
    {
        inline constexpr uint32_t kSeed = 0xC0FFEEu;
        inline constexpr std::size_t kMaxBoids = 32;
        inline constexpr std::size_t kSpawnedBoids = 24;
        inline constexpr float kBoundsMinX = -120.0f;
        inline constexpr float kBoundsMinY = -80.0f;
        inline constexpr float kBoundsMinZ = -120.0f;
        inline constexpr float kBoundsMaxX = 120.0f;
        inline constexpr float kBoundsMaxY = 80.0f;
        inline constexpr float kBoundsMaxZ = 120.0f;
        inline constexpr float kBoundsMargin = 24.0f;
        inline constexpr float kBoundsStrength = 90.0f;
        inline constexpr float kNeighborRadius = 48.0f;
        inline constexpr float kSeparationRadius = 18.0f;
        inline constexpr float kSeparationWeight = 110.0f;
        inline constexpr float kAlignmentWeight = 2.8f;
        inline constexpr float kCohesionWeight = 0.8f;
        inline constexpr float kMinSpeed = 50.0f;
        inline constexpr float kMaxSpeed = 100.0f;
    } // namespace boids

    namespace boid_mesh
    {
        inline constexpr int kNoseX = 0;
        inline constexpr int kNoseY = 0;
        inline constexpr int kNoseZ = 28;
        inline constexpr int kLeftX = -8;
        inline constexpr int kLeftY = -5;
        inline constexpr int kLeftZ = -12;
        inline constexpr int kRightX = 8;
        inline constexpr int kRightY = -5;
        inline constexpr int kRightZ = -12;
        inline constexpr int kTopX = 0;
        inline constexpr int kTopY = 9;
        inline constexpr int kTopZ = -8;
    } // namespace boid_mesh

    namespace rendering
    {
        inline constexpr std::size_t kInitialSceneObjects = 32;
    } // namespace rendering

} // namespace constants

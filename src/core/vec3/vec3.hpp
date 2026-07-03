/*
 * Copyright (c) 2026 Yuancheng Li
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cmath>

struct Vec3
{
    float x{};
    float y{};
    float z{};

    constexpr Vec3 operator+(const Vec3 &rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }

    constexpr Vec3 operator-(const Vec3 &rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    constexpr Vec3 operator*(float scalar) const
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    constexpr Vec3 &operator+=(const Vec3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr float length_squared() const
    {
        return x * x + y * y + z * z;
    }

    float length() const
    {
        return std::sqrt(length_squared());
    }

    Vec3 normalized() const
    {
        const float len = length();
        return len > 0.0f ? *this * (1.0f / len) : Vec3{};
    }

    Vec3 limited(float max_length) const
    {
        const float len_sq = length_squared();
        const float max_sq = max_length * max_length;

        if (len_sq <= max_sq || len_sq == 0.0f)
        {
            return *this;
        }

        return normalized() * max_length;
    }

    Vec3 clamp_magnitude(float min_magnitude, float max_magnitude) const
    {
        const float magnitude_sq = length_squared();

        if (magnitude_sq == 0.0f)
        {
            return {};
        }

        const float min_sq = min_magnitude * min_magnitude;
        const float max_sq = max_magnitude * max_magnitude;

        if (magnitude_sq >= min_sq && magnitude_sq <= max_sq)
        {
            return *this;
        }

        const float magnitude = std::sqrt(magnitude_sq);
        const float target_magnitude =
            magnitude < min_magnitude ? min_magnitude : max_magnitude;

        return *this * (target_magnitude / magnitude);
    }

    static constexpr float dot(const Vec3 &a, const Vec3 &b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
};
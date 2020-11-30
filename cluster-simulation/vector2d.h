#pragma once
#include "olcPixelGameEngine.h"

template <typename T = int32_t>
struct vector2d : public olc::v2d_generic<T>
{
    vector2d() : olc::v2d_generic<T>(0, 0) {}
    vector2d(T x, T y) : olc::v2d_generic<T>(x, y) {}
    vector2d(const vector2d& v) : olc::v2d_generic<T>(v) {}
    vector2d(const olc::v2d_generic<T>& v) : olc::v2d_generic<T>(v) {}

    T distance_squared(const vector2d<T>& other) const
    {
        T deltaX = other.x - this->x;
        T deltaY = other.y - this->y;

        return deltaX * deltaX + deltaY * deltaY;
    }
};

using vi16_2d = vector2d<int16_t>;
using vu16_2d = vector2d<uint16_t>;
using vi2d = vector2d<int32_t>;
using vu2d = vector2d<uint32_t>;
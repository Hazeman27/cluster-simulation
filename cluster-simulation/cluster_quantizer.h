#pragma once
#include "olcPixelGameEngine.h"
#include "vector2d.h"

template <typename T>
struct observation : public vector2d<T>
{
    mutable T mean = INT32_MAX;
    mutable size_t cluster_index = SIZE_MAX;

    observation() = default;

    observation(const vector2d<T>& position) : vector2d<T>position)
    {}

    observation(const vector2d<T>& position, T mean, size_t cluster_index) :
        vector2d<T>(position),
        mean(mean),
        cluster_index(cluster_index)
    {}

    observation(T x, T y, T mean, size_t cluster_index) :
        vector2d<T>(x, y),
        mean(mean),
        cluster_index(cluster_index)
    {}
};

using obsi = observation<int32_t>;

struct cluster_quantizer
{
    std::string name = "Cluster Quantizer";
    int32_t default_init_param = INT32_MIN;

    cluster_quantizer() = default;

    cluster_quantizer(std::string name, int32_t default_init_param)
        : name(name), default_init_param(default_init_param)
    {}

    virtual std::vector<std::vector<vi2d>> quantize(
        const std::vector<obsi>& observations,
        uint32_t iterations_amount,
        int32_t init_param
    ) = 0;
};
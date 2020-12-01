#pragma once
#include "vector2d.h"
#include "timer.h"

namespace ntf
{
    namespace cluster
    {
        template <typename T = int32_t>
        struct observation : public vector2d<T>
        {
            mutable T mean = INT32_MAX;
            mutable size_t cluster_index = 0;

            observation() = default;

            observation(const observation<T>& obs) :
                vector2d<T>(obs.x, obs.y),
                mean(obs.mean),
                cluster_index(obs.cluster_index)
            {}

            observation(const vector2d<T>& position) : vector2d<T>(position)
            {}

            observation(T x, T y) : vector2d<T>(x, y)
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

            observation& operator = (const observation& obs) = default;

            operator vector2d<T>() const
            {
                return { this->x, this->y };
            }
        };

        using obsi = observation<int32_t>;

        struct quantizer
        {
            std::string name = "Cluster Quantizer";
            mutable int32_t quantization_param;

            int32_t quantization_param_min;
            int32_t quantization_param_max;

            quantizer() = default;

            quantizer(const quantizer& quantizer) :
                name(quantizer.name),
                quantization_param(quantizer.quantization_param),
                quantization_param_min(quantizer.quantization_param_min),
                quantization_param_max(quantizer.quantization_param_max)
            {}

            quantizer(
                std::string name,
                int32_t quantization_param,
                int32_t quantization_param_min,
                int32_t quantization_param_max) :
                name(name),
                quantization_param(quantization_param),
                quantization_param_min(quantization_param_min),
                quantization_param_max(quantization_param_max)
            {}

            virtual void quantize(const std::vector<obsi>& observations, microseconds& _elapsed_time, uint32_t& iterations) = 0;
        };
    }
}
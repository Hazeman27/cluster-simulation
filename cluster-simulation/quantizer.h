#pragma once
#include "vector2d.h"
#include "timer.h"
#include "constrained.h"

namespace ntf
{
    namespace cluster
    {
        template <typename T, T default_mean>
        struct observation : public vector2d<T>
        {
            mutable T mean = default_mean;
            mutable size_t cluster_index = 0;

            observation() = default;

            observation(const observation<T, default_mean>& obs) :
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

            observation& operator= (const observation& obs) = default;

            operator vector2d<T>() const
            {
                return { this->x, this->y };
            }
        };

        using obsi = observation<int32_t, INT32_MAX>;

        struct quantizer
        {
            std::string name = "Cluster Quantizer";
            constrained<uint8_t, 1, UINT8_MAX, 5> quantization_param;

            quantizer() = default;

            quantizer(const std::string& name) : name(name)
            {}

            quantizer(std::string&& name) : name(name)
            {}

            virtual void quantize(const std::vector<obsi>& observations, microseconds& _elapsed_time, uint32_t& iterations) = 0;

            static void reset_observations_means(const std::vector<obsi>& observations)
            {
                for (auto& observation : observations)
                    observation.mean = INT32_MAX;
            }
        };
    }
}
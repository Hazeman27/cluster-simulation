#pragma once
#include <random>
#include <queue>
#include "colors.h"
#include "constrained.h"
#include "timer.h"
#include "vector2d.h"

namespace ntf
{
    namespace cluster
    {
        template <typename T>
        struct cluster
        {
            std::vector<size_t> observation_indices;
            vector2d<T> mean;

            color color{ 255, 255, 255 };

            uint64_t variability(const std::vector<vector2d<T>>& observations) const
            {
                uint64_t result = 0;

                for (auto& i : this->observation_indices)
                    result += this->mean.euclidean_distance_squared(observations[i]);

                return result;
            }
        };

        struct partitioning_profile
        {
            size_t iterations = 0;
            microseconds elapsed_time = microseconds::zero();

            void reset()
            {
                this->iterations = 0;
                this->elapsed_time = microseconds::zero();
            }

            partitioning_profile& operator+= (const partitioning_profile& rhs)
            {
                this->iterations += rhs.iterations;
                this->elapsed_time += rhs.elapsed_time;

                return *this;
            }

            partitioning_profile& operator-= (const partitioning_profile& rhs)
            {
                this->iterations -= rhs.iterations;
                this->elapsed_time -= rhs.elapsed_time;

                return *this;
            }

            partitioning_profile operator+ (const partitioning_profile& rhs) const
            {
                return { this->iterations + rhs.iterations, this->elapsed_time + rhs.elapsed_time };
            }

            partitioning_profile operator- (const partitioning_profile& rhs) const
            {
                return { this->iterations - rhs.iterations, this->elapsed_time - rhs.elapsed_time };
            }
        };

        template <typename T>
        struct partitioner
        {
            std::string name;
            constrained<uint8_t, 1, UINT8_MAX, 5> param;

            virtual std::vector<cluster<T>> partition(const std::vector<vector2d<T>>& observations, partitioning_profile& profile = {}) = 0;
        };

        template <typename T = int32_t>
        uint64_t dissimilarity(const std::vector<cluster<T>>& clusters, const std::vector<vector2d<T>>& observations)
        {
            uint64_t result = 0;

            for (auto& cluster : clusters)
                result += cluster.variability(observations);

            return result;
        }
    }
}

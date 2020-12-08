#pragma once
#include <random>
#include <queue>
#include "colors.h"
#include "constrained.h"
#include "timer.h"
#include "vector2d.h"

namespace ntf::cluster
{
    template <typename T = int>
    struct cluster
    {
        std::vector<v2d_shared_ptr<T>> observations;
        v2d<T> mean;

        color color{ 255, 255, 255 };

        double variability() const
        {
            double result = 0;

            for (auto& observation : this->observations)
                result += this->mean.euclidean_distance_squared(*observation);

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
        std::string param_name;
        constrained<uint8_t, 1, UINT8_MAX, 15> param;

        virtual std::vector<cluster<T>> partition(std::vector<v2d<T>>& observations, partitioning_profile& profile = {}) = 0;
    };

    template <typename T = int32_t>
    double variability(const v2d<T>& mean, const std::vector<v2d<T>>& observations)
    {
        double result = 0;

        for (auto& observation : observations)
            result += mean.euclidean_distance_squared(observation);

        return result;
    }

    template <typename T = int32_t>
    double variability(const v2d<T>& mean, const std::vector<v2d_shared_ptr<T>>& observations)
    {
        double result = 0;

        for (auto& observation : observations)
            result += mean.euclidean_distance_squared(*observation);

        return result;
    }

    template <typename T = int32_t>
    double dissimilarity(const std::vector<cluster<T>>& clusters)
    {
        double result = 0;

        for (auto& cluster : clusters)
            result += cluster.variability();

        return result;
    }

    template <typename T = int32_t>
    void clear_clusters(std::vector<cluster<T>>& clusters)
    {
        for (auto& cluster : clusters)
            cluster.observations.clear();
    }

    template <typename T = int32_t>
    typename std::vector<cluster<T>>::const_iterator find_empty_cluster(const std::vector<cluster<T>>& clusters)
    {
        return std::find_if(
            clusters.begin(),
            clusters.end(),
            [&](const cluster<T>& c) { return c.observations.empty(); }
        );
    }
}

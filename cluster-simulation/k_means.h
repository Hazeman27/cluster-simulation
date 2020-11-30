#pragma once
#include "cluster_quantizer.h"
#include <random>


struct k_means : public cluster_quantizer
{
    k_means() : cluster_quantizer("K means", 20)
    {}

    std::vector<std::vector<vi2d>> quantize(
        const std::vector<obsi>& observations,
        uint32_t iterations_amount,
        int32_t init_param
    ) override
    {
        std::default_random_engine random_engine(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );

        std::uniform_int_distribution<int32_t> distribution(0, static_cast<int32_t>(observations.size()));
        std::vector<obsi> centroids;

        std::unordered_map<int32_t, bool> visited_observations;

        for (int32_t i = 0; i < init_param;) {
            int32_t index = distribution(random_engine);

            if (visited_observations.find(index) != visited_observations.end())
                continue;

            centroids.push_back(observations[index]);
            visited_observations.insert({ index, true });

            i++;
        }

        std::vector<std::vector<vi2d>> clusters(init_param);

        for (uint32_t n = 0; n < iterations_amount; n++) {

            for (size_t i = 0; i < centroids.size(); i++) {

                auto& centroid = centroids[i];

                for (size_t j = 0; j < observations.size(); j++) {

                    auto& observation = observations[j];
                    int32_t distance = centroid.distance_squared(observation);

                    if (distance < observation.mean) {
                        observation.mean = distance;
                        observation.cluster_index = i;
                    }
                }
            }

            std::vector<size_t> clusters_observations_amount(init_param, 0);
            std::vector<vi2d> observations_coords_sum(init_param, { 0, 0 });

            for (auto& observation : observations) {

                clusters_observations_amount[observation.cluster_index]++;
                observations_coords_sum[observation.cluster_index] += observation;

                observation.mean = INT32_MAX;
            }

            for (size_t i = 0; i < centroids.size(); i++) {

                centroids[i] = {
                    observations_coords_sum[i] / clusters_observations_amount[i],
                    centroids[i].mean,
                    centroids[i].cluster_index,
                };
            }
        }

        for (auto& observation : observations)
            clusters.at(observation.cluster_index).push_back(observation);

        return clusters;
    }
};
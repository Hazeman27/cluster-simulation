#pragma once
#include "cluster_quantizer.h"
#include <random>


struct k_means : public cluster_quantizer
{
    k_means() : cluster_quantizer("K means", 20, 1, INT32_MAX)
    {}

    void quantize(const std::vector<obsi>& observations, uint32_t iterations_amount) override
    {
        std::default_random_engine random_engine(
            static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        );

        std::uniform_int_distribution<int32_t> distribution(0, static_cast<int32_t>(observations.size()));
        std::vector<obsi> centroids(this->quantization_param);

        std::unordered_map<int32_t, bool> visited_observations;

        for (int32_t i = 0; i < this->quantization_param;) {
            int32_t index = distribution(random_engine);

            if (visited_observations.find(index) != visited_observations.end())
                continue;

            centroids[i] = observations[index];
            visited_observations.insert({ index, true });

            i++;
        }

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

            std::vector<int32_t> clusters_observations_amount(this->quantization_param, 0);
            std::vector<vi2d> observations_coords_sum(this->quantization_param, { 0, 0 });

            for (auto& observation : observations) {

                clusters_observations_amount[observation.cluster_index]++;
                observations_coords_sum[observation.cluster_index] += observation;

                observation.mean = INT32_MAX;
            }

            for (size_t i = 0; i < centroids.size(); i++) {

                centroids[i] = {
                    observations_coords_sum[i] / std::max(clusters_observations_amount[i], 1),
                    centroids[i].mean,
                    centroids[i].cluster_index,
                };
            }
        }
    }
};
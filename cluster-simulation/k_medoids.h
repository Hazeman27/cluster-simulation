#pragma once
#include "quantizer.h"
#include <random>


namespace ntf
{
    namespace cluster
    {
        struct k_medoids : public quantizer
        {
            k_medoids() : quantizer("K medoids", 5, 1, UINT8_MAX)
            {}

            void quantize(
                const std::vector<obsi>& observations,
                microseconds& _elapsed_time,
                uint32_t& iterations
            ) override
            {
                timer t(_elapsed_time);
                iterations = 0;

                std::default_random_engine random_engine(
                    static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
                );

                std::uniform_int_distribution<int32_t> distribution(0, static_cast<int32_t>(observations.size()));

                std::vector<obsi> medoids(this->quantization_param);

                int32_t current_dissimilarity = 0;
                int32_t previous_dissimilarity = INT32_MAX;

                std::unordered_map<int32_t, bool> visited_observations;

                for (int32_t i = 0; i < this->quantization_param;)
                {
                    int32_t index = distribution(random_engine);

                    if (visited_observations.find(index) != visited_observations.end())
                        continue;

                    medoids[i] = observations[index];
                    visited_observations.insert({ index, true });

                    i++;
                }

                while (false)
                {
                    iterations++;
                }
            }
        };
    }
}
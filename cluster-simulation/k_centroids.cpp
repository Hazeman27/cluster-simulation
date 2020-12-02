#include "k_centroids.h"
#include <random>

ntf::cluster::k_centroids::k_centroids() : quantizer("K centroids")
{}

void ntf::cluster::k_centroids::quantize(
    const std::vector<obsi>& observations,
    microseconds& _elapsed_time,
    uint32_t& iterations)
{
    quantizer::reset_observations_means(observations);

    timer t(_elapsed_time);
    iterations = 0;

    std::default_random_engine random_engine(
        static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    );

    std::uniform_int_distribution<int32_t> distribution(0, static_cast<int32_t>(observations.size()) - 1);

    std::vector<obsi> centroids(this->quantization_param);
    std::vector<obsi> previous_centroids(this->quantization_param, { 0, 0 });

    std::unordered_map<int32_t, bool> visited_observations;

    for (int32_t i = 0; i < this->quantization_param;)
    {
        int32_t index = distribution(random_engine);

        if (visited_observations.find(index) != visited_observations.end())
            continue;

        centroids[i] = observations[index];
        visited_observations.insert({ index, true });

        i++;
    }

    auto centroids_have_changed = [&]()
    {
        bool result = false;

        for (size_t i = 0; i < this->quantization_param; i++) {
            if (centroids[i] != previous_centroids[i])
                result = true;
            previous_centroids[i] = centroids[i];
        }

        return result;
    };

    while (centroids_have_changed())
    {
        for (size_t i = 0; i < centroids.size(); i++)
        {
            auto& centroid = centroids[i];

            for (size_t j = 0; j < observations.size(); j++)
            {
                auto& observation = observations[j];
                int32_t distance = centroid.distance_squared(observation);

                if (distance < observation.mean)
                {
                    observation.mean = distance;
                    observation.cluster_index = i;
                }
            }
        }

        std::vector<int32_t> clusters_observations_amount(this->quantization_param, 0);
        std::vector<vi2d> observations_coords_sum(this->quantization_param, { 0, 0 });

        for (auto& observation : observations)
        {
            clusters_observations_amount[observation.cluster_index]++;
            observations_coords_sum[observation.cluster_index] += observation;

            observation.mean = INT32_MAX;
        }

        for (size_t i = 0; i < centroids.size(); i++)
        {
            centroids[i] = {
                observations_coords_sum[i] / std::max(clusters_observations_amount[i], 1),
                centroids[i].mean,
                centroids[i].cluster_index,
            };
        }

        iterations++;
    }
}

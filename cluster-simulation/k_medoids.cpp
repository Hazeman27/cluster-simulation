#include "k_medoids.h"
#include <random>

ntf::cluster::k_medoids::k_medoids() : quantizer("K medoids")
{}

void ntf::cluster::k_medoids::quantize(
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

    std::uniform_int_distribution<int32_t> observations_distr(0, static_cast<int32_t>(observations.size()) - 1);
    std::uniform_int_distribution<int32_t> medoids_distr(0, this->quantization_param - 1);

    std::vector<obsi> medoids(this->quantization_param);

    int32_t current_dissimilarity = INT32_MAX - 1;
    int32_t previous_dissimilarity = INT32_MAX;

    std::unordered_map<int32_t, bool> visited_observations;

    for (int32_t i = 0; i < this->quantization_param;)
    {
        int32_t index = observations_distr(random_engine);

        if (visited_observations.find(index) != visited_observations.end())
            continue;

        medoids[i] = observations[index];
        visited_observations.insert({ index, true });

        i++;
    }

    auto compute_dissimilarity = [&](const obsi& medoid)
    {
        int32_t result = 0;

        for (size_t i = 0; i < observations.size(); i++)
        {
            auto& observation = observations[i];

            auto dissimilarity = medoid.dissimilarity(observation);
            result += dissimilarity;
        }

        return result;
    };

    auto is_medoid = [&](const obsi& obs)
    {
        for (auto& medoid : medoids)
        {
            if (medoid == obs)
                return true;
        }

        return false;
    };

    vi2d swap_indices = { -1, -1 };
    std::unordered_map<vi2d, bool, vi2d> visited_swaps;

    while (current_dissimilarity < previous_dissimilarity)
    {
        if (swap_indices.x > 0 && swap_indices.y > 0)
            medoids[swap_indices.x] = observations[swap_indices.y];

        for (size_t i = 0; i < medoids.size(); i++)
        {
            auto& medoid = medoids[i];

            for (size_t j = 0; j < observations.size(); j++)
            {
                auto& observation = observations[j];
                auto dissimilarity = medoid.dissimilarity(observation);

                if (dissimilarity < observation.mean)
                {
                    observation.mean = dissimilarity;
                    observation.cluster_index = i;
                }
            }
        }

        do {
            swap_indices = { medoids_distr(random_engine), observations_distr(random_engine) };

            if (visited_swaps.find(swap_indices) != visited_swaps.end())
                continue;

        } while (is_medoid(observations[swap_indices.y]));

        previous_dissimilarity = current_dissimilarity;
        current_dissimilarity = compute_dissimilarity(observations[swap_indices.x]);

        visited_swaps.insert({ swap_indices, true });

        iterations++;
    }
}
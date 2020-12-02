#pragma once
#include "cluster.h"

namespace ntf
{
    namespace cluster
    {
        template <typename T = int32_t>
        struct k_means : public partitioner<T>
        {
            std::default_random_engine random_engine;

            k_means()
            {
                this->name = "K means";
            }

            std::vector<cluster<T>> partition(const std::vector<vector2d<T>>& observations, partitioning_profile& profile = {}) override
            {
                this->random_engine.seed(static_cast<uint32_t>(high_res_clock::now().time_since_epoch().count()));

                profile.reset();
                timer t(profile.elapsed_time);

                std::uniform_int_distribution<size_t> indices_distribution(0, observations.size() - 1);
                std::unordered_map<size_t, bool> visited_indices;

                std::vector<cluster<T>> clusters(this->param);
                std::vector<vector2d<T>> previous_means(this->param);

                for (size_t i = 0; i < this->param;)
                {
                    size_t random_index = indices_distribution(this->random_engine);

                    if (visited_indices.find(random_index) != visited_indices.end())
                        continue;

                    auto& cluster = clusters.at(i);

                    cluster.mean = observations[random_index];
                    cluster.color = VISUALLY_DISTINCT_COLORS[i];

                    visited_indices.insert({ random_index, true });
                    i++;
                }

                auto compute_centroid = [&](size_t cluster_index)
                {
                    vector2d<T> coords_sum{};
                    auto& cluster = clusters.at(cluster_index);

                    for (auto& index : cluster.observation_indices)
                        coords_sum += observations.at(index);

                    previous_means.at(cluster_index) = cluster.mean;
                    cluster.mean = coords_sum / std::max(static_cast<T>(cluster.observation_indices.size()), 1);
                };

                auto has_converged = [&]()
                {
                    for (size_t i = 0; i < clusters.size(); i++)
                    {
                        if (clusters[i].mean != previous_means[i])
                            return false;
                    }

                    return true;
                };

                while (!has_converged())
                {
                    for (auto& cluster : clusters)
                        cluster.observation_indices.clear();

                    for (size_t i = 0; i < observations.size(); i++)
                    {
                        auto& observation = observations[i];

                        double closest_distance = DBL_MAX;
                        size_t closest_cluster_index = 0;

                        for (size_t j = 0; j < this->param; j++)
                        {
                            auto& cluster = clusters[j];

                            double distance = observation.euclidean_distance(cluster.mean);

                            if (distance < closest_distance)
                            {
                                closest_distance = distance;
                                closest_cluster_index = j;
                            }
                        }

                        clusters[closest_cluster_index].observation_indices.push_back(i);
                    }

                    for (size_t i = 0; i < clusters.size(); i++)
                        compute_centroid(i);

                    profile.iterations++;
                }

                return clusters;
            }
        };

        template <typename T = int32_t>
        struct k_means_multi : public partitioner<T>
        {
            k_means_multi()
            {
                this->name = "K means (with max K)";
            }

            std::vector<cluster<T>> partition(const std::vector<vector2d<T>>& observations, partitioning_profile& profile = {}) override
            {
                profile.reset();
                timer t(profile.elapsed_time);

                partitioning_profile p{};

                std::vector<cluster<T>> clusters;
                uint8_t best_k = 1;

                uint64_t best_dissimilarity = UINT64_MAX;
                k_means k_means;

                for (uint8_t k = 1; k < this->param; k++)
                {
                    k_means.param = k;
                    clusters = std::move(k_means.partition(observations, p));

                    auto current_dissimilarity = dissimilarity(clusters, observations);
                    profile.iterations += p.iterations;
                    
                    if (current_dissimilarity < best_dissimilarity) {
                        best_k = k;
                        best_dissimilarity = current_dissimilarity;
                    }
                }

                k_means.param = best_k;
                return std::move(k_means.partition(observations, p));
            }
        };
    }
}
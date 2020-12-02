#pragma once
#include "quantizer.h"


namespace ntf
{
    namespace cluster
    {
        struct k_centroids : public quantizer
        {
            k_centroids();

            void quantize(
                const std::vector<obsi>& observations,
                microseconds& _elapsed_time,
                uint32_t& iterations
            ) override;
        };
    }
}
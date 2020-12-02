#pragma once
#include "quantizer.h"
#include <random>

namespace ntf
{
    namespace cluster
    {
        struct k_medoids : public quantizer
        {
            k_medoids();

            void quantize(
                const std::vector<obsi>& observations,
                microseconds& _elapsed_time,
                uint32_t& iterations
            ) override;
        };
    }
}
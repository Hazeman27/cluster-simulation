#include "simulator.h"
#include "k_centroids.h"
#include "k_medoids.h"


int main()
{
    ntf::cluster::simulator simulator({
        std::make_shared<ntf::cluster::k_centroids>(),
        std::make_shared<ntf::cluster::k_medoids>(),
    });

    if (simulator.Construct(480, 320, 2, 2))
        simulator.Start();

    return 0;
}
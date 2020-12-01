#include "cluster_simulator.h"
#include "k_means.h"


int main()
{
    cluster_simulator simulator({ std::make_shared<k_means>() });

    if (simulator.Construct(480, 320, 2, 2))
        simulator.Start();

    return 0;
}
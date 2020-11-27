#include "cluster_simulator.h"


int main()
{
    cluster_simulator simulator;

    if (simulator.Construct(480, 320, 2, 2))
        simulator.Start();

    return 0;
}
#include <cassert>
#include "k_means.h"
#include "simulator.h"

int main()
{
    ntf::cluster::simulator simulator({
        std::make_shared<ntf::cluster::k_means<>>(),
        std::make_shared<ntf::cluster::k_means_multi<>>(),
    });

    if (simulator.Construct(480, 320, 2, 2))
        simulator.Start();

    return 0;
}
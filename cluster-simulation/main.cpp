#include <cassert>
#include "k_means.h"
#include "simulator.h"

int main()
{
    std::vector<std::shared_ptr<ntf::cluster::partitioner<int>>> partitioners{
        std::make_shared<ntf::cluster::k_means<>>(),
        std::make_shared<ntf::cluster::k_means_multi<>>(),
    };

    std::shared_ptr<ntf::screen> simulator(std::make_shared<ntf::cluster::simulator>(partitioners));
    std::vector<std::shared_ptr<ntf::screen>> screens{ simulator };

    std::shared_ptr<ntf::window> window(std::make_shared<ntf::window>(
        "Cluster simulation",
        screens
    ));

    if (window->Construct(480, 320, 2, 2))
        window->Start();

    return 0;
}
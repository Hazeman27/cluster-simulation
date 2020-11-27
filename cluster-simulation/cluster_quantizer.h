#pragma once
#include "olcPixelGameEngine.h"


using cluster = std::vector<olc::vi2d>;

class cluster_quantizer
{
    std::string name;
    virtual std::vector<cluster> quantize(std::vector<olc::vi2d> observations) = 0;
};
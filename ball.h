#pragma once
#include "vec2f.h"

struct Ball {
    Vec2f pos;
    Vec2f vel;

    Ball();
    void step(const double& acc, const int timestep);
    void fast_step(const double& acc);
};

#pragma once

#include "common.hpp"

#include <utility>
#include <vector>


struct Vec3s {
    std::vector<float> xs;
    std::vector<float> ys;
    std::vector<float> zs;
};


struct Bodies {
    Vec3s positions;
    Vec3s velocities;
    std::vector<float> masses;
};


class SimulationDod {
public:
    SimulationDod(Bodies bodies) : m_bodies(std::move(bodies)) {}

    void Update(float deltaTime);
    const Bodies& GetBodies() const { return m_bodies; }

private:
    Bodies m_bodies;
};
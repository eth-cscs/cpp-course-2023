#include "dod.hpp"
#include "oop.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>


template <class Rng, class Rne>
std::vector<float> RandomVector(size_t size, Rng&& rng, Rne&& rne) {
    std::vector<float> v(size);
    std::ranges::generate(v, [&] { return rng(rne); });
    return v;
}


Bodies RandomBodies(size_t numBodies) {
    std::mt19937_64 rne(6356467);

    std::uniform_real_distribution<float> positionRng(-1.0f, 1.0f);
    std::uniform_real_distribution<float> velocityRng(-0.05f, 0.05f);
    std::uniform_real_distribution<float> massRng(0.05f, 1.0f);

    Bodies bodies;
    bodies.positions.xs = RandomVector(numBodies, positionRng, rne);
    bodies.positions.ys = RandomVector(numBodies, positionRng, rne);
    bodies.positions.zs = RandomVector(numBodies, positionRng, rne);

    bodies.velocities.xs = RandomVector(numBodies, velocityRng, rne);
    bodies.velocities.ys = RandomVector(numBodies, velocityRng, rne);
    bodies.velocities.zs = RandomVector(numBodies, velocityRng, rne);

    bodies.masses = RandomVector(numBodies, massRng, rne);
    return bodies;
}


std::vector<Body> ConvertBodies(const Bodies& bodies) {
    std::vector<Body> convertedBodies;
    convertedBodies.reserve(bodies.masses.size());
    for (size_t i = 0; i < bodies.masses.size(); ++i) {
        convertedBodies.emplace_back(
            Vec3{ bodies.positions.xs[i], bodies.positions.ys[i], bodies.positions.zs[i] },
            Vec3{ bodies.velocities.xs[i], bodies.velocities.ys[i], bodies.velocities.zs[i] },
            bodies.masses[i]);
    }
    return convertedBodies;
}


bool CompareBodies(const std::vector<Body>& lhs, const std::vector<Body>& rhs) {
    const auto compare = [](const Body& lhs, const Body& rhs) {
        return lhs.GetPos().x == rhs.GetPos().x
               && lhs.GetPos().y == rhs.GetPos().y
               && lhs.GetPos().z == rhs.GetPos().z;
    };
    return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), true, std::logical_and{}, compare);
}


void PrintBodies(const std::vector<Body>& bodies) {
    for (auto& body : bodies) {
        std::cout << "{"
                  << body.GetPos().x << ", " << body.GetPos().y << ", " << body.GetPos().z
                  << "}"
                  << std::endl;
    }
}


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    const size_t numBodies = 100000;
    const size_t numTimesteps = 1;
    const float deltaTime = 0.001f;

    const auto dodBodies = RandomBodies(numBodies);
    const auto oopBodies = ConvertBodies(dodBodies);

    SimulationDod simDod{ dodBodies };
    SimulationOop simOop{ oopBodies };

    {
        const auto start = high_resolution_clock::now();
        for (size_t timestep = 0; timestep < numTimesteps; ++timestep) {
            simOop.Update(deltaTime);
        }
        const auto end = high_resolution_clock::now();
        const auto elapsed = duration_cast<milliseconds>(end - start);
        std::cout << "OOP: " << elapsed.count() << " ms" << std::endl;
    }

    {
        const auto start = high_resolution_clock::now();
        for (size_t timestep = 0; timestep < numTimesteps; ++timestep) {
            simDod.Update(deltaTime);
        }
        const auto end = high_resolution_clock::now();
        const auto elapsed = duration_cast<milliseconds>(end - start);
        std::cout << "DoD: " << elapsed.count() << " ms" << std::endl;
    }

    const auto& oopBodiesEnd = simOop.GetBodies();
    const auto& dodBodiesEnd = ConvertBodies(simDod.GetBodies());

    // std::cout << "OOP bodies:\n";
    // PrintBodies(oopBodiesEnd);
    // std::cout << "\nDoD bodies:\n";
    // PrintBodies(dodBodiesEnd);

    if (CompareBodies(oopBodiesEnd, dodBodiesEnd)) {
        std::cout << "Two simulations match!" << std::endl;
    }
    else {
        std::cout << "Two simulations DO NOT match!" << std::endl;
    }
}
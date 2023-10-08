#include "dod.hpp"

#include <cmath>


float Distance(float x1, float y1, float z1, float x2, float y2, float z2) {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float dz = z2 - z1;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}


Vec3s GetForces(const Vec3s& positions, const std::vector<float>& masses) {
    const size_t n = masses.size();
    Vec3s forces{
        std::vector<float>(n, 0.0f),
        std::vector<float>(n, 0.0f),
        std::vector<float>(n, 0.0f),
    };

    for (size_t anchorIdx = 0; anchorIdx < n; ++anchorIdx) {
        const float anchorPosX = positions.xs[anchorIdx];
        const float anchorPosY = positions.ys[anchorIdx];
        const float anchorPosZ = positions.zs[anchorIdx];
        const float anchorMass = masses[anchorIdx];

        const auto updateForces = [&](size_t runningIdx) {
            const float runningPosX = positions.xs[runningIdx];
            const float runningPosY = positions.ys[runningIdx];
            const float runningPosZ = positions.zs[runningIdx];
            const float runningMass = masses[runningIdx];

            const float distance = Distance(anchorPosX, anchorPosY, anchorPosZ, runningPosX, runningPosY, runningPosZ);
            const float forceMagnitude = GravitationalForce(anchorMass, runningMass, distance);

            forces.xs[runningIdx] += (runningPosX - anchorPosX) / distance * forceMagnitude;
            forces.ys[runningIdx] += (runningPosY - anchorPosY) / distance * forceMagnitude;
            forces.zs[runningIdx] += (runningPosZ - anchorPosZ) / distance * forceMagnitude;
        };

        for (size_t runningIdx = 0; runningIdx < anchorIdx; ++runningIdx) {
            updateForces(runningIdx);
        }
        for (size_t runningIdx = anchorIdx + 1; runningIdx < n; ++runningIdx) {
            updateForces(runningIdx);
        }
    }

    return forces;
}


Vec3s GetAccelerations(const Vec3s& forces, const std::vector<float>& masses) {
    const auto n = masses.size();
    Vec3s accelerations;
    accelerations.xs.resize(n);
    accelerations.ys.resize(n);
    accelerations.zs.resize(n);
    for (size_t i = 0; i < n; ++i) {
        const auto mass = masses[i];
        accelerations.xs[i] = forces.xs[i] * mass;
        accelerations.ys[i] = forces.ys[i] * mass;
        accelerations.zs[i] = forces.zs[i] * mass;
    }
    return accelerations;
}


Vec3s IntegrateVec3s(const Vec3s& quantity, const Vec3s& derivative, float deltaTime) {
    const auto n = quantity.xs.size();
    Vec3s newQuantity;
    newQuantity.xs.resize(n);
    newQuantity.ys.resize(n);
    newQuantity.zs.resize(n);
    for (size_t i = 0; i < n; ++i) {
        newQuantity.xs[i] = Integrate(quantity.xs[i], derivative.xs[i], deltaTime);
        newQuantity.ys[i] = Integrate(quantity.ys[i], derivative.ys[i], deltaTime);
        newQuantity.zs[i] = Integrate(quantity.zs[i], derivative.zs[i], deltaTime);
    }
    return newQuantity;
}


void SimulationDod::Update(float deltaTime) {
    const auto forces = GetForces(m_bodies.positions, m_bodies.masses);
    const auto accelerations = GetAccelerations(forces, m_bodies.masses);
    auto velocities = IntegrateVec3s(m_bodies.velocities, accelerations, deltaTime);
    auto positions = IntegrateVec3s(m_bodies.positions, velocities, deltaTime);
    m_bodies.positions = std::move(positions);
    m_bodies.velocities = std::move(velocities);
}

#include "oop.hpp"

#include <cmath>
#include <ranges>


float Distance(const Vec3& lhs, const Vec3& rhs) {
    const float dx = lhs.x - rhs.x;
    const float dy = lhs.y - rhs.y;
    const float dz = lhs.z - rhs.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}


Vec3 Add(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
    };
}


Vec3 Body::GetForce(const Body& other) const {
    const float distance = Distance(m_pos, other.m_pos);
    const float forceMagnitude = GravitationalForce(m_mass, other.m_mass, distance);
    const Vec3 forceVector = {
        (other.m_pos.x - m_pos.x) / distance * forceMagnitude,
        (other.m_pos.y - m_pos.y) / distance * forceMagnitude,
        (other.m_pos.z - m_pos.z) / distance * forceMagnitude,
    };
    return forceVector;
}


void Body::Update(float deltaTime, const Vec3& force) {
    const Vec3 acceleration = {
        force.x / m_mass,
        force.y / m_mass,
        force.z / m_mass,
    };
    // Low quality numerical integration, but that's not the point here.
    const Vec3 newVelocity = {
        Integrate(m_velocity.x, acceleration.x, deltaTime),
        Integrate(m_velocity.y, acceleration.y, deltaTime),
        Integrate(m_velocity.z, acceleration.z, deltaTime),
    };
    const Vec3 newPos = {
        Integrate(m_pos.x, newVelocity.x, deltaTime),
        Integrate(m_pos.y, newVelocity.y, deltaTime),
        Integrate(m_pos.z, newVelocity.z, deltaTime),
    };
    m_pos = newPos;
    m_velocity = newVelocity;
}


void SimulationOop::Update(float deltaTime) {
    std::vector<Vec3> forces(m_bodies.size(), Vec3{ 0, 0, 0 });

    const auto n = m_bodies.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            const auto& lhs = m_bodies[i];
            const auto& rhs = m_bodies[j];
            const auto force = lhs.GetForce(rhs);
            forces[i] = Add(forces[i], force);
            forces[j] = Add(forces[i], Vec3{ -force.x, -force.y, -force.z });
        }
    }

    for (size_t i = 0; i < n; ++i) {
        m_bodies[i].Update(deltaTime, forces[i]);
    }
}
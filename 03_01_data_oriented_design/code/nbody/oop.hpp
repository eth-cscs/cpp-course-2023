#pragma once

#include "common.hpp"

#include <utility>
#include <vector>


struct Vec3 {
    float x;
    float y;
    float z;
};


float Distance(const Vec3& lhs, const Vec3& rhs);

float GravitationalForce(float mass1, float mass2, float distance);


class Body {
public:
    Body(Vec3 pos, Vec3 velocity, float mass) : m_pos(pos), m_velocity(velocity), m_mass(mass) {}

    Vec3 GetForce(const Body& other) const;
    void Update(float deltaTime, const Vec3& force);
    Vec3 GetPos() const { return m_pos; }
    Vec3 GetVelocity() const { return m_velocity; }
    float GetMass() const { return m_mass; }

private:
    Vec3 m_pos;
    Vec3 m_velocity;
    float m_mass;
};


class SimulationOop {
public:
    SimulationOop(std::vector<Body> bodies) : m_bodies(std::move(bodies)) {}

    void Update(float deltaTime);
    const std::vector<Body>& GetBodies() const { return m_bodies; }

private:
    std::vector<Body> m_bodies;
};
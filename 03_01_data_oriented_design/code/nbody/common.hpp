#pragma once

inline constexpr float G = 6.67430e-11;


inline float GravitationalForce(float mass1, float mass2, float distance) {
    return G * mass1 * mass2 / (distance * distance);
}


inline float Integrate(float quantity, float derivative, float deltaTime) {
    return quantity + derivative * deltaTime;
}
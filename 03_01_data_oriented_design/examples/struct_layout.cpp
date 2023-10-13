// Example of various approaches for data oriented programming
// see the tips in the data_oriented_tips slides
// this is a basis to experiment
// think about the data layout
// try removing the PKind from the structure foe example
// think about how the array of structs approach might become
// better/competitive
#include <algorithm>
#include <compare>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <random>
#include <ranges>
#include <string>
#include <vector>

enum class PKind : std::uint8_t {
    Normal,
    Floating,
    WithDrag
};

// # initial naive implementation

struct Particle {
    PKind kind;
    double pos_x;
    bool fixed_x;
    double pos_y;
    bool fixed_y;
    double pos_z;
    bool fixed_z;
    float mass;
    double v_x;
    double v_y;
    bool v_z_boost;
    double v_z;
    int charge;
    double f_x;
    double f_y;
    double f_z;


    void evalForce(double epsilon, double field_x, double field_y, double field_z) {
        if (fixed_x) {
            f_x = 0;
        }
        else {
            f_x = charge * epsilon * (v_y * field_z - v_z * field_y);
        }
        if (fixed_y) {
            f_y = 0;
        }
        else {
            f_y = charge * epsilon * (-v_x * field_z + v_z * field_x);
        }
        if (fixed_z) {
            f_z = 0;
        }
        else {
            f_z = charge * epsilon * (v_x * field_y - v_y * field_x);
        }
        const double floatiness = 0.004;
        switch (kind) {
            case PKind::Normal:
                break;
            case PKind::Floating:
                f_z += -floatiness * pos_z;
                break;
            case PKind::WithDrag:
                if (!fixed_x) {
                    f_x += -0.1 * mass * v_x;
                }
                if (!fixed_y) {
                    f_y += -0.1 * mass * v_y;
                }
                if (!fixed_z) {
                    f_z += -0.1 * mass * v_z;
                }
                break;
        }
    }

    void integrate(double dt) {
        double a_x = f_x / mass;
        double a_y = f_y / mass;
        double a_z = f_z / mass * (v_z_boost ? 2 : 1);
        v_x += a_x * dt / 2;
        v_y += a_y * dt / 2;
        v_z += a_z * dt / 2;
        pos_x += v_x * dt;
        pos_y += v_y * dt;
        pos_z += v_z * dt;
        v_x += a_x * dt / 2;
        v_y += a_y * dt / 2;
        v_z += a_z * dt / 2;
    }

    Particle() = default;

    explicit Particle(std::mt19937 gen) {
        std::uniform_int_distribution<int> kindG(0, 2);
        std::uniform_real_distribution<double> unitBox(-1.0, 1.0);
        std::uniform_real_distribution<double> massG(1.0, 6.0);
        std::uniform_int_distribution<int> boolG(0, 9);
        std::uniform_int_distribution<int> chargeG(-1, 1);
        std::normal_distribution<double> vG{ 0.0, 1.0 };
        kind = PKind(kindG(gen));
        pos_x = unitBox(gen);
        fixed_x = boolG(gen) == 0;
        pos_y = unitBox(gen);
        fixed_y = boolG(gen) == 0;
        pos_z = unitBox(gen);
        fixed_z = boolG(gen) == 0;
        mass = massG(gen);
        v_x = vG(gen);
        v_y = vG(gen);
        v_z_boost = boolG(gen) == 0;
        v_z = vG(gen);
        charge = chargeG(gen);
        f_x = 0.0;
        f_y = 0.0;
        f_z = 0.0;
    }
};

struct System {
    std::vector<Particle> particles;

    System(const std::vector<Particle>& p) : particles(p) {}

    void advance() {
        for (Particle& p : particles) {
            p.evalForce(0.01, 1.0, 1.0, 1.0);
        }
        for (Particle& p : particles) {
            p.integrate(0.01);
        }
    }

    double sumCenterOfMass() const {
        double c_mass_x = 0.0;
        double c_mass_y = 0.0;
        double c_mass_z = 0.0;
        double tot_mass = 0.0;
        for (const Particle& p : particles) {
            c_mass_x += p.mass * p.pos_x;
            c_mass_y += p.mass * p.pos_y;
            c_mass_z += p.mass * p.pos_z;
            tot_mass += p.mass;
        }
        c_mass_x /= tot_mass;
        c_mass_y /= tot_mass;
        c_mass_z /= tot_mass;
        return c_mass_x + c_mass_y + c_mass_z;
    }
};

// # Optimized layout & alignement
struct Particle2 {
    alignas(16) double mass;
    double pos_x;
    double pos_y;
    double pos_z;
    double v_x;
    double v_y;
    double v_z;
    double f_x;
    double f_y;
    double f_z;
    int charge;
    PKind kind;
    bool fixed_x;
    bool fixed_y;
    bool fixed_z;
    bool v_z_boost;


    void evalForce(double epsilon, double field_x, double field_y, double field_z) {
        if (fixed_x) {
            f_x = 0;
        }
        else {
            f_x = charge * epsilon * (v_y * field_z - v_z * field_y);
        }
        if (fixed_y) {
            f_y = 0;
        }
        else {
            f_y = charge * epsilon * (-v_x * field_z + v_z * field_x);
        }
        if (fixed_z) {
            f_z = 0;
        }
        else {
            f_z = charge * epsilon * (v_x * field_y - v_y * field_x);
        }
        const double floatiness = 0.004;
        switch (kind) {
            case PKind::Normal:
                break;
            case PKind::Floating:
                f_z += -floatiness * pos_z;
                break;
            case PKind::WithDrag:
                if (!fixed_x) {
                    f_x += -0.1 * mass * v_x;
                }
                if (!fixed_y) {
                    f_y += -0.1 * mass * v_y;
                }
                if (!fixed_z) {
                    f_z += -0.1 * mass * v_z;
                }
                break;
        }
    }

    void integrate(double dt) {
        double a_x = f_x / mass;
        double a_y = f_y / mass;
        double a_z = f_z / mass * (v_z_boost ? 2 : 1);
        v_x += a_x * dt / 2;
        v_y += a_y * dt / 2;
        v_z += a_z * dt / 2;
        pos_x += v_x * dt;
        pos_y += v_y * dt;
        pos_z += v_z * dt;
        v_x += a_x * dt / 2;
        v_y += a_y * dt / 2;
        v_z += a_z * dt / 2;
    }

    Particle2(const Particle& p) : mass(p.mass),
                                   pos_x(p.pos_x),
                                   pos_y(p.pos_y),
                                   pos_z(p.pos_z),
                                   v_x(p.v_x),
                                   v_y(p.v_y),
                                   v_z(p.v_z),
                                   f_x(p.f_x),
                                   f_y(p.f_y),
                                   f_z(p.f_z),
                                   kind(p.kind),
                                   charge(p.charge),
                                   fixed_x(p.fixed_x),
                                   fixed_y(p.fixed_y),
                                   fixed_z(p.fixed_z),
                                   v_z_boost(p.v_z_boost) {}

    Particle2() = default;
};

struct System2 {
    std::vector<Particle2> particles;

    System2(const std::vector<Particle2>& p) : particles(p) {}

    void advance() {
        for (Particle2& p : particles) {
            p.evalForce(0.01, 1.0, 1.0, 1.0);
        }
        for (Particle2& p : particles) {
            p.integrate(0.01);
        }
    }

    double sumCenterOfMass() const {
        double c_mass_x = 0.0;
        double c_mass_y = 0.0;
        double c_mass_z = 0.0;
        double tot_mass = 0.0;
        for (const Particle2& p : particles) {
            c_mass_x += p.mass * p.pos_x;
            c_mass_y += p.mass * p.pos_y;
            c_mass_z += p.mass * p.pos_z;
            tot_mass += p.mass;
        }
        c_mass_x /= tot_mass;
        c_mass_y /= tot_mass;
        c_mass_z /= tot_mass;
        return c_mass_x + c_mass_y + c_mass_z;
    }
};

// # removed kind thanks to centralized handling

struct Particle3 {
    alignas(16) double mass;
    double pos_x;
    double pos_y;
    double pos_z;
    double v_x;
    double v_y;
    double v_z;
    double f_x;
    double f_y;
    double f_z;
    int charge;
    bool fixed_x;
    bool fixed_y;
    bool fixed_z;
    bool v_z_boost;

    Particle3(const Particle& p) : mass(p.mass),
                                   pos_x(p.pos_x),
                                   pos_y(p.pos_y),
                                   pos_z(p.pos_z),
                                   v_x(p.v_x),
                                   v_y(p.v_y),
                                   v_z(p.v_z),
                                   f_x(p.f_x),
                                   f_y(p.f_y),
                                   f_z(p.f_z),
                                   charge(p.charge),
                                   fixed_x(p.fixed_x),
                                   fixed_y(p.fixed_y),
                                   fixed_z(p.fixed_z),
                                   v_z_boost(p.v_z_boost) {}

    Particle3() = default;
};


struct System3 {
    std::vector<Particle3> particles;
    size_t nNormal, nFloatingNormal;

    System3(const std::vector<Particle>& p) {
        Particle testParticle;
        testParticle.kind = PKind::Normal;
        auto normalEnd = std::upper_bound(p.cbegin(), p.cend(), testParticle,
                                          [](const auto& p1, const auto& p2) -> bool { return p1.kind < p2.kind; });
        nNormal = normalEnd - p.cbegin();
        testParticle.kind = PKind::Floating;
        auto floatingEnd = std::upper_bound(p.cbegin(), p.cend(), testParticle,
                                            [](const auto& p1, const auto& p2) -> bool { return p1.kind < p2.kind; });
        nFloatingNormal = floatingEnd - p.cbegin();
        particles.reserve(p.size());
        for (const Particle& particle : p) {
            particles.emplace_back(particle);
        }
    }

    void advance() {
        evalForce(0.01, 1.0, 1.0, 1.0);
        integrate(0.01);
    }

    double sumCenterOfMass() const {
        double c_mass_x = 0.0;
        double c_mass_y = 0.0;
        double c_mass_z = 0.0;
        double tot_mass = 0.0;
        for (const Particle3& p : particles) {
            c_mass_x += p.mass * p.pos_x;
            c_mass_y += p.mass * p.pos_y;
            c_mass_z += p.mass * p.pos_z;
            tot_mass += p.mass;
        }
        c_mass_x /= tot_mass;
        c_mass_y /= tot_mass;
        c_mass_z /= tot_mass;
        return c_mass_x + c_mass_y + c_mass_z;
    }

    void evalForce(double epsilon, double field_x, double field_y, double field_z) {
        for (size_t i = 0; i < nNormal; ++i) {
            Particle3& p = particles[i];
            if (p.fixed_x)
                p.f_x = 0;
            else
                p.f_x = p.charge * epsilon * (p.v_y * field_z - p.v_z * field_y);
            if (p.fixed_y)
                p.f_y = 0;
            else
                p.f_y = p.charge * epsilon * (-p.v_x * field_z + p.v_z * field_x);
            if (p.fixed_z)
                p.f_z = 0;
            else
                p.f_z = p.charge * epsilon * (p.v_x * field_y - p.v_y * field_x);
        }
        const double floatiness = 0.004;

        for (size_t i = nNormal; i < nFloatingNormal; ++i) {
            Particle3& p = particles[i];
            if (p.fixed_x)
                p.f_x = 0;
            else
                p.f_x = p.charge * epsilon * (p.v_y * field_z - p.v_z * field_y);
            if (p.fixed_y)
                p.f_y = 0;
            else
                p.f_y = p.charge * epsilon * (-p.v_x * field_z + p.v_z * field_x);
            if (p.fixed_z)
                p.f_z = 0;
            else
                p.f_z = p.charge * epsilon * (p.v_x * field_y - p.v_y * field_x)
                        - floatiness * p.pos_z;
        }
        for (size_t i = nFloatingNormal; i < particles.size(); ++i) {
            Particle3& p = particles[i];
            if (p.fixed_x)
                p.f_x = 0;
            else
                p.f_x = p.charge * epsilon * (p.v_y * field_z - p.v_z * field_y)
                        - 0.1 * p.mass * p.v_x;

            if (p.fixed_y)
                p.f_y = 0;
            else
                p.f_y = p.charge * epsilon * (-p.v_x * field_z + p.v_z * field_x)
                        - 0.1 * p.mass * p.v_y;
            if (p.fixed_z)
                p.f_z = 0;
            else
                p.f_z = p.charge * epsilon * (p.v_x * field_y - p.v_y * field_x)
                        - 0.1 * p.mass * p.v_z;
        }
    }

    void integrate(double dt) {
        for (Particle3& p : particles) {
            double a_x = p.f_x / p.mass;
            double a_y = p.f_y / p.mass;
            double a_z = p.f_z / p.mass * (p.v_z_boost ? 2 : 1);
            p.v_x += a_x * dt / 2;
            p.v_y += a_y * dt / 2;
            p.v_z += a_z * dt / 2;
            p.pos_x += p.v_x * dt;
            p.pos_y += p.v_y * dt;
            p.pos_z += p.v_z * dt;
            p.v_x += a_x * dt / 2;
            p.v_y += a_y * dt / 2;
            p.v_z += a_z * dt / 2;
        }
    }
};

// # Struct of arrays

struct System4 {
    size_t n_particles;
    std::vector<float> mass;
    std::vector<double> pos;
    std::vector<double> v;
    std::vector<double> f;
    std::vector<bool> fixed_pos;
    std::vector<bool> v_z_boost;
    std::vector<int> charge;
    std::vector<PKind> kind;

    System4(const std::vector<Particle>& particles) : n_particles(particles.size()),
                                                      mass(particles.size()),
                                                      pos(3 * particles.size()),
                                                      v(3 * particles.size()),
                                                      f(3 * particles.size()),
                                                      fixed_pos(3 * particles.size()),
                                                      v_z_boost(particles.size()),
                                                      charge(particles.size()),
                                                      kind(particles.size()) {
        for (size_t i = 0; i < particles.size(); ++i) {
            size_t i3 = 3 * i;
            const Particle& p = particles[i];
            mass[i] = p.mass;
            pos[i3] = p.pos_x;
            pos[i3 + 1] = p.pos_y;
            pos[i3 + 2] = p.pos_z;
            v[i3] = p.v_x;
            v[i3 + 1] = p.v_y;
            v[i3 + 2] = p.v_z;
            f[i3] = p.f_x;
            f[i3 + 1] = p.f_y;
            f[i3 + 2] = p.f_z;
            fixed_pos[i3] = p.fixed_x;
            fixed_pos[i3 + 1] = p.fixed_y;
            fixed_pos[i3 + 2] = p.fixed_z;
            v_z_boost[i] = p.v_z_boost;
            charge[i] = p.charge;
            kind[i] = p.kind;
        }
    }

    void advance() {
        evalForce(0.01, 1.0, 1.0, 1.0);
        integrate(0.01);
    }

    double sumCenterOfMass() const {
        double c_mass_x = 0.0;
        double c_mass_y = 0.0;
        double c_mass_z = 0.0;
        double tot_mass = 0.0;
        for (size_t i = 0; i < n_particles; ++i) {
            size_t i3 = 3 * i;
            c_mass_x += mass[i] * pos[i3];
            c_mass_y += mass[i] * pos[i3 + 1];
            c_mass_z += mass[i] * pos[i3 + 2];
            tot_mass += mass[i];
        }
        c_mass_x /= tot_mass;
        c_mass_y /= tot_mass;
        c_mass_z /= tot_mass;
        return c_mass_x + c_mass_y + c_mass_z;
    }

    void evalForce(double epsilon, double field_x, double field_y, double field_z) {
        for (size_t i = 0; i < n_particles; ++i) {
            size_t i3 = 3 * i;
            if (fixed_pos[i3]) {
                f[i3] = 0;
            }
            else {
                f[i3] = charge[i] * epsilon * (v[i3 + 1] * field_z - v[i3 + 2] * field_y);
            }
            if (fixed_pos[i3 + 1]) {
                f[i3 + 1] = 0;
            }
            else {
                f[i3 + 1] = charge[i] * epsilon * (-v[i3] * field_z + v[i3 + 2] * field_x);
            }
            if (fixed_pos[i3 + 2]) {
                f[i3 + 2] = 0;
            }
            else {
                f[i3 + 2] = charge[i] * epsilon * (v[i3] * field_y - v[i3 + 1] * field_x);
            }
            const double floatiness = 0.004;
            switch (kind[i]) {
                case PKind::Normal:
                    break;
                case PKind::Floating:
                    f[i3 + 2] += -floatiness * pos[i3 + 2];
                    break;
                case PKind::WithDrag:
                    if (!fixed_pos[i3]) {
                        f[i3] += -0.1 * mass[i] * v[i3];
                    }
                    if (!fixed_pos[i3 + 1]) {
                        f[i3 + 1] += -0.1 * mass[i] * v[i3 + 1];
                    }
                    if (!fixed_pos[i3 + 2]) {
                        f[i3 + 2] += -0.1 * mass[i] * v[i3 + 2];
                    }
                    break;
            }
        }
    }

    void integrate(double dt) {
        for (size_t i = 0; i < n_particles; ++i) {
            size_t i3 = 3 * i;
            double a_x = f[i3] / mass[i];
            double a_y = f[i3 + 1] / mass[i];
            double a_z = f[i3 + 2] / mass[i] * (v_z_boost[i] ? 2 : 1);
            v[i3] += a_x * dt / 2;
            v[i3 + 1] += a_y * dt / 2;
            v[i3 + 2] += a_z * dt / 2;
            pos[i3] += v[i3] * dt;
            pos[i3 + 1] += v[i3 + 1] * dt;
            pos[i3 + 2] += v[i3 + 2] * dt;
            v[i3] += a_x * dt / 2;
            v[i3 + 1] += a_y * dt / 2;
            v[i3 + 2] += a_z * dt / 2;
        }
    }
};

// # struct of arrays with multiple loops, and removing kind

struct System5 {
    size_t n_particles;
    std::vector<float> mass;
    std::vector<double> pos;
    std::vector<double> v;
    std::vector<double> f;
    std::vector<bool> fixed_pos;
    std::vector<bool> v_z_boost;
    std::vector<int> charge;
    std::vector<PKind> kind;

    System5(const std::vector<Particle>& particles) : n_particles(particles.size()),
                                                      mass(particles.size()),
                                                      pos(3 * particles.size()),
                                                      v(3 * particles.size()),
                                                      f(3 * particles.size()),
                                                      fixed_pos(3 * particles.size()),
                                                      v_z_boost(particles.size()),
                                                      charge(particles.size()),
                                                      kind(particles.size()) {
        for (size_t i = 0; i < particles.size(); ++i) {
            size_t i3 = 3 * i;
            const Particle& p = particles[i];
            mass[i] = p.mass;
            pos[i3] = p.pos_x;
            pos[i3 + 1] = p.pos_y;
            pos[i3 + 2] = p.pos_z;
            v[i3] = p.v_x;
            v[i3 + 1] = p.v_y;
            v[i3 + 2] = p.v_z;
            f[i3] = p.f_x;
            f[i3 + 1] = p.f_y;
            f[i3 + 2] = p.f_z;
            fixed_pos[i3] = p.fixed_x;
            fixed_pos[i3 + 1] = p.fixed_y;
            fixed_pos[i3 + 2] = p.fixed_z;
            v_z_boost[i] = p.v_z_boost;
            charge[i] = p.charge;
            kind[i] = p.kind;
        }
    }

    void advance() {
        evalForce(0.01, 1.0, 1.0, 1.0);
        integrate(0.01);
    }

    double sumCenterOfMass() const {
        double c_mass_x = 0.0;
        double c_mass_y = 0.0;
        double c_mass_z = 0.0;
        double tot_mass = 0.0;
        for (size_t i = 0; i < n_particles; ++i) {
            size_t i3 = 3 * i;
            c_mass_x += mass[i] * pos[i3];
            c_mass_y += mass[i] * pos[i3 + 1];
            c_mass_z += mass[i] * pos[i3 + 2];
            tot_mass += mass[i];
        }
        c_mass_x /= tot_mass;
        c_mass_y /= tot_mass;
        c_mass_z /= tot_mass;
        return c_mass_x + c_mass_y + c_mass_z;
    }

    void evalForce(double epsilon, double field_x, double field_y, double field_z) {
        auto normalEnd = std::upper_bound(kind.cbegin(), kind.cend(), PKind::Normal);
        size_t nNormal = normalEnd - kind.cbegin();
        auto floatingEnd = std::upper_bound(kind.cbegin(), kind.cend(), PKind::Floating);
        size_t nFloatingNormal = floatingEnd - kind.cbegin();
        for (size_t i = 0; i < nFloatingNormal; ++i) {
            size_t i3 = 3 * i;
            f[i3] = charge[i] * epsilon * (v[i3 + 1] * field_z - v[i3 + 2] * field_y);
            f[i3 + 1] = charge[i] * epsilon * (-v[i3] * field_z + v[i3 + 2] * field_x);
            f[i3 + 2] = charge[i] * epsilon * (v[i3] * field_y - v[i3 + 1] * field_x);
        }
        const double floatiness = 0.004;
        for (size_t i = nNormal; i < nFloatingNormal; ++i) {
            size_t i3 = 3 * i;
            f[i3 + 2] += -floatiness * pos[i3 + 2];
        }
        for (size_t i = nFloatingNormal; i < n_particles; ++i) {
            size_t i3 = 3 * i;
            f[i3] = charge[i] * epsilon * (v[i3 + 1] * field_z - v[i3 + 2] * field_y)
                    - 0.1 * mass[i] * v[i3];
            f[i3 + 1] = charge[i] * epsilon * (-v[i3] * field_z + v[i3 + 2] * field_x)
                        - 0.1 * mass[i] * v[i3 + 1];
            f[i3 + 2] = charge[i] * epsilon * (v[i3] * field_y - v[i3 + 1] * field_x)
                        - 0.1 * mass[i] * v[i3 + 2];
        }
        for (size_t i = 0; i < 3 * n_particles; ++i) {
            if (fixed_pos[i])
                f[i] = 0;
        }
    }

    void integrate(double dt) {
        for (size_t i = 0; i < n_particles; ++i) {
            size_t i3 = 3 * i;
            double dv_x = f[i3] / mass[i] * dt / 2;
            double dv_y = f[i3 + 1] / mass[i] * dt / 2;
            double dv_z = f[i3 + 2] / mass[i] * (v_z_boost[i] ? 2 : 1) * dt / 2;
            v[i3] += dv_x;
            v[i3 + 1] += dv_y;
            v[i3 + 2] += dv_z;
            pos[i3] += v[i3] * dt;
            pos[i3 + 1] += v[i3 + 1] * dt;
            pos[i3 + 2] += v[i3 + 2] * dt;
            v[i3] += dv_x;
            v[i3 + 1] += dv_y;
            v[i3 + 2] += dv_z;
        }
    }
};

#include <catch2/catch_all.hpp>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("StructLayout") {
    auto n = GENERATE(1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144);
    size_t N = size_t(n);
    CAPTURE(N);
    SECTION("nParticles " + std::to_string(N)) {
        const unsigned int seed = Catch::getSeed();
        std::mt19937 gen;
        gen.seed(seed);
        std::vector<Particle> startV(N);
        for (Particle& p : startV) {
            p = Particle(gen);
        }
        System sys{ startV };
        std::vector<Particle2> startV2(N);
        for (size_t i = 0; i < N; ++i) {
            startV2[i] = Particle2(startV[i]);
        }
        std::vector<Particle> startV3(startV);
        std::stable_sort(startV3.begin(), startV3.end(), [](const Particle& p1, const Particle& p2) -> bool {
            return uint8_t(p1.kind) < uint8_t(p2.kind);
        });
        System2 sys2{ startV2 };
        System3 sys3(startV3);
        System4 sys4{ startV };
        System5 sys5{ startV3 };
        REQUIRE(sys.particles.size() == N);
        REQUIRE(sys2.particles.size() == N);
        REQUIRE(sys3.particles.size() == N);
        REQUIRE(sys4.n_particles == N);
        REQUIRE(sys5.n_particles == N);
        for (size_t i = 0; i < startV.size(); ++i) {
            REQUIRE_THAT(sys4.pos.at(3 * i), WithinAbs(startV.at(i).pos_x, 1.0e-12));
            REQUIRE_THAT(sys4.pos.at(3 * i + 1), WithinAbs(startV.at(i).pos_y, 1.0e-12));
            REQUIRE_THAT(sys4.pos.at(3 * i + 2), WithinAbs(startV.at(i).pos_z, 1.0e-12));
            REQUIRE_THAT(sys4.v.at(3 * i), WithinAbs(startV.at(i).v_x, 1.0e-12));
            REQUIRE_THAT(sys4.v.at(3 * i + 1), WithinAbs(startV.at(i).v_y, 1.0e-12));
            REQUIRE_THAT(sys4.v.at(3 * i + 2), WithinAbs(startV.at(i).v_z, 1.0e-12));
            REQUIRE_THAT(sys4.mass.at(i), WithinAbs(startV.at(i).mass, 1.0e-12));
            REQUIRE_THAT(sys4.charge.at(i), WithinAbs(startV.at(i).charge, 1.0e-12));
        }

        double cMass = sys.sumCenterOfMass();
        REQUIRE_THAT(sys2.sumCenterOfMass(), WithinAbs(cMass, 1.0e-12));
        REQUIRE_THAT(sys3.sumCenterOfMass(), WithinAbs(cMass, 1.0e-12));
        REQUIRE_THAT(sys4.sumCenterOfMass(), WithinAbs(cMass, 1.0e-12));
        REQUIRE_THAT(sys5.sumCenterOfMass(), WithinAbs(cMass, 1.0e-12));
        sys.advance();
        sys2.advance();
        sys3.advance();
        sys4.advance();
        sys5.advance();
        cMass = sys.sumCenterOfMass();
        REQUIRE_THAT(sys2.sumCenterOfMass(), WithinAbs(cMass, 1.0e-10) || WithinRel(cMass));
        REQUIRE_THAT(sys3.sumCenterOfMass(), WithinAbs(cMass, 1.0e-10) || WithinRel(cMass));
        REQUIRE_THAT(sys4.sumCenterOfMass(), WithinAbs(cMass, 1.0e-10) || WithinRel(cMass));
        REQUIRE_THAT(sys5.sumCenterOfMass(), WithinAbs(cMass, 1.0e-10) || WithinRel(cMass));
        const int steps = 2;

        BENCHMARK("originalLayout") {
            for (int i = 0; i < steps; ++i)
                sys.advance();
            return sys.sumCenterOfMass();
        };
        BENCHMARK("betterLayout") {
            for (int i = 0; i < steps; ++i)
                sys2.advance();
            return sys2.sumCenterOfMass();
        };
        BENCHMARK("betterLayoutCentralized") {
            for (int i = 0; i < steps; ++i)
                sys3.advance();
            return sys3.sumCenterOfMass();
        };
        BENCHMARK("CentralizedSoA") {
            for (int i = 0; i < steps; ++i)
                sys4.advance();
            return sys4.sumCenterOfMass();
        };
        BENCHMARK("MultiLoopSoA") {
            for (int i = 0; i < steps; ++i)
                sys5.advance();
            return sys5.sumCenterOfMass();
        };
    }
}

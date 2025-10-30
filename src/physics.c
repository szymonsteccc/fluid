#include "physics.h"
#include "particle.h"
#include <SDL3/SDL.h>

#include <math.h>
#include <stdlib.h>


float CalculateDistance(Particle a, Particle b) {
    float dx = (float)b.position.x - (float)a.position.x;
    float dy = (float)b.position.y - (float)a.position.y;
    return (float)sqrtf(dx * dx + dy * dy);
}

float CalculateDistanceMouse(Particle a, float mouse_x, float mouse_y) {
    float dx = mouse_x - (float)a.position.x;
    float dy = mouse_y - (float)a.position.y;
    return (float)sqrtf(dx * dx + dy * dy);
}

bool circles_intersect(Particle a, Particle b) {
    float distance = CalculateDistance(a, b);
    return (distance < 2*RADIUS);
}

void resolve_colisions(Particle *p) {
    // wall collisions
    if (p->position.x - RADIUS < 0) {
        p->position.x = RADIUS;
        p->velocity.x *= -1  * COLLISION_DAMPING;
    } else if (p->position.x + RADIUS > WINDOW_WIDTH) {
        p->position.x = WINDOW_WIDTH - RADIUS;
        p->velocity.x *= -1  * COLLISION_DAMPING;
    }

    if (p->position.y - RADIUS < 0) {
        p->position.y = RADIUS;
        p->velocity.y *= -1  * COLLISION_DAMPING;
    } else if (p->position.y + RADIUS > WINDOW_HEIGHT) {
        p->position.y = WINDOW_HEIGHT - RADIUS;
        p->velocity.y *= -1 * COLLISION_DAMPING;
    }

    // particle-particle separation + simple collision response
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle *other = &particles[i];
        if (p == other) continue;

        float dist = CalculateDistance(*p, *other);
        if (dist <= 0.0f) continue;
        if (dist < 2 * RADIUS) {
            float overlap = (2 * RADIUS - dist) / 2.0f;
            float nx = (p->position.x - other->position.x) / dist;
            float ny = (p->position.y - other->position.y) / dist;

            p->position.x += nx * overlap;
            p->position.y += ny * overlap;
            other->position.x -= nx * overlap;
            other->position.y -= ny * overlap;

            float tx = -ny;
            float ty = nx;

            float dpTan1 = p->velocity.x * tx + p->velocity.y * ty;
            float dpTan2 = other->velocity.x * tx + other->velocity.y * ty;

            float dpNorm1 = p->velocity.x * nx + p->velocity.y * ny;
            float dpNorm2 = other->velocity.x * nx + other->velocity.y * ny;

            float m1 = (dpNorm1 * (PARTICLE_MASS - PARTICLE_MASS) + 2.0f * PARTICLE_MASS * dpNorm2) / (PARTICLE_MASS + PARTICLE_MASS);
            float m2 = (dpNorm2 * (PARTICLE_MASS - PARTICLE_MASS) + 2.0f * PARTICLE_MASS * dpNorm1) / (PARTICLE_MASS + PARTICLE_MASS);

            p->velocity.x = tx * dpTan1 + nx * m1 * COLLISION_DAMPING;
            p->velocity.y = ty * dpTan1 + ny * m1 * COLLISION_DAMPING;
            other->velocity.x = tx * dpTan2 + nx * m2 * COLLISION_DAMPING;
            other->velocity.y = ty * dpTan2 + ny * m2 * COLLISION_DAMPING;
        }
    }
}

float SmoothingFunction(float dst) {
    if (dst >= SMOOTHING_RADIUS) { return 0.0f; }
    float volume = (M_PI * powf(SMOOTHING_RADIUS, 4.0f)) / 6.0f;
    float t = (SMOOTHING_RADIUS - dst);
    return (t * t) / volume;
}

float SmoothingFunctionDerivative(float dst) {
    if (dst >= SMOOTHING_RADIUS) { return 0.0f; }
    float scale = 12.0f / (powf(SMOOTHING_RADIUS, 4.0f) * M_PI);
    return (dst - SMOOTHING_RADIUS) * scale;
}

void CalculateParticleDensity(int particle_index) {
    particles[particle_index].density = 0.0f;
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        float distance = CalculateDistance(particles[particle_index], particles[i]);
        float influence = SmoothingFunction(distance);
        particles[particle_index].density += PARTICLE_MASS * influence;
    }
    if (particles[particle_index].density < TARGET_DENSITY) {
        particles[particle_index].density = TARGET_DENSITY;
    }
}

void CalculateDensities() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        CalculateParticleDensity(i);
    }
}

float convert_density_to_pressure(float density) {
    float density_error = density - TARGET_DENSITY;
    return PRESSURE_COEFFICIENT * density_error;
}

float CalculateSharedPressure(float density_A, float density_B) {
    float pressure_A = convert_density_to_pressure(density_A);
    float pressure_B = convert_density_to_pressure(density_B);
    return (pressure_A + pressure_B) / 2.0f;
}

Vector2 CalculatePressureForce(int particle_index) {
    Vector2 pressure_force = VECTOR2_ZERO;
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        float distance = CalculateDistance(particles[particle_index], particles[i]);
        if (distance <= 0.0f || distance >= SMOOTHING_RADIUS) { continue; }

        Vector2 dir = {
            (particles[i].position.x - particles[particle_index].position.x) / distance,
            (particles[i].position.y - particles[particle_index].position.y) / distance
        };

        float slope = SmoothingFunctionDerivative(distance);
        float shared_pressure = CalculateSharedPressure(particles[particle_index].density, particles[i].density);

        if (particles[i].density <= 0.0f) continue;
        pressure_force.x += shared_pressure * dir.x * slope * PARTICLE_MASS / particles[i].density;
        pressure_force.y += shared_pressure * dir.y * slope * PARTICLE_MASS / particles[i].density;
    }
    return pressure_force;
}

Vector2 CalculatePressureForceMouse(int particle_index) {
    Vector2 pressure_force_mouse = VECTOR2_ZERO;
    Vector2 mouse_cord;
    SDL_GetGlobalMouseState(&mouse_cord.x, &mouse_cord.y);

    float distance = CalculateDistanceMouse(particles[particle_index], mouse_cord.x, mouse_cord.y);
    // if (distance <= 0.0f) return pressure_force_mouse;
    Vector2 dir = {
        (mouse_cord.x - particles[particle_index].position.x) / distance,
        (mouse_cord.y - particles[particle_index].position.y) / distance
    };

    float slope = SmoothingFunctionDerivative(distance);
    float pressure = convert_density_to_pressure(distance);

    pressure_force_mouse.x -= pressure * dir.x * slope * PARTICLE_MASS / 10;
    pressure_force_mouse.y -= pressure * dir.y * slope * PARTICLE_MASS / 10;
    return pressure_force_mouse;
}

void applyInertia(float dx, float dy) {
    const float INERTIA_SCALE = 0.1f;
    float ix = -dx * INERTIA_SCALE;
    float iy = -dy * INERTIA_SCALE;
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].velocity.x += ix;
        particles[i].velocity.y += iy;
    }
}

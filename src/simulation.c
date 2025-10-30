#include "simulation.h"
#include "particle.h"
#include "physics.h"
#include <SDL3/SDL.h>

#include <stdlib.h>
#include <time.h>


void update_simulation(SDL_Renderer *renderer, float time_delta, float dx, float dy) {
    SDL_Color color = {255, 0, 0, 255};

    // gravity + densities
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].velocity.x += VECTOR2_DOWN.x * GRAVITY * time_delta;
        particles[i].velocity.y += VECTOR2_DOWN.y * GRAVITY * time_delta;
        CalculateParticleDensity(i);
    }

    // pressure / mouse forces -> velocity
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Vector2 pressure_force = CalculatePressureForce(i);
        Vector2 pressure_acceleration = {
            pressure_force.x / particles[i].density,
            pressure_force.y / particles[i].density,
        };
        particles[i].velocity.x = pressure_acceleration.x * time_delta;
        particles[i].velocity.y = pressure_acceleration.y * time_delta;

        Vector2 pressure_force_mouse = CalculatePressureForceMouse(i);
        Vector2 pressure_acceleration_mouse = {
            pressure_force_mouse.x / particles[i].density,
            pressure_force_mouse.y / particles[i].density,
        };
        particles[i].velocity.x += pressure_acceleration_mouse.x * time_delta;
        particles[i].velocity.y += pressure_acceleration_mouse.y * time_delta;
    }

    // optional global inertia from window movement
    if (dx != 0.0f || dy != 0.0f) {
        applyInertia(dx, dy);
    }

    // integrate + collisions + render
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].position.x += particles[i].velocity.x * time_delta;
        particles[i].position.y += particles[i].velocity.y * time_delta;
        resolve_colisions(&particles[i]);
        draw_circle(renderer, particles[i].position.x, particles[i].position.y, color);
    }
}
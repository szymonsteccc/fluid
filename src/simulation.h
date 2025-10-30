#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_sensor.h>

#include <stdbool.h>
#include <stddef.h>


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

static const int RADIUS = 10;
static const int CIRCLE_SEGMENTS = 32;
// static const int SIMULATION_DELAY = 200;
static const float GRAVITY = SDL_STANDARD_GRAVITY; // SDL_STANDARD_GRAVITY
static const int PARTICLE_COUNT = 300;
static const float COLLISION_DAMPING = 0.8f;
static const float SMOOTHING_RADIUS = RADIUS * 6.0f;
static const float PARTICLE_MASS = 1;
static const float TARGET_DENSITY = 1.e-6f;
static const float PRESSURE_COEFFICIENT = 100000.0f;

// static const float INERTIA_SCALE = 10.0f;

static const int CIRCLE_BOUNDARY_X = WINDOW_WIDTH - RADIUS;
static const int CIRCLE_BOUNDARY_Y = WINDOW_HEIGHT - RADIUS;


typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float density;
    // Vector2 inertia;
} Particle;


static const Vector2 VECTOR2_UP    = { 0.0f, -1.0f };
static const Vector2 VECTOR2_DOWN  = { 0.0f,  1.0f };
static const Vector2 VECTOR2_LEFT  = { -1.0f, 0.0f };
static const Vector2 VECTOR2_RIGHT = {  1.0f, 0.0f };
static const Vector2 VECTOR2_ONE   = { 1.0f,  1.0f };
static const Vector2 VECTOR2_ZERO   = { 0.0f,  0.0f };


extern Particle *particles;


void init_particles();

// Particle random_particle();
// bool particle_is_used(Particle p);
// bool circles_intersect(Particle a, Particle b);

// void applyInertia(float dx, float dy);
void update_simulation(SDL_Renderer *renderer, float time_delta, float dx, float dy);
// void update(Particle *p, float time_delta);
// void draw_circle(SDL_Renderer *renderer, float centerX, float centerY, SDL_Color color);

#endif
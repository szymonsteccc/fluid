#include "simulation.h"
// #include "app.h"
#include <SDL3/SDL.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>

Particle *particles = NULL;


Particle random_particle() {
    Particle rand_particle;
    rand_particle.position.x = (rand() % (WINDOW_WIDTH - 2*RADIUS)) + RADIUS;
    rand_particle.position.y = (rand() % (WINDOW_HEIGHT - 2*RADIUS)) + RADIUS;
    rand_particle.velocity.x = 0.0f;
    rand_particle.velocity.y = 0.0f;
    rand_particle.density = 0;
    return rand_particle;
}

float CalculateDistance(Particle a, Particle b) {
    // printf("asdasd: %f\n", b.position.x);
    float dx = (float)b.position.x - (float)a.position.x;
    float dy = (float)b.position.y - (float)a.position.y;
    // printf("lol : %f\n", (float)sqrtf(dx * dx + dy * dy));
    return (float)sqrtf(dx * dx + dy * dy);
}

// not used .))
bool circles_intersect(Particle a, Particle b) {
    float distance = CalculateDistance(a, b);
    if (distance < 2*RADIUS) { return true; }
    return false;
}

void print_particle(Particle *p) {
    if (p == NULL) {
        printf("Particle: NULL\n");
        return;
    }
    printf("Particle { pos=(%f, %f), vel=(%f, %f), den=%f }\n",
           p->position.x, p->position.y, p->velocity.x, p->velocity.y, p->density);
}

void init_particles(){
    particles = malloc(PARTICLE_COUNT * sizeof(Particle));
    if (particles == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    srand((unsigned)time(NULL));

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle candidate;
        bool ok;
        int attempts = 0;
        do {
            candidate = random_particle();
            ok = true;
            for (int j = 0; j < i; j++) {
                if (circles_intersect(particles[j], candidate)) {
                    ok = false;
                    break;
                }
            }
            attempts++;
            if (attempts > 1000) {
                fprintf(stderr, "init_particles: too many attempts to place particle %d\n", i);
                break;
            }
        } while (!ok);
        particles[i] = candidate;
    }
}


void draw_circle(SDL_Renderer *renderer, float centerX, float centerY, SDL_Color color) {

    SDL_Vertex *verts = malloc((CIRCLE_SEGMENTS + 2) * sizeof(SDL_Vertex));

    verts[0].position.x = centerX;
    verts[0].position.y = centerY;
    verts[0].color.r = color.r;
    verts[0].color.g = color.g;
    verts[0].color.b = color.b;
    verts[0].color.a = color.a;
    verts[0].tex_coord.x = 0.0f;
    verts[0].tex_coord.y = 0.0f;

    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        float angle = (float)i / CIRCLE_SEGMENTS * (2.0f * M_PI); 
        verts[1 + i].position.x = centerX + cosf(angle) * RADIUS;
        verts[1 + i].position.y = centerY + sinf(angle) * RADIUS;
        verts[1 + i].color.r = color.r;
        verts[1 + i].color.g = color.g;
        verts[1 + i].color.b = color.b;
        verts[1 + i].color.a = color.a;
        verts[1 + i].tex_coord.x = 0.0f;
        verts[1 + i].tex_coord.y = 0.0f;
    }

    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        SDL_Vertex triangle[3] = {verts[0], verts[i], verts[1 + i]};
        SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);
    }

    free(verts);
}

void resolve_colisions(Particle *p) {

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

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle *other = &particles[i];
        if (p == other) continue;

        float dist = CalculateDistance(*p, *other);
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

            // Project velocities onto normal and tangent vectors
            float dpTan1 = p->velocity.x * tx + p->velocity.y * ty;
            float dpTan2 = other->velocity.x * tx + other->velocity.y * ty;

            float dpNorm1 = p->velocity.x * nx + p->velocity.y * ny;
            float dpNorm2 = other->velocity.x * nx + other->velocity.y * ny;

            // Conservation of momentum in 1D
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
    float pressure = PRESSURE_COEFFICIENT * density_error;
    return pressure;
}

float CalculateSharedPressure(float density_A, float density_B) {
    float pressure_A = convert_density_to_pressure(density_A);
    float pressure_B = convert_density_to_pressure(density_B);
    return (pressure_A + pressure_B) / 2;
}

Vector2 CalculatePressureForce(int particle_index) {
    Vector2 preassure_force = VECTOR2_ZERO;

    // TODO check not every particle, just those around
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        float distance = CalculateDistance(particles[particle_index], particles[i]);

        if (distance <= 0.0f || distance >= SMOOTHING_RADIUS) { continue; }

        Vector2 dir = {
            (particles[i].position.x - particles[particle_index].position.x) / distance,
            (particles[i].position.y - particles[particle_index].position.y) / distance
        };

        float slope = SmoothingFunctionDerivative(distance);

        // if (particles[i].density < TARGET_DENSITY) { continue; }

        float shared_pressure = CalculateSharedPressure(particles[particle_index].density, particles[i].density);

        preassure_force.x += shared_pressure * dir.x * slope * PARTICLE_MASS / particles[i].density;
        preassure_force.y += shared_pressure * dir.y * slope * PARTICLE_MASS / particles[i].density;
    }

    return preassure_force;
}

void update_simulation(SDL_Renderer *renderer, float time_delta) {
    SDL_Color color = {255, 0, 0, 255};

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        print_particle(&particles[i]);

        // particles[i].velocity.x += VECTOR2_DOWN.x * GRAVITY * time_delta;
        // particles[i].velocity.y += VECTOR2_DOWN.y * GRAVITY * time_delta;

        CalculateParticleDensity(i);
    }

    for (int i = 0; i < PARTICLE_COUNT; i++) {

        Vector2 pressure_force = CalculatePressureForce(i);
        Vector2 pressure_acceleration = {
            pressure_force.x / particles[i].density,
            pressure_force.y / particles[i].density,
        };

        // dodajemy przyrost prędkości zamiast nadpisywać (zachowujemy grawitację itp.)
        particles[i].velocity.x += pressure_acceleration.x * time_delta;
        particles[i].velocity.y += pressure_acceleration.y * time_delta;
    }

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].position.x += particles[i].velocity.x * time_delta;
        particles[i].position.y += particles[i].velocity.y * time_delta;
        resolve_colisions(&particles[i]);
        draw_circle(renderer, particles[i].position.x, particles[i].position.y, color);
    }

}
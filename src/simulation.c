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
    return rand_particle;
}

bool circles_intersect(Particle a, Particle b) {
    float dx = (float)b.position.x - (float)a.position.x;
    float dy = (float)b.position.y - (float)a.position.y;
    float distance = sqrtf(dx * dx + dy * dy);
    if (distance < 2*RADIUS) { return true; }
    return false;
}

// bool in_boundries(Particle *p) {
//     if (p->position.x - RADIUS >= 0 && p->position.x + RADIUS <= WINDOW_WIDTH) {
//         if (p->position.y - RADIUS >= 0 && p->position.y + RADIUS <= WINDOW_HEIGHT) {
//             return true;
//         }
//     }
//     return false;
// }

bool particle_is_used(Particle p) {
    for (size_t i = 0; i < PARTICLE_COUNT; i++) {
        if (circles_intersect(particles[i], p)) {
            return true;
        }
    }
    return false;
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
            if (attempts > 10000) {
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

bool out_of_boundries_X(Particle *p) {
    if (p->position.x - RADIUS < 0 || p->position.x + RADIUS > WINDOW_WIDTH) {
        return true;
    }
    return false;
}

bool out_of_boundries_Y(Particle *p) {
    if (p->position.y - RADIUS < 0 || p->position.y + RADIUS > WINDOW_HEIGHT) {
        return true;
    }
    return false;
}


void resolve_colisions(Particle *p) {

    if (p->position.x - RADIUS < 0) {
        p->position.x = RADIUS;
        p->velocity.x = -p->velocity.x * COLLISION_DAMPING;
    } else if (p->position.x + RADIUS > WINDOW_WIDTH) {
        p->position.x = WINDOW_WIDTH - RADIUS;
        p->velocity.x = -p->velocity.x * COLLISION_DAMPING;
    }

    if (p->position.y - RADIUS < 0) {
        p->position.y = RADIUS;
        p->velocity.y = -p->velocity.y * COLLISION_DAMPING;
    } else if (p->position.y + RADIUS > WINDOW_HEIGHT) {
        p->position.y = WINDOW_HEIGHT - RADIUS;
        p->velocity.y = -p->velocity.y * COLLISION_DAMPING;
    }
}

void update_particle(Particle *p, float dt) {
    p->velocity.x += VECTOR2_DOWN.x * GRAVITY;
    p->velocity.y += VECTOR2_DOWN.y * GRAVITY;

    float dispX = p->velocity.x * dt;
    float dispY = p->velocity.y * dt;

    p->position.x += dispX;
    p->position.y += dispY;
}

void update_simulation(SDL_Renderer *renderer, float time_delta) {
    SDL_Color color = {255, 0, 0, 255};
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        update_particle(&particles[i], time_delta);
        resolve_colisions(&particles[i]);
        draw_circle(renderer, particles[i].position.x, particles[i].position.y, color);
    }
}
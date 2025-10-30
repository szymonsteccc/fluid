#include "particle.h"
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

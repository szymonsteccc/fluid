#ifndef PARTICLE_H
#define PARTICLE_H

#include "simulation.h"
#include "physics.h"
#include <SDL3/SDL.h>

extern Particle *particles;

Particle random_particle();
void init_particles();
void print_particle(Particle *p);
void draw_circle(SDL_Renderer *renderer, float centerX, float centerY, SDL_Color color);

#endif

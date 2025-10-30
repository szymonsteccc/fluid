#ifndef PHYSICS_H
#define PHYSICS_H

#include "simulation.h"
#include <SDL3/SDL.h>

float CalculateDistance(Particle a, Particle b);
float CalculateDistanceMouse(Particle a, float mouse_x, float mouse_y);
bool circles_intersect(Particle a, Particle b);

void resolve_colisions(Particle *p);

float SmoothingFunction(float dst);
float SmoothingFunctionDerivative(float dst);

void CalculateParticleDensity(int particle_index);
void CalculateDensities();

float convert_density_to_pressure(float density);
float CalculateSharedPressure(float density_A, float density_B);

Vector2 CalculatePressureForce(int particle_index);
Vector2 CalculatePressureForceMouse(int particle_index);

void applyInertia(float dx, float dy);

#endif

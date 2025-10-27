#ifndef APP_H
#define APP_H

#include "simulation.h"
#include <SDL3/SDL_main.h>

static SDL_Window *window;
static SDL_Renderer *renderer;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
SDL_AppResult SDL_AppIterate(void *appstate);
void SDL_AppQuit(void *appstate, SDL_AppResult result);

#endif
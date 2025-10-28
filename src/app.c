#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL3/SDL_timer.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#include "app.h"
#include "simulation.h"

#include <time.h>
#include <stdlib.h>

static Uint64 last_time = 0;

// void init_particles(void);
void update_simulation(SDL_Renderer *renderer, float dt);


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Fluid", "1.0", "com.example.snake");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Fluid", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create Fluid: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    srand(rand());
    init_particles();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_KeyboardEvent *key = &event->key;

        if (key->key == SDLK_ESCAPE) {
            SDL_Log("Escape pressed, quitting...");
            return SDL_APP_SUCCESS;
        }

        //here

    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 now = SDL_GetTicks();
    const float elapsed = ((float) (now - last_time)) / 1000.0f;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // SDL_Color color = {255, 0, 0, 255};
    // float centerX = (float)WINDOW_WIDTH / 2;
    // float cetterY = (float)WINDOW_HEIGHT / 2;

    // draw_circle(renderer, centerX, cetterY, color);
    // printf("elapsed: %f\n", elapsed);
    update_simulation(renderer, elapsed);

    last_time = now;

    SDL_RenderPresent(renderer);
    // SDL_Delay(SIMULATION_DELAY);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{

}
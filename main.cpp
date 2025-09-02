#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>


struct SDLState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
};

void cleanup(SDLState &state);

int main(int argc, char *argv[])
{
    SDLState state;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Event event;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    const int compiled = SDL_VERSION; /* hardcoded number from SDL headers */
    const int linked = SDL_GetVersion(); /* reported by linked SDL library */

    SDL_Log("We compiled against SDL version %d,%d,%d ...\n",
         SDL_VERSIONNUM_MAJOR(compiled),
         SDL_VERSIONNUM_MINOR(compiled),
         SDL_VERSIONNUM_MICRO(compiled));

    SDL_Log("But we are linking against SDL version %d,%d,%d,\n",
         SDL_VERSIONNUM_MAJOR(linked),
         SDL_VERSIONNUM_MINOR(linked),
         SDL_VERSIONNUM_MICRO(linked));

    int width = 320;
    int height = 240;

    // create window
    state.window = SDL_CreateWindow("Byte Racers", width, height, SDL_WINDOW_RESIZABLE);
    if (!state.window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        cleanup(state);
        return 3;
    }

    // create renderer
    state.renderer = SDL_CreateRenderer(state.window, NULL);
    if (!state.renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        cleanup(state);
        return 3;
    }

    // create surface
    surface = SDL_LoadBMP("assets/sample.bmp");
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
        return 3;
    }

    //create texture
    texture = SDL_CreateTextureFromSurface(state.renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
        return 3;
    }


    // game loop
    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }

        // drawing commands
        SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(state.renderer);
        SDL_RenderTexture(state.renderer, texture, NULL, NULL);

        // swap buffers and present
        SDL_RenderPresent(state.renderer);
    }

    cleanup(state);
    return 0;
}

void cleanup(SDLState &state) {
    SDL_DestroySurface(state.surface);
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();

}

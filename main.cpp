#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>


struct SDLState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width, height, logW, logH;
};


bool initialize(SDLState &state);
void cleanup(SDLState &state);

int main(int argc, char *argv[])
{
    SDLState state;
    state.width = 1600;
    state.height = 900;
    state.logW = 640;
    state.logH = 320;

    SDL_Surface *image;
    SDL_Texture *texture;

    if (!initialize(state)) {
        return 3;
    }

    // load game assets
    image = IMG_Load("assets/idle.png");
    if (!image) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());

        return 3;
    }

    // create texture for assets
    texture = SDL_CreateTextureFromSurface(state.renderer, image);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
        return 3;
    }

    // setup game data
    const bool *keys = SDL_GetKeyboardState(NULL);
    float playerX = 0;
    float playerY = 0;
    uint64_t prevTime = SDL_GetTicks();


    // game loop
    bool running = true;
    while (running) {
        uint64_t nowTime = SDL_GetTicks();
        float deltaTime = (nowTime - prevTime) / 1000.0f; //converts to sec
        SDL_Event event{0};
        while (SDL_PollEvent(&event))
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED: {
                    state.width = event.window.data1;
                    state.height = event.window.data2;
                    break;
                }
            }

        // handle movement input
        float moveAmountX = 0;
        float moveAmountY = 0;
        if (keys[SDL_SCANCODE_A]) {
            moveAmountX += -75.0f;
        }
        if (keys[SDL_SCANCODE_D]) {
            moveAmountX += 75.0f;
        }
        if (keys[SDL_SCANCODE_W]) {
            moveAmountY += -75.0f;
        }
        if (keys[SDL_SCANCODE_S]) {
            moveAmountY += 75.0f;
        }
        playerX += moveAmountX * deltaTime;
        playerY += moveAmountY * deltaTime;


        // perform drawing commands
        SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(state.renderer);

        const float spriteSize = 32;
        SDL_FRect src{
            .x = 0,
            .y = 0,
            .w = spriteSize,
            .h = spriteSize
        };

        SDL_FRect dst{
            .x = playerX,
            .y = playerY,
            .w = spriteSize,
            .h = spriteSize
        };

        SDL_RenderTexture(state.renderer, texture, &src, &dst);


        // swap buffers and present
        SDL_RenderPresent(state.renderer);
        prevTime = nowTime;
    }

    SDL_DestroySurface(image);
    SDL_DestroyTexture(texture);
    cleanup(state);
    return 0;
}

bool initialize(SDLState &state) {

    bool initSuccess = true;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        initSuccess = false;
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


    // create window
    state.window = SDL_CreateWindow("Byte Racers", state.width, state.height, SDL_WINDOW_RESIZABLE);
    if (!state.window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        cleanup(state);
        initSuccess = false;
    }

    // create renderer
    state.renderer = SDL_CreateRenderer(state.window, NULL);
    if (!state.renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        cleanup(state);
        initSuccess = false;
    }

    // config presentation
    SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return initSuccess;
}

void cleanup(SDLState &state) {
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();

}

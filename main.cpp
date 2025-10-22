#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "Player.h"
#include "Map.h"
#include "Camera.h"

struct SDLState {
    SDL_Window*   window{};
    SDL_Renderer* renderer{};
    int           winW{1280}, winH{720};
    int           logicalW{0}, logicalH{0};
};

// Set tile size
static const int TILE = 32;

static bool init(SDLState& s) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    s.window = SDL_CreateWindow("Byte Racers", s.winW, s.winH, SDL_WINDOW_RESIZABLE);
    if (!s.window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    // renderer selection; NULL chooses default
    s.renderer = SDL_CreateRenderer(s.window, NULL);
    if (!s.renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }

    // logical presentation
    if (s.logicalW > 0 && s.logicalH > 0) {
        SDL_SetRenderLogicalPresentation(s.renderer, s.logicalW, s.logicalH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    return true;
}

static void shutdown(SDLState& s) {
    if (s.renderer) SDL_DestroyRenderer(s.renderer);
    if (s.window)   SDL_DestroyWindow(s.window);
    SDL_Quit();
}

// tries assets/idle.png, else generates a placeholder via Player::render()
static SDL_Texture* loadCarTexture(SDL_Renderer* ren) {
    SDL_Texture* tex = IMG_LoadTexture(ren, "assets/idle.png");
    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    return tex;
}

int main(int, char**) {
    SDLState s;

    if (!init(s)) { shutdown(s); return 1; }

    // load car sprite (PNG w/ transparent background, oriented “up”)
    SDL_Texture* carTex = loadCarTexture(s.renderer);

    // create  Player in the middle of the current render size
    int rw = s.winW, rh = s.winH;
    if (s.logicalW > 0 && s.logicalH > 0) { rw = s.logicalW; rh = s.logicalH; }
    Player car(float(rw) * 0.5f, float(rh) * 0.5f, -90.f);

    // Create and load a basic map
    Map map;
    std::string mapErr;
    if (!map.loadFromFile("levels/levels_camera_test.txt", TILE, &mapErr))
    {
        // Empty for now
    }

    Camera camera;
    {
        int outW=0, outH=0;
        SDL_GetRenderOutputSize(s.renderer, &outW, &outH);
        camera.setViewport((float)outW, (float)outH);
    }

    // timing   high res
    Uint64 now = SDL_GetPerformanceCounter();
    const auto freq = (double)SDL_GetPerformanceFrequency();

    // steering keys    rate-limited
    bool steerLeft = false, steerRight = false;
    bool running = true;

    while (running) {
        // events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                s.winW = e.window.data1;
                s.winH = e.window.data2;
                camera.setViewport((float)s.winW, (float)s.winH);
            } else if (e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat) {
                if (e.key.key == SDLK_ESCAPE) running = false;
                if (e.key.key == SDLK_LEFT)  steerLeft  = true;
                if (e.key.key == SDLK_RIGHT) steerRight = true;
                if (e.key.key == SDLK_R) { // reset to center
                    int rw2 = s.winW, rh2 = s.winH;
                    if (s.logicalW > 0 && s.logicalH > 0) { rw2 = s.logicalW; rh2 = s.logicalH; }
                    car = Player(float(rw2) * 0.5f, float(rh2) * 0.5f, -90.f);
                }
            } else if (e.type == SDL_EVENT_KEY_UP && !e.key.repeat) {
                if (e.key.key == SDLK_LEFT)  steerLeft  = false;
                if (e.key.key == SDLK_RIGHT) steerRight = false;
            }
        }

        // input    continuous
        const bool* ks = SDL_GetKeyboardState(NULL);
        float throttle = 0.f;
        if (ks[SDL_SCANCODE_UP])   throttle += 1.f;
        if (ks[SDL_SCANCODE_DOWN]) throttle -= 1.f;

        float brake = ks[SDL_SCANCODE_SPACE] ? 1.f : 0.f;

        float steerIn = 0.f;
        if (steerLeft)  steerIn -= 1.f;
        if (steerRight) steerIn += 1.f;

        // timing
        Uint64 newNow = SDL_GetPerformanceCounter();
        auto dt = float((double)(newNow - now) / freq);
        now = newNow;
        if (dt > 1.f/30.f) dt = 1.f/30.f; // clamp spikes

        // update
        car.setInputs(throttle, brake, steerIn);
        car.update(dt, map, TILE);

        // temp simple bounds in current render space
        int curW = s.winW, curH = s.winH;
        if (s.logicalW > 0 && s.logicalH > 0) { curW = s.logicalW; curH = s.logicalH; }

        // Follow player (world size from map)
        const float worldW = (float)map.worldPixelWidth();
        const float worldH = (float)map.worldPixelHeight();
        camera.follow(car.x(), car.y(), worldW, worldH);

        // render
        SDL_SetRenderDrawColor(s.renderer, 24, 28, 32, 255);
        SDL_RenderClear(s.renderer);

        map.render(s.renderer, camera);            // only visible tiles, offset by camera
        car.render(s.renderer, carTex, camera);    // draw player relative to camera

        // simple velocity bar
        float spd = std::abs(car.speed());
        curW = s.winW, curH = s.winH;
        if (s.logicalW > 0 && s.logicalH > 0) { curW = s.logicalW; curH = s.logicalH; }
        SDL_FRect hud { 20.f, float(curH) - 28.f, std::min(spd / 1200.f, 1.f) * 300.f, 8.f };
        SDL_SetRenderDrawColor(s.renderer, 0, 200, 120, 255);
        SDL_RenderFillRect(s.renderer, &hud);

        SDL_RenderPresent(s.renderer);

        SDL_Delay(1);
    }

    if (carTex) SDL_DestroyTexture(carTex);
    shutdown(s);
    return 0;
}

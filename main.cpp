#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include "EnemyCar.h"
#include <vector>
#include <fstream>
#include <string>

struct SDLState {
    SDL_Window*   window{};
    SDL_Renderer* renderer{};
    int           winW{1280}, winH{720};
    int           logicalW{0}, logicalH{0};
};

// Set tile size
static const int TILE = 32;

struct Flag {
    float x, y;
    bool  taken = false;
};

static inline void renderFlag(SDL_Renderer* r, const Camera& cam, const Flag& f) {
    if (f.taken) return;
    const float s = 16.f;
    SDL_FRect fr{ f.x - cam.view.x - s*0.5f, f.y - cam.view.y - s*0.5f, s, s };
    // pole
    SDL_SetRenderDrawColor(r, 210, 210, 210, 255);
    SDL_FRect pole{ fr.x + fr.w * 0.45f, fr.y - 8.f, 3.f, fr.h + 8.f };
    SDL_RenderFillRect(r, &pole);
    // flag
    SDL_SetRenderDrawColor(r, 255, 215, 0, 255);
    SDL_RenderFillRect(r, &fr);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderRect(r, &fr);
}

static inline bool playerTouchesFlag(const Player& p, const Flag& f, float radius = 22.f) {
    float dx = p.x() - f.x, dy = p.y() - f.y;
    return (dx*dx + dy*dy) <= radius*radius;
}

static inline bool allFlagsCollected(const std::vector<Flag>& flags) {
    for (const auto& f : flags) if (!f.taken) return false;
    return true;
}

// Read the same ASCII file the Map loads, and spawn flags at 'F' tile centers.
static void loadFlagsFromAscii(const std::string& levelPath, const Map& map, std::vector<Flag>& out) {
    std::ifstream in(levelPath);
    if (!in) { SDL_Log("Flag scan: cannot open %s", levelPath.c_str()); return; }

    const int TILE = map.tileSize();  // <- use the Map’s tile size (prevents 16 vs 32 mismatches)  :contentReference[oaicite:3]{index=3}
    std::string line;  int row = 0;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back(); // windows CR
        if (!line.empty() && (line[0]==';' || (line.size()>=2 && line[0]=='/' && line[1]=='/'))) { ++row; continue; } // skip comments like Map does  :contentReference[oaicite:4]{index=4}
        for (int col = 0; col < (int)line.size(); ++col) {
            if (line[col] == 'F') {
                float wx = col * TILE + TILE * 0.5f;
                float wy = row * TILE + TILE * 0.5f;
                if (!map.isWallAtPixel(wx, wy)) out.push_back({wx, wy, false});  // safety  :contentReference[oaicite:5]{index=5}
            }
        }
        ++row;
    }
}

static void loadEnemiesFromAscii(const std::string& levelPath,
                                 const Map& map,
                                 std::vector<EnemyCar>& out)
{
    std::ifstream in(levelPath);
    if (!in) { SDL_Log("Enemy scan: cannot open %s", levelPath.c_str()); return; }

    const int T = map.tileSize();
    std::string line; int row = 0;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && (line[0]==';' || (line.size()>=2 && line[0]=='/' && line[1]=='/'))) { ++row; continue; }
        for (int col = 0; col < (int)line.size(); ++col) {
            if (line[col] == 'E') {
                float wx = col * T + T * 0.5f;
                float wy = row * T + T * 0.5f;
                if (!map.isWallAtPixel(wx, wy)) out.emplace_back(wx, wy);
            }
        }
        ++row;
    }
}

static bool loadPlayerSpawnFromAscii(const std::string& levelPath,
                                     const Map& map,
                                     float& outX, float& outY)
{
    std::ifstream in(levelPath);
    if (!in) { SDL_Log("Spawn scan: cannot open %s", levelPath.c_str()); return false; }

    const int T = map.tileSize();
    std::string line; int row = 0;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && (line[0]==';' || (line.size()>=2 && line[0]=='/' && line[1]=='/'))) { ++row; continue; }
        for (int col = 0; col < (int)line.size(); ++col) {
            if (line[col] == 'P') {
                outX = col * T + T * 0.5f;
                outY = row * T + T * 0.5f;
                if (!map.isWallAtPixel(outX, outY)) return true;
            }
        }
        ++row;
    }
    return false;
}


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
    SDL_SetRenderVSync(s.renderer, 1);
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

static SDL_Texture* loadEnemyTexture(SDL_Renderer* ren) {
    SDL_Texture* tex = IMG_LoadTexture(ren, "assets/enemy_n.png");
    if (tex) SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    return tex;
}

int main(int, char**) {
    SDLState s;

    if (!init(s)) { shutdown(s); return 1; }

    // load car sprite (PNG w/ transparent background, oriented “up”)
    SDL_Texture* carTex = loadCarTexture(s.renderer);
    SDL_Texture* enemyTex = loadEnemyTexture(s.renderer);

    static constexpr float kPlayerRadius = 12.0f;
    static constexpr float kEnemyRadius  = 15.0f;

    auto playerHitEnemy = [&](const Player& p, const EnemyCar& e) -> bool {
        const float dx = p.x() - e.x();
        const float dy = p.y() - e.y();
        const float r  = kPlayerRadius + kEnemyRadius;
        return (dx*dx + dy*dy) <= (r * r);
    };

    // create  Player in the middle of the current render size
    int rw = s.winW, rh = s.winH;
    if (s.logicalW > 0 && s.logicalH > 0) { rw = s.logicalW; rh = s.logicalH; }
    Player car(float(rw) * 0.5f, float(rh) * 0.5f, -90.f);

    Camera camera;
    {
        int outW=0, outH=0;
        SDL_GetRenderOutputSize(s.renderer, &outW, &outH);
        camera.setViewport((float)outW, (float)outH);
    }

    // Create and load a basic map
    Map map;
    std::string mapErr;
    std::string levelPath = "levels/level1.txt";   // same path used by Map::loadFromFile
    std::string err;
    if (!map.loadFromFile(levelPath, /*tile=*/32, &err)) { SDL_Log("Map load failed: %s", err.c_str()); }
    camera.setViewport(s.winW, s.winH); // important for correct camera-space drawing  :contentReference[oaicite:6]{index=6}

    std::vector<Flag> flags;
    loadFlagsFromAscii(levelPath, map, flags);
    SDL_Log("Loaded %zu flags", flags.size());

    std::vector<EnemyCar> enemies;
    std::vector<SDL_FPoint> enemySpawns;

    loadEnemiesFromAscii(levelPath, map, enemies);
    enemySpawns.reserve(enemies.size());
    for (const auto& e : enemies) enemySpawns.push_back({e.x(), e.y()});
    SDL_Log("Loaded %zu enemies", enemies.size());

    // timing   high res
    Uint64 now = SDL_GetPerformanceCounter();
    const auto freq = (double)SDL_GetPerformanceFrequency();

    // steering keys    rate-limited
    bool steerLeft = false, steerRight = false;
    bool running = true;

    float spawnX = car.x(), spawnY = car.y();
    if (loadPlayerSpawnFromAscii(levelPath, map, spawnX, spawnY)) {
        car.setPosition(spawnX, spawnY);
        car.setHeading(-90.f);
    }

    int lives = 3;

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

        for (auto& e : enemies)
            e.update(dt, map, car.x(), car.y());

        bool collided = false;
        for (const auto& e : enemies) {
            if (playerHitEnemy(car, e)) {
                collided = true;
                break;
            }
        }

        if (collided) {
            lives--;

            if (lives > 0) {
                // Respawn player at level spawn, fully reset dynamics
                car = Player(spawnX, spawnY, -90.f);

                // Respawn each enemy at its original spawn point
                enemies.clear();
                enemies.reserve(enemySpawns.size());
                for (const auto& p : enemySpawns) {
                    enemies.emplace_back(p.x, p.y);
                }

                // Optional: brief grace period (invuln) – skip updates a few frames, etc.
                // (You can add a timer if you like.)
            } else {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                         "Game Over",
                                         "You ran out of lives!",
                                         s.window);
                running = false;
            }
        }



        // Collect
        for (auto& f : flags) {
            if (!f.taken && playerTouchesFlag(car, f)) {
                f.taken = true;
                // game.collectFlag(); // if you’re using your Game counter
            }
        }

        // Win check
        if (allFlagsCollected(flags)) {
            // Option A: quick native popup (zero extra libs)
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                     "You Win!", "All flags collected!", s.window);

            // Option B: optional in-engine overlay (see below)
            running = false;
            continue; // break out cleanly after showing the message
        }

        map.render(s.renderer, camera);            // only visible tiles, offset by camera
        car.render(s.renderer, carTex, camera);    // draw player relative to camera

        for (auto& e : enemies)
            e.render(s.renderer, enemyTex, camera);
        for (const auto& f : flags) renderFlag(s.renderer, camera, f);
        // simple velocity bar
        float spd = std::abs(car.speed());

        // Draw Lives
        for (int i = 0; i < 3; ++i) {
            SDL_FRect life { 20.f + i * 18.f, float(curH) - 48.f, 12.f, 12.f };
            if (i < lives) SDL_SetRenderDrawColor(s.renderer, 255, 60, 60, 255);
            else           SDL_SetRenderDrawColor(s.renderer, 80, 80, 80, 255);
            SDL_RenderFillRect(s.renderer, &life);
        }

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

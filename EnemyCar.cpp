#include "EnemyCar.h"
#include "Camera.h"
#include <algorithm>
#include <cstdlib>

static inline float deg2rad(float d){ return d * 3.14159265358979323846f / 180.f; }
static inline float rad2deg(float r){ return r * 180.f / 3.14159265358979323846f; }
static inline float clampf(float v,float lo,float hi){ return std::max(lo,std::min(hi,v)); }

EnemyCar::EnemyCar(float x, float y) : x_(x), y_(y) {}

bool EnemyCar::wallAhead(const Map& map, float probeDist) const {
    const float ang = deg2rad(headingDeg_);
    const float nx = x_ + std::cos(ang) * probeDist;
    const float ny = y_ + std::sin(ang) * probeDist;
    return map.isWallAtPixel(nx, ny);
}

bool EnemyCar::canSee(const Map& map, float tx, float ty) const {
    const float dx = tx - x_;
    const float dy = ty - y_;
    const float dist = std::max(1.f, std::sqrt(dx*dx + dy*dy));
    const int   steps = int(dist / 6.f);
    const float stepx = dx / steps;
    const float stepy = dy / steps;
    float px = x_, py = y_;
    for (int i=0;i<steps;i++){
        px += stepx; py += stepy;
        if (map.isWallAtPixel(px, py)) return false;
    }
    return true;
}

void EnemyCar::turnToward(float tx, float ty, float dt){
    const float angTo = rad2deg(std::atan2(ty - y_, tx - x_));
    float delta = angTo - headingDeg_;
    while (delta > 180.f) delta -= 360.f;
    while (delta < -180.f) delta += 360.f;
    const float maxStep = turnRate_ * dt;
    delta = clampf(delta, -maxStep, maxStep);
    headingDeg_ += delta;
}

void EnemyCar::turnAwayFrom(float tx, float ty, float dt){
    const float angFrom = rad2deg(std::atan2(y_ - ty, x_ - tx));
    float delta = angFrom - headingDeg_;
    while (delta > 180.f) delta -= 360.f;
    while (delta < -180.f) delta += 360.f;
    const float maxStep = turnRate_ * dt;
    delta = clampf(delta, -maxStep, maxStep);
    headingDeg_ += delta;
}

void EnemyCar::update(float dt, const Map& map, float playerX, float playerY){
    const bool seePlayer = canSee(map, playerX, playerY);

    switch (mode_){
        case Mode::Patrol:
            if (seePlayer) mode_ = Mode::Chase;
            if (wallAhead(map, 18.f)) {
                float leftAng  = deg2rad(headingDeg_ + 60.f);
                float rightAng = deg2rad(headingDeg_ - 60.f);
                bool leftFree  = !map.isWallAtPixel(x_ + std::cos(leftAng)*18.f,  y_ + std::sin(leftAng)*18.f);
                bool rightFree = !map.isWallAtPixel(x_ + std::cos(rightAng)*18.f, y_ + std::sin(rightAng)*18.f);
                if (leftFree && !rightFree)      headingDeg_ += 60.f;
                else if (rightFree && !leftFree) headingDeg_ -= 60.f;
                else headingDeg_ += (std::rand()%2 ? 50.f : -50.f);
            } else {
                headingDeg_ += (std::rand()%3 - 1) * 20.f * dt;
            }
            speed_ = patrolSpeed_;
            break;

        case Mode::Chase:
            if (!seePlayer) mode_ = Mode::Patrol;
            turnToward(playerX, playerY, dt);
            speed_ = wallAhead(map, 28.f) ? patrolSpeed_ : chaseSpeed_;
            break;

        case Mode::Blinded:
            blindTimer_ -= dt;
            if (blindTimer_ <= 0.f) { mode_ = Mode::Patrol; break; }
            turnAwayFrom(playerX, playerY, dt);
            speed_ = blindedSpeed_;
            break;
    }

    const float ang = deg2rad(headingDeg_);
    float nx = x_ + std::cos(ang) * speed_ * dt;
    float ny = y_ + std::sin(ang) * speed_ * dt;

    if (!map.isWallAtPixel(nx, y_)) x_ = nx;
    if (!map.isWallAtPixel(x_, ny)) y_ = ny;

    if (headingDeg_ > 180.f) headingDeg_ -= 360.f;
    if (headingDeg_ < -180.f) headingDeg_ += 360.f;
}

void EnemyCar::render(SDL_Renderer* r, SDL_Texture* tex, const Camera& cam) const {
    const float scale = 1.0f;
    SDL_FRect dst {
        (x_ - cam.view.x) - (width_ * scale) * 0.5f,
        (y_ - cam.view.y) - (height_ * scale) * 0.5f,
        width_ * scale, height_ * scale
    };
    SDL_FPoint center{ dst.w * 0.5f, dst.h * 0.5f };

    // Base sprite faces UP (north). Our heading uses 0°=+X, 90°=+Y (down).
    // To rotate an UP-facing sprite to match heading, add +90°.
    // If it looks mirrored, use -(headingDeg_ + 90.f) instead.
    const double renderAngle = double(headingDeg_ + 90.f);

    if (tex) {
        SDL_RenderTextureRotated(r, tex, nullptr, &dst, renderAngle, &center, SDL_FLIP_NONE);
    } else {
        SDL_SetRenderDrawColor(r, 220, 50, 50, 255);
        SDL_RenderFillRect(r, &dst);
    }
}

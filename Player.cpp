#include "Player.h"
#include <algorithm>
#include <cmath>


static constexpr float PI = 3.14159265358979323846f;
static inline float toRad(float deg) { return deg * PI / 180.f; }
static inline float toDeg(float rad) { return rad * 180.f / PI; }

float Player::clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }
float Player::sgnf(float v) { return (v > 0.f) - (v < 0.f); }

Player::Player(float x, float y, float headingDeg)
: x_(x), y_(y), heading_(headingDeg) {}

void Player::setInputs(float throttle, float brake, float steer) {
    throttle_ = clampf(throttle, -1.f, 1.f);
    brake_    = clampf(brake,     0.f,  1.f);
    steerIn_  = clampf(steer,    -1.f,  1.f);
}

void Player::update(float dt) {
    // steering    rate-limited, clamped
    if (steerIn_ != 0.f) {
        // active input
        steerDeg_ += steerRate_ * steerIn_ * dt;
    } else {
        // no input, return to center
        if (steerDeg_ > 0.f) {
            steerDeg_ = std::max(0.f, steerDeg_ - steerReturnRate_ * dt);
        }
        else if (steerDeg_ < 0.f) {
            steerDeg_ = std::min(0.f, steerDeg_ + steerReturnRate_ * dt);
        }
    }
    steerDeg_  = clampf(steerDeg_, -maxSteer_, maxSteer_);

    // longitudinal acceleration
    float a = 0.f;
    if (throttle_ > 0.f) a += throttle_ * engineAcc_;
    else if (throttle_ < 0.f) a += throttle_ * reverseAcc_; // negative

    if (brake_ > 0.f) a += brake_ * brakeAcc_ * (v_ != 0.f ? -sgnf(v_) : -1.f);

    // passive losses
    a += -sgnf(v_) * rolling_;
    a += -drag_ * v_ * std::abs(v_);

    // integrate speed
    v_ += a * dt;
    if (std::abs(v_) < 2.f) v_ = 0.f;
    v_ = clampf(v_, -maxSpeed_ * 0.35f, maxSpeed_);

    // heading via bicycle model: ω = v/L * tan(δ)
    float hRad = toRad(heading_);
    const float dRad = toRad(steerDeg_);
    if (std::abs(steerDeg_) > 0.001f) {
        const float yawRate = (v_ / wheelbase_) * std::tan(dRad); // rad/s
        hRad += yawRate * dt;
        heading_ = toDeg(hRad);
    }

    // position update
    x_ += v_ * std::cos(hRad) * dt;
    y_ += v_ * std::sin(hRad) * dt;
}

void Player::render(SDL_Renderer* ren, SDL_Texture* tex) {
    // query texture size
    if (tex && !haveSize_) {
        float w = 0.f, h = 0.f;
        SDL_GetTextureSize(tex, &w, &h);
        if (w > 0.f && h > 0.f) { texW_ = w; texH_ = h; }
        haveSize_ = true;
    }

    const float scale = 1.0f;
    SDL_FRect dst { x_ - (texW_ * scale) * 0.5f,
                    y_ - (texH_ * scale) * 0.5f,
                    texW_ * scale, texH_ * scale };
    SDL_FPoint center { dst.w * 0.5f, dst.h * 0.5f };

    if (tex) {
        // rotate render
        SDL_RenderTextureRotated(ren, tex, NULL, &dst, double(heading_), &center, SDL_FLIP_NONE);
    } else {
        // placeholder
        SDL_SetRenderDrawColor(ren, 30, 140, 230, 255);
        SDL_RenderFillRect(ren, &dst);
    }
}



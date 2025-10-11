#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include "Map.h"


class Player {
public:
    // construct at (x,y), heading degrees , -90° = "up" sprite.
    Player(float x = 640.f, float y = 360.f, float headingDeg = -90.f);

    // inputs each frame: throttle [-1,1], brake [0,1], steer [-1,1] (L..R)
    void setInputs(float throttle, float brake, float steer);
    void update(float dtSeconds, Map &map, int tileSize);

    // render the car rotated to its physical heading. ff tex == NULL, draws a placeholder.
    void render(SDL_Renderer* ren, SDL_Texture* tex);

    // accessors / utilities
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    void setHeading(float deg) { heading_ = deg; }
    void setSteerReturnRate(float degPerSec) {steerReturnRate_ = std::max(0.f, degPerSec);}
    float x() const { return x_; }
    float y() const { return y_; }
    float heading() const { return heading_; }
    float speed() const { return v_; }


private:
    // state
    float x_ = 0.f;
    float y_ = 0.f;
    float heading_ = -90.f;     // degrees
    float v_ = 0.f;             // px/s (forward +, reverse -)
    float steerDeg_ = 0.f;      // wheel angle in degrees (relative to body)

    // inputs
    float throttle_ = 0.f;      // [-1,1]
    float brake_ = 0.f;         // [0,1]
    float steerIn_ = 0.f;       // [-1,1]

    // tunables
    float wheelbase_  = 85.f;       // pixels
    float maxSteer_   = 28.f;       // deg
    float steerRate_  = 140.f;      // deg/s
    float steerReturnRate_ = 220.f; //
    float engineAcc_  = 650.f;      // px/s^2
    float brakeAcc_   = 950.f;      // px/s^2
    float reverseAcc_ = 400.f;      // px/s^2
    float rolling_    = 3.f;        // px/s^2
    float drag_       = 0.005f;     // quadratic air resistance
    float maxSpeed_   = 1200.f;     // px/s

    // cached texture size for rendering
    bool  haveSize_ = false;
    float texW_ = 90.f, texH_ = 48.f;  // fallback if texture unknown

    static float clampf(float v, float lo, float hi);
    static float sgnf(float v);
};

#endif //PLAYER_H

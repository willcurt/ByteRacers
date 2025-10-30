#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include "Map.h"
class Camera; // forward declaration

class EnemyCar {
public:
    enum class Mode { Patrol, Chase, Blinded };

    EnemyCar(float x, float y);

    void update(float dt, const Map& map, float playerX, float playerY);
    void render(SDL_Renderer* r, SDL_Texture* tex, const Camera& cam) const;

    // temporarily blinds the enemy (e.g. smoke)
    void blind(float seconds) { mode_ = Mode::Blinded; blindTimer_ = seconds; }

    // accessors
    float x() const { return x_; }
    float y() const { return y_; }
    float heading() const { return headingDeg_; }
    Mode  mode() const { return mode_; }

private:
    // helpers
    void turnToward(float targetX, float targetY, float dt);
    void turnAwayFrom(float targetX, float targetY, float dt);
    bool wallAhead(const Map& map, float probeDist) const;
    bool canSee(const Map& map, float tx, float ty) const;

    // state
    float x_{}, y_{};
    float headingDeg_{0.f}; // 0° = +X, 90° = +Y
    float speed_{90.f};
    Mode  mode_{Mode::Patrol};
    float blindTimer_{0.f};

    // tuning
    float chaseSpeed_{130.f};
    float patrolSpeed_{90.f};
    float blindedSpeed_{50.f};
    float turnRate_{220.f};   // deg/sec
    float width_{46.f}, height_{26.f}; // visual car size
};

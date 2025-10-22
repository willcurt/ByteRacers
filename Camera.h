#pragma once
#include <SDL3/SDL.h>
#include <algorithm>

class Camera {
public:
    // The visible area in world coordinates
    SDL_FRect view{0.f, 0.f, 0.f, 0.f};

    // How fast the camera follows the target (0 < lerpFactor ≤ 1)
    float lerpFactor = 0.1f;  // smaller = smoother, slower

    void setViewport(float width, float height) {
        view.w = width;
        view.h = height;
    }

    void follow(float targetX, float targetY, float worldW, float worldH) {
        // Desired center position (player at middle of screen)
        float desiredX = targetX - view.w * 0.5f;
        float desiredY = targetY - view.h * 0.5f;

        // Smoothly approach desired position
        view.x += (desiredX - view.x) * lerpFactor;
        view.y += (desiredY - view.y) * lerpFactor;

        // Clamp camera to world boundaries
        view.x = std::max(0.0f, std::min(view.x, worldW - view.w));
        view.y = std::max(0.0f, std::min(view.y, worldH - view.h));
    }
};

#pragma once
#include <algorithm>
#include <cmath>

class Player {
private:
    float x, y;            // position
    float velX, velY;      // velocity
    float acceleration;    // acceleration rate
    float friction;        // natural slowing
    float maxSpeed;        // maximum speed

public:
    // Constructor
    Player(float startX = 0, float startY = 0);

    // Update loop
    void update();

    // Movement controls
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    // Getters
    float getX() const;
    float getY() const;
    float getVelX() const;
    float getVelY() const;

private:
    void clampSpeed();
};

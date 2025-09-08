#include <algorithm>
#include <cmath>

class Player {
private:
    float x, y;            // position in the maze
    float velX, velY;      // velocity in each direction
    float acceleration;    // how fast the car accelerates
    float friction;        // natural slowing down
    float maxSpeed;        // max speed clamp

public:
    // Constructor
    Player(float startX = 0, float startY = 0)
        : x(startX), y(startY), velX(0), velY(0),
          acceleration(0.2f), friction(0.05f), maxSpeed(3.0f) {}

    // Update position each frame (apply velocity & friction)
    void update() {
        // Apply friction (natural slowing when no input)
        if (velX > 0) velX = std::max(0.0f, velX - friction);
        else if (velX < 0) velX = std::min(0.0f, velX + friction);

        if (velY > 0) velY = std::max(0.0f, velY - friction);
        else if (velY < 0) velY = std::min(0.0f, velY + friction);

        // Update position
        x += velX;
        y += velY;
    }

    // Movement controls (accelerate in direction)
    void moveUp() {
        velY -= acceleration;
        clampSpeed();
    }
    void moveDown() {
        velY += acceleration;
        clampSpeed();
    }
    void moveLeft() {
        velX -= acceleration;
        clampSpeed();
    }
    void moveRight() {
        velX += acceleration;
        clampSpeed();
    }

    // Clamp velocity to max speed
    void clampSpeed() {
        if (velX > maxSpeed) velX = maxSpeed;
        if (velX < -maxSpeed) velX = -maxSpeed;
        if (velY > maxSpeed) velY = maxSpeed;
        if (velY < -maxSpeed) velY = -maxSpeed;
    }

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getVelX() const { return velX; }
    float getVelY() const { return velY; }
};

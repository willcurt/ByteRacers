#include "PlayerCar.h"

// Constructor initializes position and speed
Player::Player(int startX, int startY) {
    x = startX;
    y = startY;
    speed = 1; // move 1 unit per step
}

// Movement methods
void Player::moveUp() { y -= speed; }
void Player::moveDown() { y += speed; }
void Player::moveLeft() { x -= speed; }
void Player::moveRight() { x += speed; }

// Getters
int Player::getX() const { return x; }
int Player::getY() const { return y; }

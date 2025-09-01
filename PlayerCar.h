#pragma once

class Player {
private:
    int x, y;      // position in the maze
    int speed;     // how many units the player moves per step

public:
    // Constructor
    Player(int startX = 0, int startY = 0);

    // Movement
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    // Getters
    int getX() const;
    int getY() const;
};
#pragma once
#include <string>
#include <vector>
#include <utility> // for std::pair

using namespace std;

// EnemyCar represents an AI-controlled car that chases the player in a maze.
class EnemyCar {
private:
    int x, y;               // Current position
    int speed;              // Movement speed (tiles or pixels per update)

    // Helper function: BFS to find the next step toward the player
    pair<int,int> findNextStep(int playerX, int playerY, const vector<vector<int>>& map);

public:
    // Constructor
    EnemyCar(int startX, int startY, int carSpeed);

    // Update enemy position to chase player using BFS
    void updatePosition(int playerX, int playerY, const vector<vector<int>>& map);

    // Getters
    int getX() const;
    int getY() const;

    // Debugging helper
    string getStatus() const;
};

#include "EnemyCar.h"
#include <vector>
#include <queue>
#include <utility>   // for pair
#include <string>
#include <iostream>

using namespace std;

// Constructor: sets initial position and speed
EnemyCar::EnemyCar(int startX, int startY, int carSpeed) {
    x = startX;
    y = startY;
    speed = carSpeed;
}

// Helper function: find the next tile to move toward the player using BFS
pair<int,int> EnemyCar::findNextStep(int playerX, int playerY, const vector<vector<int>>& map) {
    int rows = map.size();
    int cols = map[0].size();

    // Create visited grid (all false)
    vector<bool> rowVisited(cols, false);
    vector<vector<bool>> visited(rows, rowVisited);

    // Create parent grid to reconstruct path
    vector<pair<int,int>> parentRow(cols, {-1, -1});
    vector<vector<pair<int,int>>> parent(rows, parentRow);

    // BFS queue
    queue<pair<int,int>> q;
    q.push({x, y});
    visited[y][x] = true;

    // Directions: up, down, left, right
    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};

    // BFS loop
    while (!q.empty()) {
        pair<int,int> current = q.front();
        q.pop();
        int cx = current.first;
        int cy = current.second;

        // Reached player
        if (cx == playerX && cy == playerY) {
            // Trace back one step toward enemy start
            while (parent[cy][cx] != make_pair(x, y)) {
                tie(cx, cy) = parent[cy][cx];
            }
            return {cx, cy};
        }

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            // Check bounds and walkable tile
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
                map[ny][nx] == 0 && !visited[ny][nx]) {
                visited[ny][nx] = true;
                parent[ny][nx] = {cx, cy};
                q.push({nx, ny});
            }
        }
    }

    // No path found, stay in place
    return {x, y};
}

// Update enemy position each tick
void EnemyCar::updatePosition(int playerX, int playerY, const vector<vector<int>>& map) {
    pair<int,int> next = findNextStep(playerX, playerY, map);
    x = next.first;
    y = next.second;
}

// Getters
int EnemyCar::getX() const { return x; }
int EnemyCar::getY() const { return y; }

// For debugging
string EnemyCar::getStatus() const {
    return "Enemy at (" + to_string(x) + ", " + to_string(y) + ")";
}

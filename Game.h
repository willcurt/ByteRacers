#pragma once
/*
 same as...
    #ifndef PLAYER_H
    #define PLAYER_H
*/

#include <vector>
#include <string>
#include <iostream>
using std::string;  // Use this over using namespace std to avoid polluting the global namespace. (Safer option for a header file)

class Game{
private:
    int score;
    int lives;
    int totalFlags;
    int collectedFlags;
    bool running;
    
public:
    // Constructor
    Game(int totalFlags = 5, int lives = 3);

    // Game state methods
    void start();
    void update();
    void end();

    // Score and flag handling
    void collectFlag();
    int getScore() const;
    int getLives() const;
    int getRemainingFlags() const;

    // Utlility
    bool isRunning() const;             // Returns true if the game is still active, false if over (win/loss)
    string getGameStatus() const;       // Returns a string summarizing score, lives, and remaining flags
};
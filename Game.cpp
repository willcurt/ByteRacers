#include "Game.h"
#include <iostream>
using namespace std;

// Constructor initializes the game state
Game::Game(int totalFlagsInput, int livesInput){
    score = 0;
    lives = livesInput;     // Initialize from parameter
    totalFlags = totalFlagsInput;
    collectedFlags = 0;
    running = false;
}

// Start the game
void Game::start() {
    running = true;
    score = 0;
    collectedFlags = 0;
    cout << "Game started! Collect all " << totalFlags << " flags." << endl;
}

// Called every "tick" of the game (later SDL will call this per frame)
void Game::update() {
    if (collectedFlags >= totalFlags) {
        cout << "All flags collected! You win!" << endl;
        end();
    }

    if (lives <= 0) {
        cout << "Game over!" << endl;
        end();
    }
}

// Ends the game
void Game::end() {
    running = false;
    cout << "Final Score: " << score << endl;
}

// Flag collection
void Game::collectFlag() {
    if (running && collectedFlags < totalFlags) {
        collectedFlags++;
        score += 100; // arbitrary points per flag
        cout << "Flag collected! (" << collectedFlags << "/" << totalFlags << ")" << endl;
    }
}

// Getters
int Game::getScore() const { return score; }
int Game::getLives() const { return lives; }
int Game::getRemainingFlags() const { return totalFlags - collectedFlags; }
bool Game::isRunning() const { return running; }

// Status report
string Game::getGameStatus() const {
    return "Score: " + to_string(score) +
           " | Lives: " + to_string(lives) +
           " | Flags left: " + to_string(getRemainingFlags());
}
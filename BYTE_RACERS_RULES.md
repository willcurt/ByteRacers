# Overview

- Rally X is a classic arcade game from 1980. The player controls a small car in a maze-like environment, collecting flags while avoiding enemy cars. The game ends when the player loses all lives, and the goal is to achieve the highest score possible.

- This document explains the rules and logic of our version of Rally X, which will be implemented in C++ with SDL3.

# Player Car

- The player starts with 3 lives.

- The car can move up, down, left, right using the keyboard.

- The player loses a life if:

- Colliding with an enemy car.

- Running out of lives = game over.

# Smoke Screen

- The player has a limited supply of smoke screens.

- When activated, it temporarily disables enemy cars that drive into the smoke.

# Flags

- Flags are scattered around the maze.

- Collecting a flag increases the score by 100 points.

- The level is complete when all flags are collected.

# Enemy Cars

- Enemy cars move around the maze and chase the player.

- Their movement will be controlled by search algorithms (such as Breadth-First Search or Depth-First Search) to simulate intelligent pursuit.

- If an enemy collides with the player, the player loses one life.

# Scoring

- Flag collected: +100 points.

- Surviving a level: Bonus points (e.g., +1000).

- Enemy car disabled by smoke: Optional extra points (to be decided).

- Score is displayed during gameplay.

# Win & Loss Conditions

- Win condition: Collect all flags to finish the level.

- Loss condition: Lose all lives → Game Over.

# Planned Features

- Level progression: Later levels may have more enemies or faster enemies.

- Stretch goals:

- Sound effects and background music.

- High score tracking.

- Multiple maps.
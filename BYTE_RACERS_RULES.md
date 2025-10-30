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

## Win Conditions

The player wins a level by successfully collecting all flags scattered throughout the maze while avoiding enemy cars and managing fuel. When all flags have been collected:

- The level transitions to a "Stage Clear" or "Level Complete" state.
- Remaining fuel is converted into bonus points (for example, remaining fuel × 10).
- A summary screen or message is displayed before progressing to the next level.
- Higher levels may increase maze complexity, the number of flags, or enemy speed.

Optional variations:

- Collecting a Special Flag grants an instant bonus (e.g., +1,000 points) and may automatically award points for all remaining regular flags.
- Completing a level without using any smoke screens could provide an additional skill bonus.

**Implementation note:** Win detection occurs when the number of collected flags equals the total flags on the map. This check typically occurs inside the main game loop.

## Loss Conditions

The player loses a life when colliding with an enemy car or running out of fuel. Each lost life triggers a short animation before restarting the current level.

The player loses one life when:

- The player car collides with an enemy car.
- The fuel meter reaches zero.

After losing a life:

- The player respawns at the starting position.
- Enemies and flags reset to their default positions.
- Fuel and smoke supplies are replenished for that life.
- Score and collected flags are not reset unless all lives are lost.

The player reaches Game Over when all three lives are lost. The game transitions to a Game Over screen displaying the final score and options such as "Play Again" or "Main Menu."

Optional extensions:

- A fuel warning indicator (visual or sound) when fuel is low.
- A checkpoint or continue system in later levels.

**Implementation note:** Loss detection occurs during the update loop. If a collision with an enemy is detected or fuel reaches zero, a life is lost. If all lives are lost, the game state is set to Game Over.

## Gameplay Flow Summary

1. Start Level → Player begins with 3 lives, full fuel, and limited smoke screens.
2. During Play → Player collects flags, avoids enemies, and manages fuel.
3. Win Case → All flags collected → Stage Clear → Bonus points → Next Level.
4. Lose Life → Collision or no fuel → Respawn if lives remain.
5. Lose Game → All lives lost → Game Over → Display final score.


# Planned Features

- Level progression: Later levels may have more enemies or faster enemies.

- Stretch goals:

- Sound effects and background music.

- High score tracking.

- Multiple maps.
